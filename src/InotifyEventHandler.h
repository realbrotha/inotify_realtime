//
// Created by realbro on 4/16/20.
//

#ifndef MODULE_NAME_SRC_INOTIMGR_H_
#define MODULE_NAME_SRC_INOTIMGR_H_

#include "FdMgr.h"
#include "DirectoryMgr.h"

#include <sys/inotify.h>
#include <atomic>
#include <string>

typedef void(*EventCallbackFunc)(struct inotify_event *event);

class InotifyEventHandler {
 public :
  static InotifyEventHandler &GetInstance();
  bool Init();

 private:
  InotifyEventHandler();
  ~InotifyEventHandler();

  bool AddWatch(std::string &target_path);
  bool RmWatch(int watch_fd);

  static void EventCallback(const struct inotify_event *event);
  static void *EpollHandler(void *arg);

  friend class DirectoryMgr;
  std::atomic<bool> stopped_;
};

#endif //MODULE_NAME_SRC_INOTIMGR_H_
