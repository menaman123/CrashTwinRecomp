# --- Project Setup ---
cmake_minimum_required(VERSION 3.16)
project(PS2Tools CXX)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# --- Dependencies ---

# Fetch Capstone automatically
FetchContent_Declare(
  capstone
  URL https://github.com/capstone-engine/capstone/archive/5.0.1.zip
)
FetchContent_MakeAvailable(capstone)

# Fetch GoogleTest automatically
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# --- EEAnalyze Library and Tests ---

add_library(EEAnalyze STATIC analyze.cpp)
target_include_directories(EEAnalyze PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(EEAnalyze PRIVATE capstone)

add_executable(analyze_test analyze_test.cpp)
target_link_libraries(analyze_test PRIVATE EEAnalyze gtest_main capstone)

include(GoogleTest)
gtest_discover_tests(analyze_test)

# --- Disassembler Executable ---

add_executable(disasm main.cpp)
target_link_libraries(disasm PRIVATE capstone)
target_include_directories(disasm PRIVATE "C:\\Users\\Owner\\Desktop\\PS2_Recomp\\ELFIO")
