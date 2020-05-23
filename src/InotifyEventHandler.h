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
#include <deque>

struct customized_inotify_event
{
  int wd;		/* Watch descriptor.  */
  uint32_t mask;	/* Watch mask.  */
  uint32_t cookie;	/* Cookie to synchronize two events.  */
  uint32_t len;		/* Length (including NULs) of name.  */
  std::string name;	/* Name.  */
};


class InotifyEventHandler {
 public :
  InotifyEventHandler(RealtimeConfig& config);
  ~InotifyEventHandler();

  bool Initialize();
  void Finalize();

  void EpollHandler();
  void DirectoryTree();

  static int FtwCallback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
  static bool ParseRawEvent(std::deque<struct customized_inotify_event>& list);

private :
  int inotify_fd_;
  int epoll_fd_;

  RealtimeConfig& config_;
  static std::atomic<bool> stopped_;
};

#endif //MODULE_NAME_SRC_INOTIMGR_H_
