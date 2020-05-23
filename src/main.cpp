//
// Created by realbro on 4/13/20.
//

#include <stdio.h>
#include <unistd.h>

#include "RealtimeInterface.h"
#include "common/WaitSignalKill.hpp"

int main(int argc, char *argv[]) {
  printf ("Test Inotify Event\n");
  RealtimeInterface realtime_interface;
  realtime_interface.Initialize();

  WaitSignalKill();
}
