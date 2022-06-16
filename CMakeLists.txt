cmake_minimum_required(VERSION 3.16)
project(code)
#set(CMAKE_CXX_FLAGS "-std=c++17 -O3")
#set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS " -O2")
set(CMAKE_CXX_FLAGS " -Ofast")

add_executable(code main_new.cpp)
