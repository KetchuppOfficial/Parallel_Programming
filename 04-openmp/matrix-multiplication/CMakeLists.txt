cmake_minimum_required(VERSION 3.20)

project(matrix_multiplication
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

find_package(OpenMP REQUIRED)
find_package(Boost REQUIRED
             COMPONENTS PROGRAM_OPTIONS)

set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_executable(sequential ${SOURCE_DIR}/matrix.cpp)
target_link_libraries(sequential
                      PRIVATE Boost::program_options)
target_include_directories(sequential PRIVATE ${INCLUDE_DIR})

add_executable(parallel ${SOURCE_DIR}/matrix.cpp)
target_link_libraries(parallel
                      PRIVATE Boost::program_options
                      PRIVATE OpenMP::OpenMP_CXX)
target_include_directories(parallel PRIVATE ${INCLUDE_DIR})
