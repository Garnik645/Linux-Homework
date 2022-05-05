cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_CLANG_TIDY
        clang-tidy;
        -format-style=llvm;)

project(http_server)

add_executable("${PROJECT_NAME}" source.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

target_link_libraries("${PROJECT_NAME}" PRIVATE Threads::Threads)

add_subdirectory(http_lib)

target_link_libraries("${PROJECT_NAME}" PUBLIC http_lib)

target_include_directories("${PROJECT_NAME}" PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/http_lib"
        )
