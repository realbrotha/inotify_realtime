//
// Created by realbro on 4/13/20.
//

#include <stdio.h>
#include <unistd.h>

#include "InotifyEventHandler.h"

int main(int argc, char *argv[]) {
  InotifyEventHandler::GetInstance().Init();
  int count =1000;
  while (count) {
    printf ("1111");
    sleep(1);
    --count;
  }
}
