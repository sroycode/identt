# - Try to find UNWIND
# Once done this will define
#  UNWIND_FOUND - System has UNWIND
#  UNWIND_INCLUDE_DIRS - The UNWIND include directories
#  UNWIND_LIBRARIES - The libraries needed to use UNWIND

find_package(PkgConfig)
pkg_check_modules(PC_UNWIND QUIET unwind)
set(UNWIND_DEFINITIONS ${PC_UNWIND_CFLAGS_OTHER})

find_path(UNWIND_INCLUDE_DIR unwind.h
	HINTS ${PC_UNWIND_INCLUDEDIR} ${PC_UNWIND_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/thirdparty/include
	/usr/include /usr/local/include /opt/local/include
)
find_library(UNWIND_LIBRARY NAMES unwind
	HINTS ${PC_UNWIND_LIBDIR} ${PC_UNWIND_LIBRARY_DIRS}
	${PROJECT_SOURCE_DIR}/thirdparty/lib64 ${PROJECT_SOURCE_DIR}/thirdparty/lib
	/usr/lib /usr/lib64 /usr/local/lib /opt/local/lib
)

set(UNWIND_LIBRARIES ${UNWIND_LIBRARY} )
set(UNWIND_INCLUDE_DIRS ${UNWIND_INCLUDE_DIR} )

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
if (CMAKE_VERSION LESS 2.8.3)
  find_package_handle_standard_args(UNWIND DEFAULT_MSG UNWIND_INCLUDE_DIR UNWIND_LIBRARY)
else ()
  find_package_handle_standard_args(UNWIND REQUIRED_VARS UNWIND_INCLUDE_DIR UNWIND_LIBRARY)
endif ()

if (UNWIND_FOUND)
set(UNWIND_PROCESS_INCLUDES UNWIND_INCLUDE_DIR UNWIND_INCLUDE_DIRS)
set(UNWIND_PROCESS_LIBS UNWIND_LIBRARY UNWIND_LIBRARIES)
endif()
