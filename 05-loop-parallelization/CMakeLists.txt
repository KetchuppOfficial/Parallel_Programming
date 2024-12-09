cmake_minimum_required(VERSION 3.21)

project(lab-1
        LANGUAGES CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

find_package(fmt REQUIRED)
find_package(CLI11 REQUIRED)

add_executable(sequential
    ./src/sequential.cpp
)

set_target_properties(sequential PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(sequential PRIVATE
    fmt::fmt
    CLI11::CLI11
)
