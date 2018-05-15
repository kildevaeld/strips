# - Try to find LibXml2
# Once done this will define
#  LIBXML2_FOUND - System has LibXml2
#  CSYSTEM_INCLUDE_DIRS - The LibXml2 include directories
#  LIBXML2_LIBRARIES - The libraries needed to use LibXml2
#  LIBXML2_DEFINITIONS - Compiler switches required for using LibXml2

find_package(PkgConfig)
pkg_check_modules(PC_CSYSTEM QUIET csystem)
set(CSYSTEM_DEFINITIONS ${PC_CSYSTEM_CFLAGS_OTHER})

find_path(CSYSTEM_INCLUDE_DIR csystem.h path.h terminal/terminal.h
          HINTS ${PC_CSYSTEM_INCLUDEDIR} ${PC_CSYSTEM_INCLUDE_DIRS}
          PATH_SUFFIXES csystem)

find_library(CSYSTEM_LIBRARY NAMES csystem csystem_terminal
             HINTS ${PC_CSYSTEM_LIBDIR} ${PC_CSYSTEM_LIBRARY_DIRS} )

include(FindPackageHandleStandardArgs)

if(NOT CSYSTEM_INCLUDE_DIR)
    set(CS_BUILD_TERMINAL on)
    set(CS_BUILD_CXX on)
    add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/csystem)
    include_directories(${PROJECT_SOURCE_DIR}/vendor/csystem/include)
    set(CSYSTEM_LIBRARY csystem)
    set(CSYSTEM_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/csystem/include)
    set(CSYSTEM_FOUND on)
    message(STATUS "Using local csystem")
else()
    message(STATUS "Found csystem")
    set(CSYSTEM_FOUND on)
endif(NOT CSYSTEM_INCLUDE_DIR)

mark_as_advanced(CSYSTEM_INCLUDE_DIR CSYSTEM_LIBRARY)

set(CSYSTEM_LIBRARIES ${CSYSTEM_LIBRARY} )
set(CSYSTEM_INCLUDE_DIRS ${CSYSTEM_INCLUDE_DIR} )