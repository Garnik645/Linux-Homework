cmake_minimum_required(VERSION 3.21)
project(shared_memory)

set(CMAKE_CXX_STANDARD 17)

add_executable(producer producer.cpp)
add_executable(consumer consumer.cpp)

add_subdirectory(buffer_lib)

target_link_libraries(producer PUBLIC buffer_lib)

target_include_directories(producer PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/buffer_lib"
        )

target_link_libraries(consumer PUBLIC buffer_lib)

target_include_directories(consumer PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/buffer_lib"
        )