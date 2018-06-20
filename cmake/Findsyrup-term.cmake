# - Try to find duktape
# Once done this will define
#  SYRUP_TERM_FOUND - System has duktape
#  SYRUP_TERM_INCLUDE_DIRS - The duktape include directories
#  SYRUP_TERM_LIBRARIES - The libraries needed to use duktape
#  SYRUP_TERM_DEFINITIONS - Compiler switches required for using duktape

find_package(PkgConfig)
pkg_check_modules(PC_CSYSTEM QUIET syrup-term)
set(SYRUP_TERM_DEFINITIONS ${PC_SYRUP_TERM_CFLAGS_OTHER})

find_path(SYRUP_TERM_INCLUDE_DIR term.h form.h edit.h line-edit.h list-edit.h
          HINTS ${PC_SYRUP_TERM_INCLUDEDIR} ${PC_SYRUP_TERM_INCLUDE_DIRS}
          PREFIX syrup)

find_library(SYRUP_TERM_LIBRARY NAMES syrup-term
             HINTS ${PC_SYRUP_TERM_LIBDIR} ${PC_SYRUP_TERM_LIBRARY_DIRS} )

include(FindPackageHandleStandardArgs)

if(NOT SYRUP_TERM_INCLUDE_DIR OR NOT SYRUP_TERM_LIBRARY)
    add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/syrup-term)
    set(SYRUP_TERM_LIBRARY syrup-term)
    set(SYRUP_TERM_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/syrup-term/include)
    set(SYRUP_TERM_FOUND on)
    message(STATUS "Using local syrup-term")
else()
    message(STATUS "Found system syrup-term")
    set(SYRUP_TERM_FOUND on)
endif(NOT SYRUP_TERM_INCLUDE_DIR OR NOT SYRUP_TERM_LIBRARY)

mark_as_advanced(SYRUP_TERM_INCLUDE_DIR SYRUP_TERM_LIBRARY)

set(SYRUP_TERM_LIBRARIES ${SYRUP_TERM_LIBRARY} )
set(SYRUP_TERM_INCLUDE_DIRS ${SYRUP_TERM_INCLUDE_DIR} )