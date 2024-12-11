cmake_minimum_required(VERSION 3.21)

project(lab-1
        LANGUAGES CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

find_package(fmt REQUIRED)
find_package(CLI11 REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Boost REQUIRED
             COMPONENTS MPI)

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

target_include_directories(sequential PRIVATE
    ./include
)


add_executable(parallel_omp
    ./src/parallel_omp.cpp
)

set_target_properties(parallel_omp PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(parallel_omp PRIVATE
    fmt::fmt
    CLI11::CLI11
    llvm-openmp::llvm-openmp
)

target_include_directories(parallel_omp PRIVATE
    ./include
)


add_executable(parallel_mpi
    ./src/parallel_mpi.cpp
)

set_target_properties(parallel_mpi PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(parallel_mpi PRIVATE
    fmt::fmt
    CLI11::CLI11
    Boost::mpi
)

target_include_directories(parallel_mpi PRIVATE
    ./include
)
