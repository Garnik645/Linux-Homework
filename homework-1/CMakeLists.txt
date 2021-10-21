cmake_minimum_required(VERSION 3.16.3)

project(ComplexRealization)

add_executable(ComplexRealization main.cpp)

add_subdirectory(MathFunctionLib)

target_link_libraries(ComplexRealization PUBLIC MathFunctionLib)

target_include_directories(ComplexRealization PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/MathFunctionLib"
                          )
