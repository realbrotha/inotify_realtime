//
// Created by realbro on 4/16/20.
//

#ifndef MODULE_NAME_SRC_INOTIMGR_H_
#define MODULE_NAME_SRC_INOTIMGR_H_

#include "DirectoryMgr.h"

#include <atomic>
#include <string>
class InotiMgr {
 public :
  InotiMgr();
  ~InotiMgr();

  bool Init();
 private:
  bool AddItem(std::string &target_path, int &i_fd, int &w_fd);
  bool RemoveItem();

  static void MaskCheck(const struct inotify_event *event);
  static void *EpollHandler(void *arg);
  bool Finalize();

  friend class DirectoryMgr;
  std::atomic<bool> stopped_;
  int inoti_fd;
  int watch_fd;
  int epoll_fd;
};

#endif //MODULE_NAME_SRC_INOTIMGR_H_
