cmake_minimum_required(VERSION 3.0)

project(http_server)

add_executable("${PROJECT_NAME}" source.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

target_link_libraries("${PROJECT_NAME}" PRIVATE Threads::Threads)

add_subdirectory(http_lib)
add_subdirectory(scheduler_lib)

target_link_libraries("${PROJECT_NAME}" PUBLIC http_lib scheduler_lib)

target_include_directories("${PROJECT_NAME}" PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/http_lib"
        "${PROJECT_SOURCE_DIR}/scheduler_lib"
        )
