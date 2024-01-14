cmake_minimum_required(VERSION 3.10)
project(rt_gen C CXX)
SET(CMAKE_BUILD_TYPE "Debug")
include_directories("include")
file(GLOB RUNTIME_SRCS src/*.cpp)

add_executable (main ${RUNTIME_SRCS})
