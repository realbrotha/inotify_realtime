//
// Created by realbro on 4/16/20.
//

#include "InotiMgr.h"
#include "EpollWrapper.h"

#include <unistd.h>
#include <sys/inotify.h>

#include <string>
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

  std::string target_path = "/home/realbro/test";
  watch_fd = inotify_add_watch(inoti_fd, target_path.c_str(), IN_MODIFY | IN_CREATE | IN_DELETE);
  if (watch_fd < 0) {
    std::cout << "inoti add error";
    return false;
  }

  std::string target_path2 = "/home/realbro/test2";
  watch_fd = inotify_add_watch(inoti_fd, target_path2.c_str(), IN_MODIFY | IN_CREATE | IN_DELETE);
  if (watch_fd < 0) {
    std::cout << "inoti add error";
    return false;
  }

  if (!EpollWrapper::EpollCreate(1, true, epoll_fd)) {
    std::cout << "epoll Create Failed" << std::endl;
    return false;
  }

  if (!EpollWrapper::EpollControll(epoll_fd,
                                   inoti_fd,
                                   EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET,
                                   EPOLL_CTL_ADD)) {
    std::cout << "epoll add failed" << std::endl;
    return false;
  }
  std::thread epoll_tracking(&InotiMgr::EpollHandler, this);
  epoll_tracking.detach();
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