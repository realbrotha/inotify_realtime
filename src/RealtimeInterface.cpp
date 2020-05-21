//
// Created by realbro on 5/20/20.
//

#include "RealtimeInterface.h"
#include "boost/scope_exit.hpp"

RealtimeInterface::RealtimeInterface() {

}
RealtimeInterface::~RealtimeInterface() {
  Finalize();
}

bool RealtimeInterface::Initialize() {
  // Todo : inotify 존재하는 kernel 버전 체크 2.16.32 부터는 스테이블 하게 사용가능 한듯하고 4.x 에서 수정이 좀 있었던거 같다 .
  // 여기서 체크를 하하거나 core 클래스에서 하던지 둘중 한쪽에서 한다.
  bool result = false;
  if (!config_.LoadConfig()) { //todo : observer 붙여야됨, 향후 정책 관리하기위해 config를 인터페이스가 보유함.
	// todo : exception 처리
  }
  if (core_.get()) {
	result = true; // already
  } else {
	try {
	  core_.reset(new RealtimeCore(config_));
	  if (core_->Initialize()) {
		result = true; // success
	  }
	}
	catch (const std::exception &ex) {
	  core_.reset();
	}
	catch (...) {
	  core_.reset();
	}
  }
  BOOST_SCOPE_EXIT(&result, &core_)
  {
	if (!result) {
	  core_.reset();
	}
  }
  BOOST_SCOPE_EXIT_END
  return result;
}

void RealtimeInterface::Finalize() {
  ;
}

