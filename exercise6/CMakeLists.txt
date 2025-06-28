cmake_minimum_required(VERSION 3.31)
project(exercise6)

set(CMAKE_CXX_STANDARD 20)

add_executable(exercise6 main.cpp
        ReadWriteMutex.cpp
        ReadWriteMutex.h)
