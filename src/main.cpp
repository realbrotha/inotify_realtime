//
// Created by realbro on 4/13/20.
//

#include <stdio.h>
#include <unistd.h>

#include "RealtimeInterface.h"
#include "common/WaitSignalKill.hpp"

int main(int argc, char *argv[]) {
  printf (" hehehehehehe eh oeh oeh eo heo heo he\n");
  RealtimeInterface realtime_interface;
  realtime_interface.Initialize();

  WaitSignalKill();

}
