//
// Created by realbro on 5/20/20.
//

#ifndef TEST_SRC_REALTIMECONFIG_HPP_
#define TEST_SRC_REALTIMECONFIG_HPP_

#include <string>
#include <vector>
class RealtimeConfig {
 public :
  RealtimeConfig() { // for test 테스트 후 삭제할것 .
    std::string buffer = "/home/realbro/test";
    detect_path_list_.push_back(buffer);

    std::string exception_buffer = "/home/realbro/skipthis";
    exception_path_list_.push_back(exception_buffer);
  }
  std::vector<std::string> GetDetectList() {
    return detect_path_list_;
  }
  std::vector<std::string> GetExceptionList() {
    return exception_path_list_;
  }

  bool LoadConfig() { return true;/*not yet rdy */}

  inline bool operator==(const RealtimeConfig &other) const {
    return running_type == other.running_type;
  }
  inline bool operator!=(const RealtimeConfig &other) const {
    return running_type != other.running_type;
  }
 private :
  // 기타등등 옵션
  bool running_type;
  std::vector<std::string> detect_path_list_;
  std::vector<std::string> exception_path_list_;
};

#endif //TEST_SRC_REALTIMECONFIG_HPP_
