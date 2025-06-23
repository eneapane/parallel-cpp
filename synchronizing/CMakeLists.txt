cmake_minimum_required(VERSION 3.31)
project(synchronizing)

set(CMAKE_CXX_STANDARD 20)

add_executable(synchronizing main.cpp
        custom_mutex.cpp
        custom_mutex.h
        custom_synchronization.cpp
        custom_synchronization.h
        futures_synchronization.cpp
        futures_synchronization.h)
