//
// Created by realbro on 20. 4. 20..
//

#include "DirectoryMgr.h"

#include "boost/filesystem.hpp"

#include <stdio.h>

DirectoryMgr::DirectoryMgr(InotiMgr inoti_mgr){
  printf("Directgory Mgrr\n");
  inoti_mgr_ = &inoti_mgr; // TODO : 싱글톤에 대한 고민.
}
DirectoryMgr::~DirectoryMgr(){
  Finalize();
}

bool DirectoryMgr::Init(){
  //TODO : need thread
  boost::filesystem::recursive_directory_iterator end;
  boost::filesystem::path target("/");

  boost::system::error_code ec;
  int dir_count = 0;

  for (boost::filesystem::recursive_directory_iterator it(target, boost::filesystem::directory_options::skip_permission_denied, ec); it != end; ++it) {
    if (boost::filesystem::is_directory(*it, ec)) {
      dir_count++;
    }
    // do something
  }
  printf("count : dir_count : %d", dir_count);
}
bool DirectoryMgr::Finalize(){

}
