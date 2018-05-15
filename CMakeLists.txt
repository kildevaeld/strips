cmake_minimum_required(VERSION 3.0)
project(strips)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

option(STRIPS_BUILD_CLI "build cli" off)
option(STRIPS_BUILD_CXX "build c++" off)

include_directories(
    include 
    vendor/duktape-2.2.1/src
    vendor/csystem/include
    vendor/which
)

if(STRIPS_BUILD_CLI)
    set(STRIPS_BUILD_CXX on)
endif(STRIPS_BUILD_CLI)

set(CS_BUILD_TERMINAL on)
if (STRIPS_BUILD_CXX)
   set(CS_BUILD_CXX on)
endif(STRIPS_BUILD_CXX)
add_subdirectory(vendor/csystem)

add_subdirectory(src)

add_subdirectory(src/io)
add_subdirectory(src/curl)
add_subdirectory(src/filesystem)


if(STRIPS_BUILD_CXX)
    set(CMAKE_CXX_STANDARD 14)
    add_subdirectory(cxx)
endif(STRIPS_BUILD_CXX)

if (STRIPS_BUILD_CLI)
    set(CMAKE_CLI_STANDARD 14)
    add_subdirectory(strips)
endif(STRIPS_BUILD_CLI)

#add_subdirectory(strips)
#include(CTest)
#add_subdirectory(vendor/unity)
#add_subdirectory(test)