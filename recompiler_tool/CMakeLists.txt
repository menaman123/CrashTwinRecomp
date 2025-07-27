cmake_minimum_required(VERSION 3.14)
project(RecompilerTool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find Capstone Library (Robust Method) ---

# Find the path to the header file (capstone.h)
find_path(CAPSTONE_INCLUDE_DIR NAMES capstone/capstone.h)

# Find the compiled library file (libcapstone.so or similar)
find_library(CAPSTONE_LIBRARY NAMES capstone)

# Check if we found everything and fail if we didn't
if(NOT CAPSTONE_INCLUDE_DIR OR NOT CAPSTONE_LIBRARY)
    message(FATAL_ERROR "Capstone library not found! Please install libcapstone-dev (or equivalent)")
endif()

# Create a modern CMake target for Capstone
add_library(Capstone::capstone UNKNOWN IMPORTED)
set_target_properties(Capstone::capstone PROPERTIES
    IMPORTED_LOCATION "${CAPSTONE_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${CAPSTONE_INCLUDE_DIR}"
)

# --- Build Your Tool ---

# Add the recompiler executable
add_executable(recompiler_tool main.cpp)

# Tell CMake to add the host_app directory to the include path for our target
target_include_directories(recompiler_tool PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../host_app)

# Link it against the Capstone target we just created
target_link_libraries(recompiler_tool PRIVATE Capstone::capstone)