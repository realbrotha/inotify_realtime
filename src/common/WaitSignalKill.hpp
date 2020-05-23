//
// Created by realbro on 5/20/20.
//

#ifndef TEST_SRC_COMMON_WAITSIGNALKILL_HPP_
#define TEST_SRC_COMMON_WAITSIGNALKILL_HPP_

#include <signal.h>
#include <string.h>
#include <errno.h>

bool WaitSignalKill() {
  sigset_t sigMaskWait;
  sigemptyset(&sigMaskWait);
  sigaddset(&sigMaskWait, SIGINT);
  sigaddset(&sigMaskWait, SIGTERM);
  sigaddset(&sigMaskWait, SIGQUIT);

  while (true) {
    siginfo_t sigInfo;
    memset(&sigInfo, 0, sizeof(siginfo_t));
    timespec waitTime = {tv_sec: 1, tv_nsec: 0};

    if (sigtimedwait(&sigMaskWait, &sigInfo, &waitTime) < 0) {
      if ((errno != EINTR) && (errno != EAGAIN)) {
        return -1;
      }
      errno = 0;
    }
  }
  return 0;
}

#endif //TEST_SRC_COMMON_WAITSIGNALKILL_HPP_
