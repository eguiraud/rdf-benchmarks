# Get Google benchmark -- either from the system or from github
find_package(benchmark QUIET)
if (NOT benchmark_FOUND)
   message(STATUS "Could not find a local installation of google benchmark.")
   message(STATUS "It will be fetched from GitHub and built together with the project.")
   # Download google benchmark at CMake configuration time, add it as a subproject
   include(FetchContent)
   FetchContent_Declare(
      benchmark
      GIT_REPOSITORY https://github.com/google/benchmark
      GIT_TAG v1.7.1
   )
   FetchContent_Populate(benchmark)
   set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON CACHE BOOL "Download gtest if needed")
   set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable building of gbench tests (C++17-incompatible)")
   set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "Disable installation of gbench (it's a private dependency)")
   add_subdirectory(${benchmark_SOURCE_DIR} ${benchmark_BINARY_DIR})
   add_library(benchmark::benchmark ALIAS benchmark)
endif()

# Get ROOT -- always from the system
message(STATUS "Looking for ROOT")
find_package(ROOT REQUIRED COMPONENTS ROOTDataFrame)
message(STATUS "ROOT ${ROOT_VERSION} found at ${ROOT_BINDIR}")

add_executable(without_io without_io.cpp)
target_link_libraries(without_io PRIVATE benchmark::benchmark ROOT::ROOTDataFrame)
