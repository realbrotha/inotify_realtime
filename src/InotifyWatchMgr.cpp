//
// Created by realbro on 5/20/20.
//

#include "InotifyWatchMgr.h"
#include "common/FileDescriptorTool.h"

#include <sys/inotify.h>
namespace {
const int kINOTIFY_MASK_FOR_TEST = IN_ACCESS | IN_MODIFY | IN_ATTRIB | IN_CLOSE| IN_OPEN| IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE | IN_CLOSE_WRITE | IN_DELETE_SELF | IN_MOVE_SELF;
}
InotifyWatchMgr::InotifyWatchMgr() : inotify_fd_(-1) {

}
InotifyWatchMgr::~InotifyWatchMgr() {
  Finalize();
}

InotifyWatchMgr &InotifyWatchMgr::GetInstance() {
  static InotifyWatchMgr instance_;
  return instance_;
}

void InotifyWatchMgr::Finalize() {

}

bool InotifyWatchMgr::Initialize(int inotify_fd) {
  inotify_fd_ = inotify_fd;
  return !!inotify_fd_;
}

bool InotifyWatchMgr::AddWatch(std::string target_path) {
  bool result = false;
  int watch_fd = inotify_add_watch(inotify_fd_,
                                   target_path.c_str(),
                                   kINOTIFY_MASK_FOR_TEST);

  if (watch_fd < 0) {
    printf("add failed.\n");
  } else {
    printf("wtach_fd [%d] inotify_fd [%d] path : [%s] success\n", watch_fd, inotify_fd_, target_path.c_str());
    AddList(watch_fd, target_path);
    result = true;
  }
  return result;
}
bool InotifyWatchMgr::AddList(int watch_fd, std::string target_path) {
  std::lock_guard<std::mutex> guard(mtx_);
  {
    fd_path_mapping_list[watch_fd] = target_path;
  }
}
bool InotifyWatchMgr::RemoveList(int watch_fd) {
  std::lock_guard<std::mutex> guard(mtx_);
  {
    if (0 < fd_path_mapping_list.count(watch_fd)) {
      fd_path_mapping_list.erase(watch_fd);
    }
  }
}

std::string InotifyWatchMgr::GetPath(int watch_fd) {
  return (0 < fd_path_mapping_list.count(watch_fd)) ? fd_path_mapping_list[watch_fd] : "";
}