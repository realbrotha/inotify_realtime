//
// Created by realbro on 20. 5. 25..
//

#include "InotifyMask.h"

#include <sys/inotify.h>

std::string InotifyMaskToString(uint64_t mask) {
  std::string result;
  typedef std::pair<uint64_t, const char *> item_t;
  static item_t masks[] = {
      std::make_pair(IN_ACCESS, "IN_ACCESS"),
      std::make_pair(IN_MODIFY, "IN_MODIFY"),
      std::make_pair(IN_ATTRIB, "IN_ATTRIB"),
      std::make_pair(IN_CLOSE_WRITE, "IN_CLOSE_WRITE"),
      std::make_pair(IN_CLOSE_NOWRITE, "IN_CLOSE_NOWRITE"),
      //std::make_pair(IN_CLOSE, "IN_CLOSE"),
      std::make_pair(IN_OPEN, "IN_OPEN"),
      std::make_pair(IN_MOVED_FROM, "IN_MOVED_FROM"),
      std::make_pair(IN_MOVED_TO, "IN_MOVED_TO"),
      //std::make_pair(IN_MOVE, "IN_MOVE"),
      std::make_pair(IN_CREATE, "IN_CREATE"),
      std::make_pair(IN_DELETE, "IN_DELETE"),
      std::make_pair(IN_DELETE_SELF, "IN_DELETE_SELF"),
      std::make_pair(IN_MOVE_SELF, "IN_MOVE_SELF"),
      std::make_pair(IN_ISDIR, "IN_ISDIR")
  };

  for (const item_t &item : masks) {
    if ((mask & item.first) != 0) {
      if (!result.empty()) {
        result += ",";
      }
      result.append(item.second);
      mask &= ~item.first;
    }
  }

  return result;
}