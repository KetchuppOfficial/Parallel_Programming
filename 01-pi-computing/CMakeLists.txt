cmake_minimum_required(VERSION 3.15)

project(pi_computing
        LANGUAGES CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

find_package(Boost REQUIRED
             COMPONENTS MPI PROGRAM_OPTIONS HEADERS)

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_executable(sequential
               ${SRC_DIR}/sequential.cpp
               ${SRC_DIR}/pi_computation.cpp
               ${SRC_DIR}/program_options.cpp)

target_link_libraries(sequential
                      Boost::program_options
                      Boost::headers)

target_include_directories(sequential
                           PRIVATE ${INCLUDE_DIR})

add_executable(parallel
               ${SRC_DIR}/parallel.cpp
               ${SRC_DIR}/pi_computation.cpp
               ${SRC_DIR}/program_options.cpp)

target_link_libraries(parallel
                      Boost::mpi
                      Boost::headers
                      Boost::program_options)

target_include_directories(parallel
                           PRIVATE ${INCLUDE_DIR})
