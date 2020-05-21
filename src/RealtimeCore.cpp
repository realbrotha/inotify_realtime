//
// Created by realbro on 5/20/20.
//

#include "RealtimeCore.h"
#include "InotifyEventHandler.h"
RealtimeCore::RealtimeCore(RealtimeConfig &config) : config_(config), inotify_event_handler_(config) {

}
RealtimeCore::~RealtimeCore() {

}
bool RealtimeCore::Initialize() {
  std::lock_guard<std::mutex> guard(mu_);
  return InitializeLock();
}

bool RealtimeCore::InitializeLock() {
  if (!inotify_event_handler_.Initialize()) {
	return false;
  }
}

void RealtimeCore::Finalize() {
}

bool RealtimeCore::StartInotifyThread() {
// 구현중
}
bool RealtimeCore::StopInotifyThread() {
// 구현중
}
bool RealtimeCore::StartDirectorySearchThread() {
// 구현중
}
bool RealtimeCore::StopDirectorySearchThread() {
// 구현중
}
