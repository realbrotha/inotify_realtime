//
// Created by realbro on 4/13/20.
//

#include <stdio.h>
#include <unistd.h>

#include "InotiMgr.h"

int main(int argc, char *argv[]) {
  printf("testtest\n");

  InotiMgr::GetInstance().Init();
  while (1) {
    printf ("sleep\n");
    sleep(1);
  }
}
