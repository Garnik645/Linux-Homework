cmake_minimum_required(VERSION 3.21)
project(http_server)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(http_server
        include/http.cpp
        include/http.h
        source.cpp)
