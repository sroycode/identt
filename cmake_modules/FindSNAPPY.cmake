# - Try to find SNAPPY
# Once done this will define
#  SNAPPY_FOUND - System has SNAPPY
#  SNAPPY_INCLUDE_DIRS - The SNAPPY include directories
#  SNAPPY_LIBRARIES - The libraries needed to use SNAPPY

find_package(PkgConfig)
pkg_check_modules(PC_SNAPPY QUIET snappy)
set(SNAPPY_DEFINITIONS ${PC_SNAPPY_CFLAGS_OTHER})

find_path(SNAPPY_INCLUDE_DIR snappy.h
	HINTS ${PC_SNAPPY_INCLUDEDIR} ${PC_SNAPPY_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/thirdparty/include
	/usr/include /usr/local/include /opt/local/include
)
find_library(SNAPPY_LIBRARY NAMES snappy
	HINTS ${PC_SNAPPY_LIBDIR} ${PC_SNAPPY_LIBRARY_DIRS}
	${PROJECT_SOURCE_DIR}/thirdparty/lib64 ${PROJECT_SOURCE_DIR}/thirdparty/lib
	/usr/lib /usr/lib64 /usr/local/lib /opt/local/lib
)

set(SNAPPY_LIBRARIES ${SNAPPY_LIBRARY} )
set(SNAPPY_INCLUDE_DIRS ${SNAPPY_INCLUDE_DIR} )

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
if (CMAKE_VERSION LESS 2.8.3)
  find_package_handle_standard_args(SNAPPY DEFAULT_MSG SNAPPY_INCLUDE_DIR SNAPPY_LIBRARY)
else ()
  find_package_handle_standard_args(SNAPPY REQUIRED_VARS SNAPPY_INCLUDE_DIR SNAPPY_LIBRARY)
endif ()

if (SNAPPY_FOUND)
set(SNAPPY_PROCESS_INCLUDES SNAPPY_INCLUDE_DIR SNAPPY_INCLUDE_DIRS)
set(SNAPPY_PROCESS_LIBS SNAPPY_LIBRARY SNAPPY_LIBRARIES)
endif()
