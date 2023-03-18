cmake_minimum_required(VERSION 3.22.3)
project(MyLSM_Tree)

set(CMAKE_CXX_STANDARD 11)

include_directories(src/)
include_directories(src/hash/)
set(CMAKE_EXE_LINKER_FLAGS "-static")
add_executable(MyLSM_Tree src/main.cpp)

add_executable(test_BF src/test_bloomfilter.cpp src/bloomfilter.h src/hash/murmurhash.h)
