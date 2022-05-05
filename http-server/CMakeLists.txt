cmake_minimum_required(VERSION 3.0)
project(http_server)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(http_server
        include/http.cpp
        include/http.h
        include/error_handler.h
        source.cpp)
