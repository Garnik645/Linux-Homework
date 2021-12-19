cmake_minimum_required(VERSION 3.16.3)

project(scheduler)

add_executable("${PROJECT_NAME}" scheduler.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

target_link_libraries("${PROJECT_NAME}" PRIVATE Threads::Threads)

add_subdirectory(scheduler_lib)

target_link_libraries("${PROJECT_NAME}" PUBLIC scheduler_lib)

target_include_directories("${PROJECT_NAME}" PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/scheduler_lib"
                          )