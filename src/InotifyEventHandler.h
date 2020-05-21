//
// Created by realbro on 4/16/20.
//

#ifndef MODULE_NAME_SRC_INOTIMGR_H_
#define MODULE_NAME_SRC_INOTIMGR_H_

#include "RealtimeConfig.hpp"

#include <ftw.h>
#include <sys/inotify.h>

#include <atomic>
#include <string>

class InotifyEventHandler {
 public :
  InotifyEventHandler(RealtimeConfig& config);
  ~InotifyEventHandler();

  bool Initialize();
  void EpollHandler();
  void DirectoryTree();

  static int FtwCallback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

private :
  int inotify_fd_;
  int epoll_fd_;

  RealtimeConfig& config_;
  std::atomic<bool> stopped_;
};

#endif //MODULE_NAME_SRC_INOTIMGR_H_
