//
// Created by realbro on 4/16/20.
//
#include "InotifyEventHandler.h"
#include "InotifyWatchMgr.h"
#include "InotifyMask.h"

#include "common/EpollWrapper.h"
#include "common/FileDescriptorTool.h"
#include "boost/scope_exit.hpp"

#include <unistd.h>
#include <ftw.h>

#include <array>
#include <thread>

namespace {
constexpr int kMAX_INOTIFY_EVENT_SIZE = 10;
}
std::atomic<bool> InotifyEventHandler::stopped_(false);

InotifyEventHandler::InotifyEventHandler(RealtimeConfig &config) : inotify_fd_(-1), epoll_fd_(-1), config_(config) {

}
InotifyEventHandler::~InotifyEventHandler() {
  Finalize();
}

void InotifyEventHandler::Finalize() {
  stopped_ = true;

  if (-1 != inotify_fd_) {
    close(inotify_fd_);
  }
  if (-1 != epoll_fd_) {
    close(epoll_fd_);
  }
}

bool InotifyEventHandler::Initialize() {
  printf("EventHandler Start \n");
  int inotify_fd = inotify_init();
  if (inotify_fd < 0) {
    return false;
  }

  if (!InotifyWatchMgr::GetInstance().Initialize(inotify_fd))
    return false;

  inotify_fd_= inotify_fd;
  FileDescriptorTool::SetCloseOnExec(inotify_fd_, true);
  FileDescriptorTool::SetNonBlock(inotify_fd_, true);

  BOOST_SCOPE_EXIT(this_, &inotify_fd_) {
      if (this_->stopped_ && inotify_fd_ != -1) {
        close(inotify_fd_);
      }
    }
  BOOST_SCOPE_EXIT_END

  int epoll_fd = epoll_create1(0);
  if (epoll_fd < 0) {
    printf("epoll_fd failed.\n");
    return false;
  }
  epoll_fd_ = epoll_fd;

  FileDescriptorTool::SetCloseOnExec(epoll_fd_, true);
  FileDescriptorTool::SetNonBlock(epoll_fd_, true);

  BOOST_SCOPE_EXIT(this_, &epoll_fd_) {
      if (this_->stopped_ && epoll_fd_ != -1) {
        close(epoll_fd_);
      }
    }
  BOOST_SCOPE_EXIT_END

  stopped_ = false;
  printf("EventHandler Done \n");
  return true;
}

int InotifyEventHandler::FtwCallback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
  if (stopped_) {
    return -1;
  }
  char real_path[1024] = {0,};

  if (FTW_D != typeflag || !realpath(fpath, real_path)) {
    return 0;
  }
  std::string target_path(real_path);

  // todo : 예외 경로 처리

  if (!InotifyWatchMgr::GetInstance().AddWatch(target_path)) {
    printf("AddWatch Failed. (out of buffer size)\n");
    return -1;
  }

  return 0;
}
void InotifyEventHandler::DirectoryTree() {
  printf("directory start\n");
  for (auto &it : config_.GetDetectList()) {
    printf("path : %s\n", it.c_str());
    if (0 > nftw(it.c_str(), FtwCallback, 1, FTW_PHYS)) {
      printf("Directory Abort.\n");
    }
  }
}

bool InotifyEventHandler::ParseRawEvent(std::deque<struct customized_inotify_event> &list) {
  // TODO : 멀티스레드 형태로 될거면 lock 아니면 안걸어도 됨
  struct customized_inotify_event &event = list.front();

  std::string mask = InotifyMaskToString(event.mask);
  printf("event mask : %s\n", mask.c_str());
  printf("event from : %d\n", event.wd);
  printf("event cokie : %d\n", event.cookie);
  printf("file size : %d\n", event.len);
  printf("file name : %s\n", event.name.c_str());
  std::string target = InotifyWatchMgr::GetInstance().GetPath(event.wd) + "/" + event.name;
  printf("full path : %s\n", target.c_str());
  printf("******************************************\n");

  list.pop_front();
}

void InotifyEventHandler::EpollHandler() {
  epoll_event settingEvent = {0, {0}};
  settingEvent.events = EPOLLIN;

  if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, inotify_fd_, &settingEvent)) {
    printf("Epoll add failed\n");
    return;
  }

  std::deque<struct customized_inotify_event> inotify_event_buffer;

  int errorCount = 0;
  while (!stopped_) {
    epoll_event gettingEvent = {0, {0}};
    int rc = epoll_wait(epoll_fd_, &gettingEvent, 1, 10 /* ms */);
    if (rc < 0) {
      if (errno == EINTR) {
        continue;
      }
      ++errorCount;
      if (errorCount < 3) {
        continue;
      } else {
        printf("ERROR Count 3!!\n");
        return;
      }
    } else if (rc == 0) {
      continue; // empty I/O or none descriptor 정확한지는 찾아봐야됨.
    } else if (rc > 0) {     // event.
      while (!stopped_) {
        if (inotify_event_buffer.size() >= kMAX_INOTIFY_EVENT_SIZE) {
          break;
        }
        std::array<char, 1024> read_buffer;

        ssize_t currentReadSize = read(inotify_fd_, read_buffer.data(), read_buffer.size());
        if (currentReadSize <= 0) {
          if (errno == EINTR || errno == ETXTBSY) {
            std::this_thread::yield(); // 유지
            errno = 0;

            continue;
          }

          if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINVAL) {
            break;
          }

          if (errno == ENFILE) {
            std::this_thread::yield();
          }
          // TODO: 반복해서 Error 발생할 때의 처리 추가
          continue;
        } else if (currentReadSize > 1024) {
          printf("read buffer over ~~~ skip this\n");
          continue;
        } else { //event success
          errorCount = 0; // err count reset
          int read_size = currentReadSize;
          int start_pos = 0;
          while (read_size >= 16) {  //데이터가 한방에 몽창 들어오는경우가 많음. 기본 16바이트에다가 len 만큼 처리하는걸로 우회 .
            struct inotify_event
                *event_buffer = reinterpret_cast<struct inotify_event *>(read_buffer.data() + start_pos);

            customized_inotify_event stack;
            stack.cookie = event_buffer->cookie;
            stack.len = event_buffer->len;
            stack.wd = event_buffer->wd;
            stack.mask = event_buffer->mask;
            stack.name = event_buffer->name;

            start_pos += stack.len + 16;
            read_size -= stack.len + 16;

            inotify_event_buffer.push_back(stack);
          }
        }
      }
    }

    while (true) {
      if (!ParseRawEvent(inotify_event_buffer)) {
        if (!inotify_event_buffer.empty()) {
          continue;
        } else {
          break;
        }
      }
    }
  }
}