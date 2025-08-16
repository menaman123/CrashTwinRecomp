
cmake_minimum_required(VERSION 3.14)
project(Ps2Recomp CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Define Paths to Capstone EXPLICITLY ---
set(CAPSTONE_INCLUDE_DIR "C:/Users/Owner/vcpkg/packages/capstone_x64-windows/include")
set(CAPSTONE_LIBRARY "C:/Users/Owner/vcpkg/packages/capstone_x64-windows/lib/capstone.lib")

# --- Build Your Tool ---
add_executable(recompiler_tool main.cpp recompiler.cpp)

target_include_directories(recompiler_tool PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../host_app)
target_include_directories(recompiler_tool PRIVATE ${CAPSTONE_INCLUDE_DIR})
target_link_libraries(recompiler_tool PRIVATE ${CAPSTONE_LIBRARY})

# --- Testing Setup ---

# Enable testing
enable_testing()

# Force gtest to use the same dynamic runtime as our project (for MSVC)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Download GoogleTest automatically
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# Create the test executable
add_executable(RecompilerTests recompiler_test.cpp recompiler.cpp)

# Link the test executable against GoogleTest and Capstone
target_include_directories(RecompilerTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../host_app)
target_include_directories(RecompilerTests PRIVATE ${CAPSTONE_INCLUDE_DIR})
target_link_libraries(RecompilerTests PRIVATE gtest_main ${CAPSTONE_LIBRARY})

# Discover and add the tests to CTest
include(GoogleTest)
gtest_discover_tests(RecompilerTests)
