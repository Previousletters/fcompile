cmake_minimum_required(VERSION 3.10)
project(rt_gen C CXX)

include_directories("include")
file(GLOB RUNTIME_SRCS src/*.cpp)

add_executable (main ${RUNTIME_SRCS})
