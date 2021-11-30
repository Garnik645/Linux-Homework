cmake_minimum_required(VERSION 3.16.3)

project(shell)

add_executable(shell shell.cpp)

add_subdirectory(shell_lib)

target_link_libraries(shell PUBLIC shell_lib)

target_include_directories(shell PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/shell_lib"
                          )