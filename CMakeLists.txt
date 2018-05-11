cmake_minimum_required(VERSION 3.0)
project(strips)

include_directories(
    include 
    vendor/duktape-2.2.1/src
    vendor/csystem/include
)

add_subdirectory(vendor/csystem)
add_subdirectory(src)
add_subdirectory(src/io)

include(CTest)
add_subdirectory(vendor/unity)
add_subdirectory(test)