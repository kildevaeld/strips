# - Try to find duktape
# Once done this will define
#  SYRUP_FS_FOUND - System has duktape
#  SYRUP_FS_INCLUDE_DIRS - The duktape include directories
#  SYRUP_FS_LIBRARIES - The libraries needed to use duktape
#  SYRUP_FS_DEFINITIONS - Compiler switches required for using duktape

find_package(PkgConfig)
pkg_check_modules(PC_CSYSTEM QUIET syrup-fs)
set(SYRUP_FS_DEFINITIONS ${PC_SYRUP_FS_CFLAGS_OTHER})

find_path(SYRUP_FS_INCLUDE_DIR fs.h
          HINTS ${PC_SYRUP_FS_INCLUDEDIR} ${PC_SYRUP_FS_INCLUDE_DIRS}
          PREFIX syrup)

find_library(SYRUP_FS_LIBRARY NAMES syrup-fs
             HINTS ${PC_SYRUP_FS_LIBDIR} ${PC_SYRUP_FS_LIBRARY_DIRS} )

include(FindPackageHandleStandardArgs)

if(NOT SYRUP_FS_INCLUDE_DIR)
    add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/syrup-fs)
    set(SYRUP_FS_LIBRARY syrup-fs)
    set(SYRUP_FS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/syrup-fs/include)
    set(SYRUP_FS_FOUND on)
    message(STATUS "Using local syrup-fs")
else()
    message(STATUS "Found system syrup-fs")
    set(SYRUP_FS_FOUND on)
endif(NOT SYRUP_FS_INCLUDE_DIR)

mark_as_advanced(SYRUP_FS_INCLUDE_DIR SYRUP_FS_LIBRARY)

set(SYRUP_FS_LIBRARIES ${SYRUP_FS_LIBRARY} )
set(SYRUP_FS_INCLUDE_DIRS ${SYRUP_FS_INCLUDE_DIR} )