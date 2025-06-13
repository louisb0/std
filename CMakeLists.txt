cmake_minimum_required(VERSION 3.14)
project(stl)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  googlebenchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.9.4.zip
)
FetchContent_MakeAvailable(googlebenchmark)

file(GLOB TEST_SOURCES "tests/*.cpp")
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests gtest gtest_main)
target_include_directories(tests PRIVATE include)

file(GLOB BENCH_SOURCES "benchmarks/*.cpp")
add_executable(benchmarks ${BENCH_SOURCES})
target_link_libraries(benchmarks benchmark)
target_include_directories(benchmarks PRIVATE include)
