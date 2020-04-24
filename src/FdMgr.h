//
// Created by realbro on 20. 4. 24..
//

#ifndef TEST_SRC_FDMGR_H_
#define TEST_SRC_FDMGR_H_

#include <map>
#include <mutex>

class FdMgr {
 public:
  static FdMgr& GetInstance();

  void Init(int inoti_fd, int epoll_fd);
  void AddFd(int watch_fd, std::string &dir_path);
  void RemoveFd(int watch_fd);

  int GetInotiFd();
  int GetEpollFd();
  //int GetWd(std::string dir_path);
  std::string GetPath(int watch_fd);
  std::map<int /* watch_fd */, std::string /* dir path */> GetAll();

 private:
  FdMgr();
  ~FdMgr();

  void Finalize();
  mutable std::mutex mtx;

  int inoti_fd;
  int epoll_fd;
  std::map<int /* watch_fd */, std::string /* dir path */> fd_list;
  //std::map<std::string /* dir path */, int /* watch_fd */> fd_list_reversal;
};

#endif //TEST_SRC_FDMGR_H_


