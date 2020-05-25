cmake_minimum_required(VERSION 3.5)
set(MODULE_NAME testtest)

project(MODULE_NAME)
set(CMAKE_CXX_STANDARD 14)

set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include)
set(LIB_PATH ${CMAKE_SOURCE_DIR}/lib)
set(BIN_PATH ${CMAKE_CURRENT_BINARY_DIR}/output_bin)

set(SOURCES
        main.cpp
        InotifyEventHandler.cpp
        common/EpollWrapper.cpp
        common/FileDescriptorTool.cpp
        common/StringTool.cpp
        RealtimeInterface.cpp
        RealtimeInterface.h
        RealtimeCore.cpp
        RealtimeCore.h
        RealtimeConfig.hpp
        InotifyWatchMgr.cpp
        InotifyWatchMgr.h common/WaitSignalKill.hpp InotifyMask.cpp InotifyMask.h)

include_directories(${INCLUDE_PATH})

add_executable(${MODULE_NAME} ${SOURCES})

find_package(Threads REQUIRED)

target_link_libraries(${MODULE_NAME}
        ${CMAKE_THREAD_LIBS_INIT})
#        ${LIB_PATH}/boost/libboost_filesystem.a
#        ${LIB_PATH}/boost/libboost_system.a)

set_property(TARGET ${MODULE_NAME} PROPERTY OUTPUT_NAME ${MODULE_NAME})

install(
        TARGETS ${MODULE_NAME}
        RUNTIME DESTINATION ${BIN_PATH})