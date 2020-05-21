//
// Created by realbro on 4/16/20.
//
#include "InotifyEventHandler.h"
#include "InotifyWatchMgr.h"
#include "common/EpollWrapper.h"
#include "common/FileDescriptorTool.h"
#include "boost/scope_exit.hpp"

#include <unistd.h>
#include <iostream>
#include <thread>
#include <ftw.h>

namespace {
constexpr int kEPOLL_MAX_EVENT_COUNT = 3;
}

InotifyEventHandler::InotifyEventHandler(RealtimeConfig& config) : inotify_fd_ (-1), epoll_fd_ (-1), config_(config) {

}
InotifyEventHandler::~InotifyEventHandler() {

}
bool InotifyEventHandler::Initialize() {
  int inotify_fd = InotifyWatchMgr::GetInstance().Initialize();
  if (inotify_fd < 0) {
	return false;
  }
  FileDescriptorTool::SetCloseOnExec(inotify_fd, true);
  FileDescriptorTool::SetNonBlock(inotify_fd, true);

  inotify_fd_ = inotify_fd;

  BOOST_SCOPE_EXIT(this_, &inotify_fd_)
  {
    if (this_->stopped_ && inotify_fd_ != -1) {
      close(inotify_fd_);
    }
  }
  BOOST_SCOPE_EXIT_END

  int epoll_fd = epoll_create(1);
  if (epoll_fd < 0) {
	return false;
  }
  epoll_fd_ = epoll_fd;

  FileDescriptorTool::SetCloseOnExec(epoll_fd_, true);
  FileDescriptorTool::SetNonBlock(epoll_fd_, true);

  BOOST_SCOPE_EXIT(this_, &epoll_fd_) {
	if (this_->stopped_ && epoll_fd_ != -1) {
	  close(epoll_fd_);
	}
  }
  BOOST_SCOPE_EXIT_END

  stopped_ = false;
  return true;
}

int InotifyEventHandler::FtwCallback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
  char real_path[1024] = {0,};

  if (FTW_D != typeflag || !realpath(fpath, real_path)) {
    return 0;
  }
  std::string target_path(real_path);

  // todo : 예외 경로 처리

  if (!InotifyWatchMgr::GetInstance().AddWatch(target_path)) {
    printf ("AddWatch Failed. (out of buffer size)\n");
    return -1;
  }

  return 0;
}
void InotifyEventHandler::DirectoryTree() {
  printf ("directory start\n");
  for (auto &it : config_.GetDetectList()) {
    printf ("path : %s\n", it.c_str());
    if (0 > nftw(it.c_str(), FtwCallback, 1, FTW_PHYS)) {
      printf ("Directory Abort.\n");
    }
  }
}
void InotifyEventHandler::EpollHandler() {

}

