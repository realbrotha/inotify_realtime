//
// Created by realbro on 5/20/20.
//

#include "RealtimeCore.h"
#include "InotifyEventHandler.h"

RealtimeCore::RealtimeCore(RealtimeConfig &config) : config_(config), inotify_event_handler_(config) {

}
RealtimeCore::~RealtimeCore() {
  Finalize();
}
bool RealtimeCore::Initialize() {
  printf("core init ");
  std::lock_guard<std::mutex> guard(mu_);
  return InitializeLock();
}

bool RealtimeCore::InitializeLock() {
  if (!inotify_event_handler_.Initialize()) {
    return false;
  }
  StartDirectorySearchThread();
}

void RealtimeCore::Finalize() {
  StopDirectorySearchThread();
}

bool RealtimeCore::StartInotifyThread() {
// 구현중
}
void RealtimeCore::StopInotifyThread() {
// 구현중
}
bool RealtimeCore::StartDirectorySearchThread() {
  printf("Directory Thread\n");
  if (directory_tread_.joinable()) {
    printf("directory thread already running\n");
  }
  std::thread dir(&InotifyEventHandler::DirectoryTree, std::ref(inotify_event_handler_));
  directory_tread_ = std::move(dir);
}

void RealtimeCore::StopDirectorySearchThread() {
  if (!directory_tread_.joinable()) {
    return;
  }
  bool shutdown_force = false; // todo : 다른곳에서 요청을 할것 일단 형태만 잡아 놓는다.
  if (!shutdown_force) {
    directory_tread_.detach();
    return;
  }
  try {
    directory_tread_.join();
  } catch (const std::exception &ex) {
    printf("exception \n");
  } catch (...) {
    printf("exception \n");
  }
}
