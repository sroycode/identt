
# - Try to find NANOMSG
# Once done this will define
#  NANOMSG_FOUND - System has NANOMSG
#  NANOMSG_INCLUDE_DIRS - The NANOMSG include directories
#  NANOMSG_LIBRARIES - The libraries needed to use NANOMSG

find_package(PkgConfig)
pkg_check_modules(PC_NANOMSG QUIET nanomsg)
set(NANOMSG_DEFINITIONS ${PC_NANOMSG_CFLAGS_OTHER})

find_path(NANOMSG_INCLUDE_DIR nanomsg/nn.h
       HINTS
       ${PC_NANOMSG_INCLUDEDIR} ${PC_NANOMSG_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/thirdparty/include
       /opt/local/include /usr/local/include )

find_library(NANOMSG_LIBRARY NAMES libnanomsg.a nanomsg
       HINTS ${PC_NANOMSG_LIBDIR} ${PC_NANOMSG_LIBRARY_DIRS}
       ${PROJECT_SOURCE_DIR}/thirdparty/lib64 ${PROJECT_SOURCE_DIR}/thirdparty/lib
       /opt/local/lib /usr/local/lib64 /usr/local/lib)

set(NANOMSG_LIBRARIES ${NANOMSG_LIBRARY} )
set(NANOMSG_INCLUDE_DIRS ${NANOMSG_INCLUDE_DIR} )

#include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set NANOMSG_FOUND to TRUE
# if all listed variables are TRUE

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(NANOMSG_PROCESS_INCLUDES NANOMSG_INCLUDE_DIR NANOMSG_INCLUDE_DIRS)
set(NANOMSG_PROCESS_LIBS NANOMSG_LIBRARY NANOMSG_LIBRARIES)
if (CMAKE_VERSION LESS 2.8.3)
	find_package_handle_standard_args(NANOMSG DEFAULT_MSG NANOMSG_INCLUDE_DIR NANOMSG_LIBRARY)
else ()
	find_package_handle_standard_args(NANOMSG REQUIRED_VARS NANOMSG_INCLUDE_DIR NANOMSG_LIBRARY)
endif ()
#mark_as_advanced(NANOMSG_INCLUDE_DIR NANOMSG_LIBRARY )
