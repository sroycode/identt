# - Try to find ROCKSDB
# Once done this will define
#  ROCKSDB_FOUND - System has ROCKSDB
#  ROCKSDB_INCLUDE_DIRS - The ROCKSDB include directories
#  ROCKSDB_LIBRARIES - The libraries needed to use ROCKSDB

find_package(PkgConfig)
pkg_check_modules(PC_ROCKSDB QUIET rocksdb)
set(ROCKSDB_DEFINITIONS ${PC_ROCKSDB_CFLAGS_OTHER})

find_path(ROCKSDB_INCLUDE_DIR rocksdb/db.h
	HINTS ${PC_ROCKSDB_INCLUDEDIR} ${PC_ROCKSDB_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/thirdparty/include
	/usr/include /usr/local/include /opt/local/include
)

find_library(ROCKSDB_LIBRARY_MAIN NAMES librocksdb.a
	HINTS ${PC_ROCKSDB_LIBDIR} ${PC_ROCKSDB_LIBRARY_DIRS}
	${PROJECT_SOURCE_DIR}/thirdparty/lib64 ${PROJECT_SOURCE_DIR}/thirdparty/lib
	/usr/lib /usr/lib64 /usr/local/lib /opt/local/lib
)

find_library(ROCKSDB_LIBRARY_ZIP NAMES libz.a
	HINTS
	${PROJECT_SOURCE_DIR}/thirdparty/lib64 ${PROJECT_SOURCE_DIR}/thirdparty/lib
	/usr/lib /usr/lib64 /usr/local/lib /opt/local/lib
)
find_library(ROCKSDB_LIBRARY_BZIP NAMES libbz2.a libbzip2.a
	HINTS
	${PROJECT_SOURCE_DIR}/thirdparty/lib64 ${PROJECT_SOURCE_DIR}/thirdparty/lib
	/usr/lib /usr/lib64 /usr/local/lib /opt/local/lib
)
find_library(ROCKSDB_LIBRARY_LZ4 NAMES liblz4.a
	HINTS
	${PROJECT_SOURCE_DIR}/thirdparty/lib64 ${PROJECT_SOURCE_DIR}/thirdparty/lib
	/usr/lib /usr/lib64 /usr/local/lib /opt/local/lib
)
find_library(ROCKSDB_LIBRARY_SNAPPY NAMES libsnappy.a
	HINTS
	${PROJECT_SOURCE_DIR}/thirdparty/lib64 ${PROJECT_SOURCE_DIR}/thirdparty/lib
	/usr/lib /usr/lib64 /usr/local/lib /opt/local/lib
)

set(ROCKSDB_LIBRARY
	${ROCKSDB_LIBRARY_MAIN}
	${ROCKSDB_LIBRARY_ZIP}
	${ROCKSDB_LIBRARY_BZIP}
	${ROCKSDB_LIBRARY_LZ4}
	${ROCKSDB_LIBRARY_SNAPPY}
	)

set(ROCKSDB_LIBRARIES ${ROCKSDB_LIBRARY} )
set(ROCKSDB_INCLUDE_DIRS ${ROCKSDB_INCLUDE_DIR} )

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
if (CMAKE_VERSION LESS 2.8.3)
  find_package_handle_standard_args(ROCKSDB DEFAULT_MSG ROCKSDB_INCLUDE_DIR ROCKSDB_LIBRARY)
else ()
  find_package_handle_standard_args(ROCKSDB REQUIRED_VARS ROCKSDB_INCLUDE_DIR ROCKSDB_LIBRARY)
endif ()

if (ROCKSDB_FOUND)
set(ROCKSDB_PROCESS_INCLUDES ROCKSDB_INCLUDE_DIR ROCKSDB_INCLUDE_DIRS)
set(ROCKSDB_PROCESS_LIBS ROCKSDB_LIBRARY ROCKSDB_LIBRARIES)
endif()
