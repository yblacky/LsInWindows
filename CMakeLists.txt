cmake_minimum_required(VERSION 3.10)
project(LsInWindows)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(LsInWindows main.cpp include/ls.c)
