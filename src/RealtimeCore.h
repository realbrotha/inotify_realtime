//
// Created by realbro on 5/20/20.
//

#ifndef TEST_REALTIMECORE_H
#define TEST_REALTIMECORE_H

#include "RealtimeConfig.h"
#include "InotifyEventHandler.h"

#include <mutex>
class RealtimeCore {
 public:
  RealtimeCore(RealtimeConfig& config);
  ~RealtimeCore();

  bool Initialize();
 private:
  void Finalize();
  bool InitializeLock();

  //for threads control
  bool StartInotifyThread();
  bool StopInotifyThread();
  bool StartDirectorySearchThread();
  bool StopDirectorySearchThread();

  std::mutex mu_;
  RealtimeConfig config_;
  InotifyEventHandler inotify_event_handler_;

};

#endif //TEST_REALTIMECORE_H
