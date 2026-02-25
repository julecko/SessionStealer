cmake_minimum_required(VERSION 3.10)

# When stable set project version to 1.0.0
project(sessionstealer VERSION 0.0.0 LANGUAGES C CXX)

# -----------------------
# Global compiler settings
# -----------------------
if (CMAKE_C_COMPILER)
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_STANDARD_REQUIRED ON)
endif()

if (CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
if (CMAKE_C_COMPILER)
    message(STATUS "C Compiler: ${CMAKE_C_COMPILER_ID}")
endif()
if (CMAKE_CXX_COMPILER)
    message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

# -----------------------
# Global compiler flags per config
# -----------------------
if (MSVC)
    set(GLOBAL_DEBUG_FLAGS /Zi /Od)
    set(GLOBAL_RELEASE_FLAGS /O2)
else()
    set(GLOBAL_DEBUG_FLAGS -g -O0 -fno-omit-frame-pointer -fsanitize=address)
    set(GLOBAL_RELEASE_FLAGS -O3)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/dlls)

# -----------------------
# Add subdirectories
# -----------------------
add_subdirectory(src)
add_subdirectory(dlls/discovery)
add_subdirectory(dlls/edge)
