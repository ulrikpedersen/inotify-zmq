cmake_minimum_required(VERSION 2.8.12)
project(filetozmq)

set(CMAKE_CXX_STANDARD 98)

set(CMAKE_MODULE_PATH ${filetozmq_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(ZeroMQ 3.2.4 REQUIRED)

include_directories(${ZEROMQ_INCLUDE_DIRS})

add_executable(filetozmq main.cpp zmq.hpp)
target_link_libraries(filetozmq ${ZEROMQ_LIBRARIES})
