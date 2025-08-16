cmake_minimum_required(VERSION 3.10)
project(Ps2Recomp CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Define Paths to Capstone EXPLICITLY ---
# Using the exact path you provided.
set(CAPSTONE_INCLUDE_DIR "C:/Users/Owner/vcpkg/packages/capstone_x64-windows/include")
set(CAPSTONE_LIBRARY "C:/Users/Owner/vcpkg/packages/capstone_x64-windows/lib/capstone.lib")

# --- Build Your Tool ---
add_executable(recompiler_tool main.cpp recompiler.cpp)

# --- Configure Includes and Libraries ---

# Add the include directory for your own headers (e.g., cpu_state.h)
target_include_directories(recompiler_tool PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../host_app)

# Add the explicit include directory for Capstone
target_include_directories(recompiler_tool PRIVATE ${CAPSTONE_INCLUDE_DIR})

# Link against the explicit Capstone library file
target_link_libraries(recompiler_tool PRIVATE ${CAPSTONE_LIBRARY})