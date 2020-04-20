//
// Created by realbro on 4/16/20.
//

#include "InotiMgr.h"
#include "EpollWrapper.h"
#include "boost/filesystem.hpp"

#include <unistd.h>
#include <sys/inotify.h>

#include <iostream>
#include <thread>

namespace {
constexpr int kEPOLL_MAX_EVENT_COUNT = 3;
}
InotiMgr::InotiMgr() : inoti_fd(0), watch_fd(0), epoll_fd(0) {

}
InotiMgr::~InotiMgr() {
  Finalize();
}

bool InotiMgr::Init() {
  inoti_fd = inotify_init();
  if (inoti_fd < 0) {
    return false;
  }
  if (!EpollWrapper::EpollCreate(1, true, epoll_fd)) {
    printf("epoll Create Failed\n");
    return false;
  }

  std::string target_path = "/home/realbro/test";
  AddItem(target_path,inoti_fd, watch_fd);

  std::thread epoll_tracking(&InotiMgr::EpollHandler, this);
  epoll_tracking.detach();
}

bool InotiMgr::AddItem(std::string &target_path, int &i_fd, int &w_fd) {
  w_fd = inotify_add_watch(i_fd, target_path.c_str(), IN_MODIFY | IN_CREATE | IN_DELETE);
  if (w_fd < 0) {
    printf("add fail.\n");
    return false;
  }
  if (!EpollWrapper::EpollControll(epoll_fd,
                                   inoti_fd,
                                   EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET,
                                   EPOLL_CTL_ADD)) {
    printf("epoll add failed.\n");
    return false;
  }
  return true;
}
bool InotiMgr::RemoveItem(){
  // not available.
  // TODO : inotify 는 path를 특정해서 등록을 뺄 수 없다 . 각기 다른 fd로 처리하면 사용 가능할텐데 fd 가 너무 많아서 고민중 ...
}

void InotiMgr::MaskCheck(const struct inotify_event *event) {
  if (event->mask & IN_ACCESS)
    printf("IN_ACCESS ");
  if (event->mask & IN_ATTRIB)
    printf("IN_ATTRIB ");
  if (event->mask & IN_CLOSE_NOWRITE)
    printf("IN_CLOSE_NOWRITE ");
  if (event->mask & IN_CLOSE_WRITE)
    printf("IN_CLOSE_WRITE ");
  if (event->mask & IN_CREATE)
    printf("IN_CREATE ");
  if (event->mask & IN_DELETE)
    printf("IN_DELETE ");
  if (event->mask & IN_ISDIR)
    printf("IN_ISDIR ");
  if (event->mask & IN_MODIFY)
    printf("IN_MODIFY ");
  if (event->mask & IN_MOVE_SELF)
    printf("IN_MOVE_SELF ");
  if (event->mask & IN_MOVED_FROM)
    printf("IN_MOVED_FROM ");
  if (event->mask & IN_MOVED_TO)
    printf("IN_MOVED_TO ");
  if (event->mask & IN_OPEN)
    printf("IN_OPEN ");
  if (event->len > 0)
    printf(": name = %s\n", event->name);
}

void *InotiMgr::EpollHandler(void *arg) {
  InotiMgr *mgr = reinterpret_cast<InotiMgr *>(arg);
  mgr->stopped_ = false;

  int errorCount = 0;
  while (!mgr->stopped_) {
    epoll_event gettingEvent[kEPOLL_MAX_EVENT_COUNT] = {0, {0}};
    int event_count = epoll_wait(mgr->epoll_fd, gettingEvent, kEPOLL_MAX_EVENT_COUNT, 10 /* ms */);

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
            MaskCheck(event);
          }
        }
      }
    }
  }

  std::cout << "EpollHandler Thread!!!! END OF Thread" << std::endl;
}

bool InotiMgr::Finalize() {
  inotify_rm_watch(inoti_fd, watch_fd);
}