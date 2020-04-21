//
// Created by realbro on 20. 4. 20..
//

#ifndef TEST_SRC_DIRECTORYMGR_H_
#define TEST_SRC_DIRECTORYMGR_H_

#include "InotiMgr.h"

#include <ftw.h>
#include <string>
#include <thread>
#include <vector>

class DirectoryMgr {
 public:
  DirectoryMgr();
  ~DirectoryMgr();

  bool Init();
 private:
  bool Finalize();
  bool TestFunc();
  void SearchDirectory();
  static bool IsSkipPath(std::string& target_path);
  static int FtwCallback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

  std::thread search_thread;
  static std::vector<std::string> sample_list;
};

#endif //TEST_SRC_DIRECTORYMGR_H_
