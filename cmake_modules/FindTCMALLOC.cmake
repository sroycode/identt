
# - Try to find TCMALLOC
# Once done this will define
#  TCMALLOC_FOUND - System has TCMALLOC
#  TCMALLOC_INCLUDE_DIRS - The TCMALLOC include directories
#  TCMALLOC_LIBRARIES - The libraries needed to use TCMALLOC

find_package(PkgConfig)
pkg_check_modules(PC_TCMALLOC QUIET tcmalloc)
set(TCMALLOC_DEFINITIONS ${PC_TCMALLOC_CFLAGS_OTHER})

find_path(TCMALLOC_INCLUDE_DIR gperftools/tcmalloc.h
       HINTS
       ${PC_TCMALLOC_INCLUDEDIR} ${PC_TCMALLOC_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/thirdparty/include
       /usr/include /opt/local/include /usr/local/include )

find_library(TCMALLOC_LIBRARY NAMES libtcmalloc.a
       HINTS ${PC_TCMALLOC_LIBDIR} ${PC_TCMALLOC_LIBRARY_DIRS} ${PROJECT_SOURCE_DIR}/thirdparty/lib
       /usr/lib64 /usr/lib /opt/local/lib /usr/local/lib64 /usr/local/lib)

set(TCMALLOC_LIBRARIES ${TCMALLOC_LIBRARY})
set(TCMALLOC_INCLUDE_DIRS ${TCMALLOC_INCLUDE_DIR} )

#include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set TCMALLOC_FOUND to TRUE
# if all listed variables are TRUE

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(TCMALLOC_PROCESS_INCLUDES TCMALLOC_INCLUDE_DIR TCMALLOC_INCLUDE_DIRS)
set(TCMALLOC_PROCESS_LIBS TCMALLOC_LIBRARY TCMALLOC_LIBRARIES)
if (CMAKE_VERSION LESS 2.8.3)
	find_package_handle_standard_args(TCMALLOC DEFAULT_MSG TCMALLOC_INCLUDE_DIR TCMALLOC_LIBRARY)
else ()
	find_package_handle_standard_args(TCMALLOC REQUIRED_VARS TCMALLOC_INCLUDE_DIR TCMALLOC_LIBRARY)
endif ()
#mark_as_advanced(TCMALLOC_INCLUDE_DIR TCMALLOC_LIBRARY )
