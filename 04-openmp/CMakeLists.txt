cmake_minimum_required(VERSION 3.20)

project(OMP_HELLO
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

find_package(OpenMP REQUIRED)
find_package(Boost REQUIRED
             COMPONENTS PROGRAM_OPTIONS)

add_executable(hello ./src/hello.cpp)
add_executable(harmonic_series ./src/harmonic-series.cpp)
add_executable(ordered_access ./src/ordered-access.cpp)
add_executable(nested ./src/nested.cpp)

target_link_libraries(hello PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(harmonic_series
                      PRIVATE OpenMP::OpenMP_CXX
                      PRIVATE Boost::program_options)
target_link_libraries(ordered_access PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(nested PRIVATE OpenMP::OpenMP_CXX)
