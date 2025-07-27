
cmake_minimum_required(VERSION 3.14)
project(HostAppTests)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the FetchContent module to download Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# Make Google Test available
FetchContent_MakeAvailable(googletest)

# Add your memory library so we can link against it
# This assumes memory.h and memory.cpp are in the same directory
add_library(memory memory.cpp)

# Add the executable for our tests
add_executable(memory_tests memory_test.cpp)

# Link our test executable against the memory library and Google Test
target_link_libraries(memory_tests memory gtest_main)

# Add the test to CTest for easy execution
include(GoogleTest)
gtest_discover_tests(memory_tests)

