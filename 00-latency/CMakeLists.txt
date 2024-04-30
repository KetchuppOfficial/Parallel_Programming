cmake_minimum_required(VERSION 3.15)

project(latency
        LANGUAGES CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

find_package(Boost REQUIRED
             COMPONENTS MPI PROGRAM_OPTIONS)

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

add_executable(measure_latency
               ${SRC_DIR}/measure_latency.cpp)

target_link_libraries(measure_latency
                      PRIVATE Boost::mpi Boost::program_options)
 