//
// Created by realbro on 5/20/20.
//

#ifndef TEST_REALTIMEINTERFACE_H
#define TEST_REALTIMEINTERFACE_H

#include "RealtimeCore.h"
#include "RealtimeConfig.h"

#include <memory>

class RealtimeInterface {
 public:
  RealtimeInterface();
  ~RealtimeInterface();

  bool Initialize();
  void Finalize();

 private:
  std::shared_ptr<RealtimeCore> core_;
  RealtimeConfig config_;
};

#endif //TEST_REALTIMEINTERFACE_H
