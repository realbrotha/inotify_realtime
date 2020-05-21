//
// Created by realbro on 4/16/20.
//
#include "InotifyEventHandler.h"

#include "common/EpollWrapper.h"

#include <unistd.h>

#include <iostream>
#include <thread>

namespace {
constexpr int kEPOLL_MAX_EVENT_COUNT = 3;
}

InotifyEventHandler::InotifyEventHandler(RealtimeConfig& config) {

}
InotifyEventHandler::~InotifyEventHandler() {

}
bool InotifyEventHandler::Initialize() {

}