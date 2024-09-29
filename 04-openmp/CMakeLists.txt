cmake_minimum_required(VERSION 3.20)

project(OMP_HELLO
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

find_package(OpenMP REQUIRED)
find_package(Boost REQUIRED
             COMPONENTS PROGRAM_OPTIONS)

set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_executable(hello ${SOURCE_DIR}/hello.cpp)
target_link_libraries(hello PRIVATE OpenMP::OpenMP_CXX)

add_executable(harmonic_series ${SOURCE_DIR}/harmonic-series.cpp)
target_link_libraries(harmonic_series
                      PRIVATE OpenMP::OpenMP_CXX
                      PRIVATE Boost::program_options)

add_executable(ordered_access ${SOURCE_DIR}/ordered-access.cpp)
target_link_libraries(ordered_access PRIVATE OpenMP::OpenMP_CXX)

add_executable(nested ${SOURCE_DIR}/nested.cpp)
target_link_libraries(nested PRIVATE OpenMP::OpenMP_CXX)

add_executable(matrix_multiplication ${SOURCE_DIR}/matrix.cpp)
target_link_libraries(matrix_multiplication
                      PRIVATE Boost::program_options)
target_include_directories(matrix_multiplication PRIVATE ${INCLUDE_DIR})

add_executable(matrix_multiplication_parallel ${SOURCE_DIR}/matrix.cpp)
target_link_libraries(matrix_multiplication_parallel
                      PRIVATE Boost::program_options
                      PRIVATE OpenMP::OpenMP_CXX)
target_include_directories(matrix_multiplication_parallel PRIVATE ${INCLUDE_DIR})
