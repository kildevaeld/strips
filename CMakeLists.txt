cmake_minimum_required(VERSION 3.0)
project(strips)

include_directories(
    include 
    vendor/duktape-2.2.1/src
    vendor/csystem/include
    vendor/which
)

add_subdirectory(vendor/csystem)
add_subdirectory(src)
add_subdirectory(src/io)
add_subdirectory(src/curl)
add_subdirectory(src/filesystem)

add_subdirectory(sp)

include(CTest)
add_subdirectory(vendor/unity)
add_subdirectory(test)