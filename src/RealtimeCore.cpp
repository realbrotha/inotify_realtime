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
  if (!StartDirectorySearchThread()) {
    printf("Directory Thread failed\n.");
    return false;
  }
  if (!StartEpollThread()) {
    printf("Epoll Thread failed\n.");
    return false;
  }
  return true;
}

bool RealtimeCore::Finalize() {
  std::lock_guard<std::mutex> guard(mu_);
  return FinalizeLock();
}

bool RealtimeCore::FinalizeLock() {
  inotify_event_handler_.Finalize();

  StopDirectorySearchThread();
  StopEpollThread();

  return true; // TODO : 명확한 종료 플래그 , 개인적으로 파이널에 bool 타입 선호하지 아니함.
}

bool RealtimeCore::StartEpollThread() {
// 구현중
  printf("StartEpoll Thread\n");
  if (epoll_handler_thread_.joinable()) {
    printf("directory thread already running\n");
  }
  std::thread epoll(&InotifyEventHandler::EpollHandler, std::ref(inotify_event_handler_));
  epoll_handler_thread_ = std::move(epoll);
  return true;
}

bool RealtimeCore::IsInEpollThread() const
{
  return std::this_thread::get_id() == directory_tread_.get_id();
}

void RealtimeCore::StopEpollThread(bool force_detach) {
  if (!epoll_handler_thread_.joinable()) {
    return;
  }
  if (IsInEpollThread() || force_detach) {
    epoll_handler_thread_.detach();
    return;
  }
  try {
    epoll_handler_thread_.join();
  } catch (const std::exception &ex) {
    printf("epoll exception 1\n");
  } catch (...) {
    printf("epoll exception 2\n");
  }
  return;
}
bool RealtimeCore::StartDirectorySearchThread() {
  printf("Directory Thread\n");
  if (directory_tread_.joinable()) {
    printf("directory thread already running\n");
  }
  std::thread dir(&InotifyEventHandler::DirectoryTree, std::ref(inotify_event_handler_));
  directory_tread_ = std::move(dir);
  return true;
}
bool RealtimeCore::IsInDirectorySearchThread() const
{
  return std::this_thread::get_id() == epoll_handler_thread_.get_id();
}

void RealtimeCore::StopDirectorySearchThread(bool force_detach) {
  if (!directory_tread_.joinable()) {
    return;
  }
  if (IsInDirectorySearchThread() || force_detach) {
    directory_tread_.detach();
    return;
  }
  try {
    directory_tread_.join();
  } catch (const std::exception &ex) {
    printf("search exception 1\n");
  } catch (...) {
    printf("search exception 2\n");
  }
  return;
}
