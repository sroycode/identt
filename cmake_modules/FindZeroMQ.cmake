
# - Try to find ZeroMQ
# Once done this will define
#  ZeroMQ_FOUND - System has ZeroMQ
#  ZeroMQ_INCLUDE_DIRS - The ZeroMQ include directories
#  ZeroMQ_LIBRARIES - The libraries needed to use ZeroMQ

find_package(PkgConfig)
pkg_check_modules(PC_ZeroMQ QUIET nanomsg)
set(ZeroMQ_DEFINITIONS ${PC_ZeroMQ_CFLAGS_OTHER})

find_path(ZeroMQ_INCLUDE_DIR zmq.h
       HINTS
       ${PC_ZeroMQ_INCLUDEDIR} ${PC_ZeroMQ_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/thirdparty/include
       /opt/local/include /usr/local/include )

find_library(ZeroMQ_LIBRARY NAMES libzmq.a libzmq zmq
       HINTS ${PC_ZeroMQ_LIBDIR} ${PC_ZeroMQ_LIBRARY_DIRS}
       ${PROJECT_SOURCE_DIR}/thirdparty/lib64 ${PROJECT_SOURCE_DIR}/thirdparty/lib
       /opt/local/lib /usr/local/lib64 /usr/local/lib)

set(ZeroMQ_LIBRARIES ${ZeroMQ_LIBRARY} )
set(ZeroMQ_INCLUDE_DIRS ${ZeroMQ_INCLUDE_DIR} )

#include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set ZeroMQ_FOUND to TRUE
# if all listed variables are TRUE

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(ZeroMQ_PROCESS_INCLUDES ZeroMQ_INCLUDE_DIR ZeroMQ_INCLUDE_DIRS)
set(ZeroMQ_PROCESS_LIBS ZeroMQ_LIBRARY ZeroMQ_LIBRARIES)
if (CMAKE_VERSION LESS 2.8.3)
	find_package_handle_standard_args(ZeroMQ DEFAULT_MSG ZeroMQ_INCLUDE_DIR ZeroMQ_LIBRARY)
else ()
	find_package_handle_standard_args(ZeroMQ REQUIRED_VARS ZeroMQ_INCLUDE_DIR ZeroMQ_LIBRARY)
endif ()
#mark_as_advanced(ZeroMQ_INCLUDE_DIR ZeroMQ_LIBRARY )
