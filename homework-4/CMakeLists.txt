cmake_minimum_required(VERSION 3.16.3)

project(parallel)

add_executable("${PROJECT_NAME}" parallel.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

target_link_libraries("${PROJECT_NAME}" PRIVATE Threads::Threads)

add_subdirectory(parallel_matrix_lib)

target_link_libraries("${PROJECT_NAME}" PUBLIC parallel_matrix_lib)

target_include_directories("${PROJECT_NAME}" PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/parallel_matrix_lib"
                          )