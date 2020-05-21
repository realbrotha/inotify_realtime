//
// Created by realbro on 4/16/20.
//

#ifndef MODULE_NAME_SRC_INOTIMGR_H_
#define MODULE_NAME_SRC_INOTIMGR_H_

#include "RealtimeConfig.h"

#include <sys/inotify.h>
#include <atomic>
#include <string>

class InotifyEventHandler {
 public :
  InotifyEventHandler(RealtimeConfig& config);
  ~InotifyEventHandler();

  bool Initialize();
private :
  std::atomic<bool> stopped_;
};

#endif //MODULE_NAME_SRC_INOTIMGR_H_
