//
// Created by realbro on 5/20/20.
//

#ifndef TEST_SRC_REALTIMECONFIG_H_
#define TEST_SRC_REALTIMECONFIG_H_
class RealtimeConfig {
 public :
  bool LoadConfig() { return true;/*not yet rdy */}
  // 기타등등 옵션
  bool running_type;
  inline bool operator==(const RealtimeConfig& other) const
  {
	return running_type == other.running_type;
  }
  inline bool operator!=(const RealtimeConfig& other) const
  {
	return running_type != other.running_type;
  }
};

#endif //TEST_SRC_REALTIMECONFIG_H_
