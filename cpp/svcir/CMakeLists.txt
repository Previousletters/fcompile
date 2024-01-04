cmake_minimum_required(VERSION 3.10)
project(rt_gen C CXX)

include_directories("include")
file(GLOB RUNTIME_SRCS src/*.cc)
file(GLOB TEST_SRCS test/main.cc)

add_executable(main ${TEST_SRCS} ${RUNTIME_SRCS})
add_library(svc SHARED ${RUNTIME_SRCS})
