cmake_minimum_required(VERSION 3.0.0)

project(xEditor)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_RELEASE -O3)
endif ()

add_compile_definitions(XENGINE_EXPORT=)

include(cmake/config.cmake)
include(cmake/xeditor.cmake)

# Uncomment to enable syntax highlighting
# include(template/CMakeLists.txt)