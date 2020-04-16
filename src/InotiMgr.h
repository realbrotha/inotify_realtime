//
// Created by realbro on 4/16/20.
//

#ifndef MODULE_NAME_SRC_INOTIMGR_H_
#define MODULE_NAME_SRC_INOTIMGR_H_

#include <atomic>

class InotiMgr {
 public :
  InotiMgr();
  ~InotiMgr();

  bool Init();
 private:
  static void MaskCheck(const struct inotify_event *event);
  static void *EpollHandler(void *arg);
  bool Finalize();

  std::atomic<bool> stopped_;
  int inoti_fd;
  int watch_fd;
  int epoll_fd;
};

#endif //MODULE_NAME_SRC_INOTIMGR_H_
