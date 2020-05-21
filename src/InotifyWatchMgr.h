//
// Created by realbro on 5/20/20.
//

#ifndef TEST_SRC_INOTIFYWATCHMGR_H_
#define TEST_SRC_INOTIFYWATCHMGR_H_

#include <string>
#include <map>
#include <mutex>
#include <sys/inotify.h>

class InotifyWatchMgr {
 public :
  static InotifyWatchMgr &GetInstance();

  bool Initialize();

  bool AddWatch(std::string target_path);
  bool RemoveWatch(const int watch_fd);

 private :
  InotifyWatchMgr();
  ~InotifyWatchMgr();

  void Finalize();

  bool AddList(int watch_fd, std::string target_path);
  bool RemoveList(int watch_fd);

  std::mutex mtx_;
  int inotify_fd_;
  std::map<int /* watch_fd */, std::string /* dir path */> fd_path_mapping_list;
};

#endif //TEST_SRC_INOTIFYWATCHMGR_H_
