cmake_minimum_required(VERSION 3.16)
project(finalmouse LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the build type to Release explicitly (to disable debug build)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)

find_package(PkgConfig REQUIRED)
pkg_check_modules(HIDAPI REQUIRED hidapi-hidraw)

include_directories(${HIDAPI_INCLUDE_DIRS})
link_directories(${HIDAPI_LIBRARY_DIRS})

# Add the finalmouse executable
add_executable(finalmouse main.cpp finalmouse.hpp)

# Link to the HIDAPI library
target_link_libraries(finalmouse ${HIDAPI_LIBRARIES})

# Optional: If you still want to make sure there are no debug flags:
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "")

# Ensure that debug is fully disabled
set(CMAKE_BUILD_TYPE Release CACHE STRING "Set build type to Release" FORCE)
