cmake_minimum_required(VERSION 3.15)

project(pi_computing
        LANGUAGES CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(${PROJECT_SOURCE_DIR}/../matplotplusplus
                 ${PROJECT_SOURCE_DIR}/../matplotplusplus/build)

add_executable(sequential
               ${SRC_DIR}/sequential.cpp)

target_link_libraries(sequential
                      PUBLIC matplot m)

target_include_directories(sequential
                           PRIVATE ${INCLUDE_DIR})
