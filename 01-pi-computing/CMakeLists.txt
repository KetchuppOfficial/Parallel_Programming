cmake_minimum_required(VERSION 3.15)

project(pi_computing
        LANGUAGES CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

find_package(Boost REQUIRED
             COMPONENTS PROGRAM_OPTIONS)

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)

add_executable(pi ${SRC_LIST})

target_link_libraries(pi
                      Boost::program_options
                      Boost::headers)

target_include_directories(pi
                           PRIVATE ${PROJECT_SOURCE_DIR}/include)
