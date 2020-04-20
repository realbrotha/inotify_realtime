//
// Created by realbro on 20. 4. 20..
//

#ifndef TEST_SRC_DIRECTORYMGR_H_
#define TEST_SRC_DIRECTORYMGR_H_

#include "InotiMgr.h"

class DirectoryMgr {
 public:
  DirectoryMgr(InotiMgr inoti_mgr);
  ~DirectoryMgr();

  bool Init();
 private:
  bool Finalize();
  InotiMgr* inoti_mgr_;
};

#endif //TEST_SRC_DIRECTORYMGR_H_
