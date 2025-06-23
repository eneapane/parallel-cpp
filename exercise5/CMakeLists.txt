cmake_minimum_required(VERSION 3.31)
project(exercise5)

set(CMAKE_CXX_STANDARD 20)

add_executable(exercise5 main.cpp
        ThreadSafeArray.cpp
        ThreadSafeArray.h)
