//
// Created by realbro on 5/20/20.
//

#ifndef TEST_REALTIMECORE_H
#define TEST_REALTIMECORE_H

#include "RealtimeConfig.hpp"
#include "InotifyEventHandler.h"

#include <mutex>
#include <thread>

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
  void StopInotifyThread();
  bool StartDirectorySearchThread();
  void StopDirectorySearchThread();

  std::mutex mu_;

  std::thread directory_tread_;
  std::thread event_handler_;
  RealtimeConfig config_;
  InotifyEventHandler inotify_event_handler_;

};

#endif //TEST_REALTIMECORE_H
