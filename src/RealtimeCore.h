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
  bool Finalize();

 private:
  bool InitializeLock();
  bool FinalizeLock();
  //for threads control

  bool StartEpollThread();
  bool IsInEpollThread() const;
  void StopEpollThread(bool force_detach = false);

  bool StartDirectorySearchThread();
  bool IsInDirectorySearchThread() const;
  void StopDirectorySearchThread(bool force_detach = false);

  std::mutex mu_;

  std::thread directory_tread_;
  std::thread epoll_handler_thread_;
  RealtimeConfig config_;
  InotifyEventHandler inotify_event_handler_;

};

#endif //TEST_REALTIMECORE_H
