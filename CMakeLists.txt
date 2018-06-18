cmake_minimum_required(VERSION 3.0)
project(strips)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

option(STRIPS_BUILD_CLI "build cli" off)
option(STRIPS_BUILD_TEST "build test" off)
option(STRIPS_BUILD_CXX "build c++" off)
option(STRIPS_BUILD_CURL "build curl" off)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

include_directories(
    include 
    vendor/duktape-2.2.1/src
    vendor/syrup-path/include
    vendor/syrup-term/include
    vendor/syrup-fs/include
    vendor/syrup-term/vendor/syrup-buffer/include
    vendor/which
)

if(STRIPS_BUILD_CLI)
    set(STRIPS_BUILD_CXX on)
    set(STRIPS_BUILD_CURL on)
endif(STRIPS_BUILD_CLI)

set(CS_BUILD_TERMINAL on)

if (STRIPS_BUILD_CXX)
   set(CS_BUILD_CXX on)
endif(STRIPS_BUILD_CXX)


add_subdirectory(vendor/syrup-path)
add_subdirectory(vendor/syrup-term)
add_subdirectory(vendor/syrup-fs)
add_subdirectory(src)


if(STRIPS_BUILD_CURL)
    add_subdirectory(src/curl)
endif(STRIPS_BUILD_CURL)


if(STRIPS_BUILD_CXX)
    set(CMAKE_CXX_STANDARD 14)
    add_subdirectory(cxx)
endif(STRIPS_BUILD_CXX)

if (STRIPS_BUILD_CLI)
    add_subdirectory(strips)
endif(STRIPS_BUILD_CLI)

if (STRIPS_BUILD_TEST)
    include(CTest)
    add_subdirectory(vendor/unity)
    add_subdirectory(test)
endif(STRIPS_BUILD_TEST)