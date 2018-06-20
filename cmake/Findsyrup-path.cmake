# - Try to find duktape
# Once done this will define
#  SYRUP_PATH_FOUND - System has duktape
#  SYRUP_PATH_INCLUDE_DIRS - The duktape include directories
#  SYRUP_PATH_LIBRARIES - The libraries needed to use duktape
#  SYRUP_PATH_DEFINITIONS - Compiler switches required for using duktape

find_package(PkgConfig)
pkg_check_modules(PC_CSYSTEM QUIET syrup-path)
set(SYRUP_PATH_DEFINITIONS ${PC_SYRUP_PATH_CFLAGS_OTHER})

find_path(SYRUP_PATH_INCLUDE_DIR path.h
          HINTS ${PC_SYRUP_PATH_INCLUDEDIR} ${PC_SYRUP_PATH_INCLUDE_DIRS}
          PREFIX syrup)

find_library(SYRUP_PATH_LIBRARY NAMES syrup-path
             HINTS ${PC_SYRUP_PATH_LIBDIR} ${PC_SYRUP_PATH_LIBRARY_DIRS} )

include(FindPackageHandleStandardArgs)

if(NOT SYRUP_PATH_INCLUDE_DIR)
    add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/syrup-path)
    set(SYRUP_PATH_LIBRARY syrup-path)
    set(SYRUP_PATH_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/syrup-path/include)
    set(SYRUP_PATH_FOUND on)
    message(STATUS "Using local syrup-path")
else()
    message(STATUS "Found system syrup-path")
    set(SYRUP_PATH_FOUND on)
endif(NOT SYRUP_PATH_INCLUDE_DIR)

mark_as_advanced(SYRUP_PATH_INCLUDE_DIR SYRUP_PATH_LIBRARY)

set(SYRUP_PATH_LIBRARIES ${SYRUP_PATH_LIBRARY} )
set(SYRUP_PATH_INCLUDE_DIRS ${SYRUP_PATH_INCLUDE_DIR} )