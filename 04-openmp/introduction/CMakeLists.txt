cmake_minimum_required(VERSION 3.20)

project(omp_intro
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

find_package(OpenMP REQUIRED)
find_package(Boost REQUIRED
             COMPONENTS PROGRAM_OPTIONS)

set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

add_executable(hello ${SOURCE_DIR}/hello.cpp)
target_link_libraries(hello PRIVATE OpenMP::OpenMP_CXX)

add_executable(harmonic_series ${SOURCE_DIR}/harmonic-series.cpp)
target_link_libraries(harmonic_series
                      PRIVATE OpenMP::OpenMP_CXX
                      PRIVATE Boost::program_options)

add_executable(ordered_access ${SOURCE_DIR}/ordered-access.cpp)
target_link_libraries(ordered_access PRIVATE OpenMP::OpenMP_CXX)
