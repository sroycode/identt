# - Try to find LEVELDB
# Once done this will define
#  LEVELDB_FOUND - System has LEVELDB
#  LEVELDB_INCLUDE_DIRS - The LEVELDB include directories
#  LEVELDB_LIBRARIES - The libraries needed to use LEVELDB

find_package(PkgConfig)
pkg_check_modules(PC_LEVELDB QUIET leveldb)
set(LEVELDB_DEFINITIONS ${PC_LEVELDB_CFLAGS_OTHER})

find_path(LEVELDB_INCLUDE_DIR leveldb/db.h
	HINTS ${PC_LEVELDB_INCLUDEDIR} ${PC_LEVELDB_INCLUDE_DIRS}
		${PROJECT_SOURCE_DIR}/thirdparty/include
		/usr/include /usr/local/include /opt/local/include)
find_library(LEVELDB_LIBRARY NAMES libleveldb.a libleveldb
	HINTS ${PC_LEVELDB_LIBDIR} ${PC_LEVELDB_LIBRARY_DIRS}
		${PROJECT_SOURCE_DIR}/thirdparty/lib
		/usr/lib /usr/lib64 /usr/local/lib /opt/local/lib)

set(LEVELDB_LIBRARIES ${LEVELDB_LIBRARY} )
set(LEVELDB_INCLUDE_DIRS ${LEVELDB_INCLUDE_DIR} )

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
if (CMAKE_VERSION LESS 2.8.3)
  find_package_handle_standard_args(LEVELDB DEFAULT_MSG LEVELDB_INCLUDE_DIR LEVELDB_LIBRARY)
else ()
  find_package_handle_standard_args(LEVELDB REQUIRED_VARS LEVELDB_INCLUDE_DIR LEVELDB_LIBRARY)
endif ()

if (LEVELDB_FOUND)
set(LEVELDB_PROCESS_INCLUDES LEVELDB_INCLUDE_DIR LEVELDB_INCLUDE_DIRS)
set(LEVELDB_PROCESS_LIBS LEVELDB_LIBRARY LEVELDB_LIBRARIES)
endif()
