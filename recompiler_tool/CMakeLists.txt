
# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(RecompilerTool CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the Capstone library package.
# The `REQUIRED` keyword will make CMake fail with an error if it can't find it.
find_package(Capstone REQUIRED)

# Add the executable target
# This tells CMake to create a program named "recompiler_tool"
# from the main.cpp source file.
add_executable(recompiler_tool main.cpp)

# Link the Capstone library to our executable.
# This makes the Capstone functions available to our code.
target_link_libraries(recompiler_tool PRIVATE Capstone::capstone)
