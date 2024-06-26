cmake_minimum_required(VERSION 3.15)

project(numerical_integration
        LANGUAGES CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

find_package(Boost REQUIRED
             COMPONENTS PROGRAM_OPTIONS)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_executable(sequential
               ${SRC_DIR}/sequential.cpp)

target_include_directories(sequential
                           PRIVATE ${INCLUDE_DIR})

target_link_libraries(sequential
                      PRIVATE m Boost::program_options)

add_executable(parallel
               ${SRC_DIR}/parallel.cpp ${SRC_DIR}/parallel_integrator.cpp)

target_include_directories(parallel
                           PRIVATE ${INCLUDE_DIR})

target_link_libraries(parallel
                      PRIVATE m ${CMAKE_THREAD_LIBS_INIT} Boost::program_options)
