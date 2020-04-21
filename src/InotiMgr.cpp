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
InotiMgr::InotiMgr() : inoti_fd(-1), watch_fd(-1), epoll_fd(-1) {

}
InotiMgr::~InotiMgr() {
  printf ("InotiMgr terminated.\n");
  Finalize();
}
InotiMgr& InotiMgr::GetInstance() {
  static InotiMgr instance_; // TODO : 싱글톤은 원래 이렇게 쓰면 안됨. 나중에 수정할것
  return instance_;
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
  if (!EpollWrapper::EpollControll(epoll_fd,
                                   inoti_fd,
                                   EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET,
                                   EPOLL_CTL_ADD)) {
    printf("epoll add failed. epoll_fd : %d\n",epoll_fd);
    return false;
  }
  printf("epoll create epoll_fd : %d\n",epoll_fd);
  DirectoryMgr directory_mgr;
  directory_mgr.Init();

  std::thread epoll_tracking(&InotiMgr::EpollHandler, this);
  epoll_tracking.detach();
}
int succ_count = 0;
bool InotiMgr::AddItem(std::string &target_path) {
  if (inoti_fd < 0){
    printf ("need init");
    return false;
  }
  printf ("path : %s\n",target_path.c_str());
  watch_fd = inotify_add_watch(inoti_fd, target_path.c_str(), IN_MODIFY | IN_CREATE | IN_DELETE);
  if (watch_fd < 0) {
    printf("add fail. succ :%d\n",succ_count);
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
  if (event->mask ^ IN_MOVE)
    printf ("move ???");
  if (event->mask & IN_MOVED_FROM)
    printf("IN_MOVED_FROM ");
  if (event->mask & IN_MOVED_TO)
    printf("IN_MOVED_TO ");
  if (event->mask & IN_OPEN)
    printf("IN_OPEN ");
  if (event->mask & IN_ALL_EVENTS)
    printf ("printf event all");
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