cmake_minimum_required(VERSION 3.16)
project(finalmouse LANGUAGES CXX)

# Set the default build type to Release (no debug builds)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# Ensure the standard is set to C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the required package hidapi-hidraw
find_package(PkgConfig REQUIRED)
pkg_check_modules(HIDAPI REQUIRED hidapi-hidraw)

include_directories(${HIDAPI_INCLUDE_DIRS})
link_directories(${HIDAPI_LIBRARY_DIRS})

# Add the finalmouse executable target
add_executable(finalmouse main.cpp finalmouse.hpp)

# Link the hidapi library to the finalmouse target
target_link_libraries(finalmouse ${HIDAPI_LIBRARIES})

# Prevent any debug build options or targets from being enabled
set_target_properties(finalmouse PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES
    COMPILE_OPTIONS "-O3"   # Force optimization (Release-like behavior)
)

# Optional: if you have any debug-specific configurations, explicitly disable them
set(CMAKE_BUILD_TYPE Release CACHE STRING "Release build (no debug)" FORCE)
