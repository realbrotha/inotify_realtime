//
// Created by realbro on 4/16/20.
//
#include "InotifyEventHandler.h"
#include "custom/EpollWrapper.h"

#include <unistd.h>

#include <iostream>
#include <thread>

namespace {
constexpr int kEPOLL_MAX_EVENT_COUNT = 3;
}

InotifyEventHandler &InotifyEventHandler::GetInstance() {
  static InotifyEventHandler instance_; // TODO : 싱글톤은 원래 이렇게 쓰면 안됨. 나중에 수정할것
  return instance_;
}
InotifyEventHandler::InotifyEventHandler() {

}
InotifyEventHandler::~InotifyEventHandler() {
  printf("InotifyEventHandler terminated.\n");
}

bool InotifyEventHandler::Init() {
  //if (ptr != nullptr) callback_func_ = ptr;
  int inoti_fd = inotify_init();
  if (inoti_fd < 0) {
    return false;
  }

  int epoll_fd = -1;
  if (!EpollWrapper::EpollCreate(1, true, epoll_fd)) {
    printf("epoll Create Failed\n");
    return false;
  }
  if (!EpollWrapper::EpollControll(epoll_fd,
                                   inoti_fd,
                                   EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET,
                                   EPOLL_CTL_ADD)) {
    printf("epoll add failed. epoll_fd : %d\n", epoll_fd);
    return false;
  }
  printf ("inoti [%d], epoll [%d]",inoti_fd, epoll_fd);
  FdMgr::GetInstance().Init(inoti_fd, epoll_fd);

  DirectoryMgr directory_mgr;
  directory_mgr.Init();

  std::thread epoll_tracking(&InotifyEventHandler::EpollHandler, this);
  epoll_tracking.detach();
}

bool InotifyEventHandler::AddWatch(std::string &target_path) {
  bool result = false;
  int watch_fd = inotify_add_watch(FdMgr::GetInstance().GetInotiFd(), target_path.c_str(), IN_MODIFY | IN_CREATE | IN_DELETE);

  printf("path : %s\n", target_path.c_str());
  printf("watch_fd : %d\n", watch_fd);
  if (watch_fd < 0) {
    printf("add fail.\n");
  } else {
    FdMgr::GetInstance().AddFd(watch_fd, target_path);
    result = true;
  }
  return result;
}

bool InotifyEventHandler::RmWatch(int watch_fd) {
  return  (0 == inotify_rm_watch(FdMgr::GetInstance().GetInotiFd(), watch_fd)) ? true : false;
}

void InotifyEventHandler::EventCallback(const struct inotify_event *event) {
  if (event->mask & IN_ISDIR) { // dir
    printf("event from folder : %d\n", event->wd);
    if (event->mask & IN_DELETE) { //
      printf("folder remove\n"); // 별도로 처리할 필요없음 삭제되면 IN_IGNORED 이벤트가 먼저 발생되면서 이벤트목록에서 사라짐.
      //std::string base_path = FdMgr::GetInstance().GetPath(event->wd);
      //base_path += "/";
      //base_path += event->name;
      //InotifyEventHandler::GetInstance().RmWatch(FdMgr::GetInstance().GetWd(base_path));
    } else if (event->mask & IN_CREATE) {
      printf("folder create\n");
      std::string base_path = FdMgr::GetInstance().GetPath(event->wd);
      base_path += "/";
      base_path += event->name;
      InotifyEventHandler::GetInstance().AddWatch(base_path);
    }
  } else { // file ??
    if (event->mask & IN_ACCESS ||
        event->mask & IN_OPEN ||
        event->mask & IN_CREATE ||
        event->mask & IN_CLOSE_NOWRITE ||
        event->mask & IN_CLOSE_WRITE ||
        event->mask & IN_MODIFY) {
      printf("*************** file event ***************\n");
      printf("event from : %d\n", event->wd);
      printf("event [%08x]\n", event->mask);
      printf("event cokie %d\n", event->cookie);
      printf("file name = %s\n", event->name);
      printf("******************************************\n");
    }
  }

}

void *InotifyEventHandler::EpollHandler(void *arg) {
  InotifyEventHandler *mgr = reinterpret_cast<InotifyEventHandler *>(arg);
  mgr->stopped_ = false;

  int errorCount = 0;
  while (!mgr->stopped_) {
    epoll_event gettingEvent[kEPOLL_MAX_EVENT_COUNT] = {0, {0}};
    int event_count = epoll_wait(FdMgr::GetInstance().GetEpollFd(), gettingEvent, kEPOLL_MAX_EVENT_COUNT, 10 /* ms */);

    if (event_count > 0) // 이벤트 떨어짐.  event_count = 0 처리안함.
    {
      for (int i = 0; i < event_count; ++i) {
        printf("epoll event index [%d], event type [%d]\n", i, gettingEvent[i].events);

        if (gettingEvent[i].data.fd) {
          char *ptr;
          char buff[4096] = {0,};
          int size = read(gettingEvent[i].data.fd, buff, sizeof(buff));
          if (size == -1 && errno != EAGAIN) {
            printf("read");
            break;
          }
          if (size <= 0)
            break;

          const struct inotify_event *event;
          for (ptr = buff; ptr < buff + size; ptr += sizeof(struct inotify_event) + event->len) {
            event = (struct inotify_event *) ptr;
            EventCallback(event);
          }
        }
      }
    }
  }

  std::cout << "EpollHandler Thread!!!! END OF Thread" << std::endl;
}

