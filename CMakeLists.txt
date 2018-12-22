cmake_minimum_required(VERSION 2.8.12)
project(filetozmq)

set(CMAKE_CXX_STANDARD 98)

set(CMAKE_MODULE_PATH ${filetozmq_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(ZeroMQ 3.2.4 REQUIRED)
find_package( Boost 1.41.0
        REQUIRED
        COMPONENTS program_options system filesystem unit_test_framework date_time thread regex)

include_directories(${ZEROMQ_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

add_executable(filetozmq main.cpp zmq.hpp inotifymock.h inotifymock.cpp)
target_link_libraries(filetozmq ${ZEROMQ_LIBRARIES} ${Boost_LIBRARIES})
