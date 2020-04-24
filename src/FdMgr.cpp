//
// Created by realbro on 20. 4. 24..
//

#include "FdMgr.h"

#include <unistd.h>
#include <sys/inotify.h>

FdMgr &FdMgr::GetInstance() {
  static FdMgr instance_;
  return instance_;
}

FdMgr::FdMgr() : inoti_fd(-1), epoll_fd(-1) {
}

FdMgr::~FdMgr() {
  printf("Fnail FDMgr");
  Finalize();
}
void FdMgr::Init(int i_fd, int e_fd) {
  inoti_fd = i_fd;
  epoll_fd = e_fd;
}
void FdMgr::Finalize() {
  printf ("final");
  std::lock_guard<std::mutex> guard(mtx);
  {
    for (auto &kv : fd_list) {
      inotify_rm_watch(inoti_fd, kv.first);
    }
    fd_list.clear();
  }
  if (inoti_fd) {
    close(inoti_fd);
  }
  if (epoll_fd) {
    close(epoll_fd);
  }
}
void FdMgr::AddFd(int watch_fd, std::string &dir_path) {
  std::lock_guard<std::mutex> guard(mtx);
  {
    fd_list[watch_fd] = dir_path;
    //fd_list_reversal[dir_path] = watch_fd;
  }
}
void FdMgr::RemoveFd(int watch_fd) {
  std::lock_guard<std::mutex> guard(mtx);
  {
    if (0 < fd_list.count(watch_fd)) {
      printf("remove fd : %d\n", watch_fd);
      printf("path : %s\n", fd_list[watch_fd].c_str());
      std::string buffer = fd_list[watch_fd];
      //fd_list_reversal.erase(buffer);
      fd_list.erase(watch_fd);
    }
  }
}

std::string FdMgr::GetPath(int watch_fd) {
  return (0 < fd_list.count(watch_fd)) ? fd_list[watch_fd] : "";
}

std::map<int, std::string /* dir path */> FdMgr::GetAll() {
  return fd_list;
}
int FdMgr::GetInotiFd() {
  return inoti_fd;
}
int FdMgr::GetEpollFd() {
  return epoll_fd;
}
/*
int FdMgr::GetWd(std::string dir_path){
  if (0 < fd_list_reversal.count(dir_path)) {
    int buffer = fd_list_reversal[dir_path];
    printf ("get Wd [%d]\n",buffer);
    return buffer;
  }
  return 0;
}
 */