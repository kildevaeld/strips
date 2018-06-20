# - Try to find duktape
# Once done this will define
#  DUKTAPE_FOUND - System has duktape
#  DUKTAPE_INCLUDE_DIRS - The duktape include directories
#  DUKTAPE_LIBRARIES - The libraries needed to use duktape
#  DUKTAPE_DEFINITIONS - Compiler switches required for using duktape

find_package(PkgConfig)
pkg_check_modules(PC_CSYSTEM QUIET duktape)
set(DUKTAPE_DEFINITIONS ${PC_DUKTAPE_CFLAGS_OTHER})

find_path(DUKTAPE_INCLUDE_DIR duktape.h duk_config.h
          HINTS ${PC_DUKTAPE_INCLUDEDIR} ${PC_DUKTAPE_INCLUDE_DIRS})

find_library(DUKTAPE_LIBRARY NAMES duktape
             HINTS ${PC_DUKTAPE_LIBDIR} ${PC_DUKTAPE_LIBRARY_DIRS} )

include(FindPackageHandleStandardArgs)

if(NOT DUKTAPE_INCLUDE_DIR)
    add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/duktape)
    #include_directories(${PROJECT_SOURCE_DIR}/vendor/csystem/include)
    set(DUKTAPE_LIBRARY duktape)
    set(DUKTAPE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/duktape/src)
    set(DUKTAPE_FOUND on)
    message(STATUS "Using local duktape")
else()
    message(STATUS "Found system duktape")
    set(DUKTAPE_FOUND on)
endif(NOT DUKTAPE_INCLUDE_DIR)

mark_as_advanced(DUKTAPE_INCLUDE_DIR DUKTAPE_LIBRARY)

set(DUKTAPE_LIBRARIES ${DUKTAPE_LIBRARY} )
set(DUKTAPE_INCLUDE_DIRS ${DUKTAPE_INCLUDE_DIR} )