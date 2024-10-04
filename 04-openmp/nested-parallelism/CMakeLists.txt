cmake_minimum_required(VERSION 3.20)

project(omp_nested
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

find_package(OpenMP REQUIRED)

set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

add_executable(nested ${SOURCE_DIR}/nested.cpp)
target_link_libraries(nested PRIVATE OpenMP::OpenMP_CXX)
