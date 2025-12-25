cmake_minimum_required(VERSION 3.16)
project(finalmouse LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(HIDAPI REQUIRED hidapi-hidraw)

include_directories(${HIDAPI_INCLUDE_DIRS})
link_directories(${HIDAPI_LIBRARY_DIRS})

add_executable(finalmouse main.cpp finalmouse.hpp)
target_link_libraries(finalmouse ${HIDAPI_LIBRARIES})
