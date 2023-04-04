cmake_minimum_required(VERSION 3.11)
project (rdf-benchmarks CXX)
add_compile_options(-Wall -Wextra -Wpedantic)
set(CMAKE_CXX_STANDARD "17" CACHE STRING "")

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type (Release/RelWithDebInfo/Debug)." FORCE)
   message(STATUS "Build type not specified, defaulting to ${CMAKE_BUILD_TYPE}.")
endif()

add_subdirectory(src)
