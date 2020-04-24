//
// Created by realbro on 20. 4. 20..
//

#include "DirectoryMgr.h"

#include "boost/filesystem.hpp"

#include <stdio.h>

std::vector<std::string> DirectoryMgr::sample_list;

DirectoryMgr::DirectoryMgr() {
}
DirectoryMgr::~DirectoryMgr() {
  Finalize();
}

bool DirectoryMgr::Init() {
  TestFunc();
  search_thread = std::thread(&DirectoryMgr::SearchDirectory, this);
}
bool DirectoryMgr::Finalize() {
  search_thread.join();

  printf("Directory MGR Finale");
}
bool DirectoryMgr::TestFunc() {
  sample_list.push_back("/proc");
  sample_list.push_back("/usr/src");
  sample_list.push_back("/usr/share");
  sample_list.push_back("/usr/lib");
}
bool DirectoryMgr::IsSkipPath(std::string &target_path) {
  // TODO : 이런식으로 찾으면 비용이 클듯.
  bool result = false;

  for (int i = 0; i < sample_list.size(); ++i) {
    if (std::string::npos != target_path.find(sample_list[i])) {
      result = true;
    }
  }
  return result;
}

int DirectoryMgr::FtwCallback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
  char real_p[1024] = {0,};

  if (FTW_D != typeflag ||
      NULL == realpath(fpath, real_p)) {
    return 0;
  }
  std::string target_path(real_p);
  if (IsSkipPath(target_path))
    return 0;

  if (!InotifyEventHandler::GetInstance().AddWatch(target_path)) {
    printf("stop!!!"); // inotify event queue full 일듯 ...
    return -1;
  }
  return 0;
}
void DirectoryMgr::SearchDirectory() {
  std::string target_path = "/home/realbro/test";
  if (0 > nftw(target_path.c_str(), FtwCallback, 1, FTW_PHYS)) {
    printf("error");
  }
  printf("SearchDirecoty done.\n");
  return;
}