cmake_minimum_required(VERSION 3.5)
set(MODULE_NAME testtest)

project(MODULE_NAME)
set(CMAKE_CXX_STANDARD 14)

set(BIN_PATH ${CMAKE_CURRENT_BINARY_DIR}/output_bin)
set(SOURCES
        main.cpp
        InotiMgr.cpp
        EpollWrapper.cpp
        FileDescriptorTool.cpp
        StringTool.cpp)

add_executable(${MODULE_NAME} ${SOURCES})

find_package(Threads REQUIRED)

target_link_libraries(${MODULE_NAME}
        ${CMAKE_THREAD_LIBS_INIT})

set_property(TARGET ${MODULE_NAME} PROPERTY OUTPUT_NAME ${MODULE_NAME})

install(
        TARGETS ${MODULE_NAME}
        RUNTIME DESTINATION ${BIN_PATH})