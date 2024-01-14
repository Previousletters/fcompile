cmake_minimum_required(VERSION 3.10)
project(rt_gen C CXX)

include_directories("include")
file(GLOB RUNTIME_SRCS src/*.cc src/driver/*.cc)
file(GLOB TEST_SRCS tb/main.cc)

add_executable(main ${TEST_SRCS} ${RUNTIME_SRCS})
add_library(svc SHARED ${RUNTIME_SRCS})
