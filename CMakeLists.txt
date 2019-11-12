cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

if(POLICY CMP0025)
	cmake_policy(SET CMP0025 NEW)
endif()

project(IDENTT_DIM LANGUAGES CXX)

# IDENTT version string
set(IDENTT_VERSION_MAJOR "1")
set(IDENTT_VERSION_MINOR "0")
set(IDENTT_VERSION_PATCH "0")
set(IDENTT_VERSION_STRING "${IDENTT_VERSION_MAJOR}.${IDENTT_VERSION_MINOR}.${IDENTT_VERSION_PATCH}")
set(IDENTT_VERSION_PHASE "prerelease/${CMAKE_BUILD_TYPE}")

# build documenation
option(IDENTT_BUILD_DOCS "Build Docs." OFF)

# build tools
option(IDENTT_BUILD_TOOLS "Build Loader and tools." ON)

# leveldb or rocksdb required but not both
option(IDENTT_BUILD_WITH_LEVELDB "Build internal store leveldb." OFF)
option(IDENTT_BUILD_WITH_ROCKSDB "Build internal store rocksdb." ON)

# static library unused
option(IDENTT_BUILD_STATIC "Build static library." OFF)

# prefer static
#SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})

# ctemplate support 
option(IDENTT_BUILD_WITH_CTEMPLATE "Build ctemplate support." ON)

# https inactive
option(IDENTT_BUILD_WITH_SSL "Build ssl support." OFF)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# compiler version check
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9")
	message(FATAL_ERROR "Insufficient GCC version - GCC 4.9+ required")
	endif()
	set (CMAKE_COMPILER_IS_GNUCC 1)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.1")
	message(FATAL_ERROR "Insufficient Apple clang version - XCode 5.1+ required")
	endif()
	set (CMAKE_COMPILER_IS_CLANG 1)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "3.5")
	message(FATAL_ERROR "Insufficient clang version - clang 3.5+ required")
	endif()
	set (CMAKE_COMPILER_IS_CLANG 1)
else()
	message(WARNING "Your compiler (${CMAKE_CXX_COMPILER_ID}) has not been tested on this project.")
endif()

set (CMAKE_SKIP_BUILD_RPATH false)
set (CMAKE_BUILD_WITH_INSTALL_RPATH false)
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH true)
# Ensure that RPATH is used on OSX
set(CMAKE_MACOSX_RPATH 1)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

# Enforce the C++ standard, and disable extensions
if(NOT DEFINED CMAKE_CXX_STANDARD)
	set(CMAKE_CXX_STANDARD 11)
endif()
set(CMAKE_CXX_EXTENSIONS OFF)

# If the user did not customize the install prefix,
# set it to live under build so we don't inadverently pollute /usr/local
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE)
endif()

if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE "Release")
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set (DEBUGMODE ON)
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
	# cmake bug workaround -- on some platforms, cmake doesn't set NDEBUG for RelWithDebInfo mode
	add_definitions ("-DNDEBUG")
endif ()

# disallow in-source builds
if ("${PROJECT_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
	message (FATAL_ERROR "In-source build not allowed ! Please delete CMakeFiles CMakeCache.txt")
endif ()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message (STATUS "Project source dir = ${PROJECT_SOURCE_DIR}")
message (STATUS "Project build dir = ${CMAKE_BINARY_DIR}")

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
		#set( ENV{CCACHE_SLOPPINESS} "pch_defines,time_macros" )
endif(CCACHE_FOUND)

## hack for including thirdparty
set( ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${PROJECT_SOURCE_DIR}/thirdparty/lib/pkgconfig" )
set( ENV{CMAKE_PREFIX_PATH} "${PROJECT_SOURCE_DIR}/thirdparty:$ENV{CMAKE_PREFIX_PATH}" )

## include proto
include_directories(${PROJECT_SOURCE_DIR}/proto)

# Required Packages

#find_package(threads REQUIRED)
find_package(Boost 1.59.0 COMPONENTS coroutine chrono system thread context date_time filesystem REQUIRED)
find_package(GLOG REQUIRED)
find_package(GFLAGS REQUIRED)
find_package(TCMALLOC REQUIRED)
find_package(ICU REQUIRED)
find_package(SNAPPY 1.1.0 REQUIRED)
find_package(Protobuf 3.1.0 REQUIRED)
find_package(Sodium 1.0.1 REQUIRED)
find_package(CURL 7.29 REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${GLOG_INCLUDE_DIRS})
include_directories(${GFLAGS_INCLUDE_DIRS})
include_directories(${TCMALLOC_INCLUDE_DIRS})
include_directories(${ICU_INCLUDE_DIRS})
include_directories(${SNAPPY_INCLUDE_DIRS})
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${Sodium_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)

## libs
set(IDENTT_LIB_DEPS
	${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	${GLOG_LIBRARIES}
	${GFLAGS_LIBRARIES}
	${TCMALLOC_LIBRARIES}
	${ICU_LIBRARIES}
	${SNAPPY_LIBRARIES}
	${PROTOBUF_LIBRARIES}
	${Sodium_LIBRARIES}
	${CURL_LIBRARIES}
	)

# Libunwind Linux only
find_package(UNWIND)
if (UNWIND_FOUND)
	include_directories(${UNWIND_INCLUDE_DIRS})
	set(IDENTT_LIB_DEPS ${IDENTT_LIB_DEPS} ${UNWIND_LIBRARIES})
endif()

# Find if tbb is there ( rocksdb automatically takes )
find_package(TBB)
if (TBB_FOUND)
	include_directories(${TBB_INCLUDE_DIRS})
	set(IDENTT_LIB_DEPS ${IDENTT_LIB_DEPS} ${TBB_LIBRARIES})
endif()

# Rocksdb or leveldb
if (IDENTT_BUILD_WITH_LEVELDB AND IDENTT_BUILD_WITH_ROCKSDB)
	message(FATAL_ERROR "Both leveldb or rocksdb cannot be set")
endif()
if (IDENTT_BUILD_WITH_LEVELDB)
	find_package(LEVELDB REQUIRED)
	include_directories(${LEVELDB_INCLUDE_DIRS})
	set(IDENTT_LIB_DEPS ${IDENTT_LIB_DEPS} ${LEVELDB_LIBRARIES})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIDENTT_BUILD_WITH_LEVELDB=1")
elseif (IDENTT_BUILD_WITH_ROCKSDB)
	find_package(ROCKSDB 5.2.0 REQUIRED)
	include_directories(${ROCKSDB_INCLUDE_DIRS})
	set(IDENTT_LIB_DEPS ${IDENTT_LIB_DEPS} ${ROCKSDB_LIBRARIES})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIDENTT_BUILD_WITH_ROCKSDB=1")
else()
	message(FATAL_ERROR "Either leveldb or rocksdb is required")
endif()

# Build Loader

# Optional Packages

# Optional Interfaces

if (IDENTT_BUILD_WITH_CTEMPLATE)
	find_package(CTEMPLATE REQUIRED)
	include_directories(${CTEMPLATE_INCLUDE_DIRS})
	set(IDENTT_LIB_DEPS ${IDENTT_LIB_DEPS} ${CTEMPLATE_LIBRARIES})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIDENTT_BUILD_WITH_CTEMPLATE=1")
endif()

if (IDENTT_BUILD_WITH_SSL)
	find_package(OpenSSL REQUIRED)
	include_directories(${OPENSSL_INCLUDE_DIR})
	set(IDENTT_LIB_DEPS ${IDENTT_LIB_DEPS} ${OPENSSL_SSL_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARIES})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIDENTT_BUILD_WITH_SSL=1")
endif()

if (IDENTT_BUILD_DOCS)
find_package(Doxygen)
if (DOXYGEN_FOUND)
	# set input and output files
	set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
	set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

	# request to configure the file
	configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
	message("Doxygen build started")

	# note the option ALL which allows to build the docs together with the application
	add_custom_target( docs ALL
		COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen"
	VERBATIM )
	else (DOXYGEN_FOUND)
		message("Doxygen need to be installed to generate the doxygen documentation")
	endif (DOXYGEN_FOUND)
endif()

# Flags and Options

add_definitions("-Wfatal-errors")
# Clang-specific options
if (CMAKE_COMPILER_IS_CLANG OR CMAKE_COMPILER_IS_APPLECLANG)
add_definitions ("-Wno-deprecated")
# add_definitions ("-Wno-unused-function")
# add_definitions ("-Wno-overloaded-virtual")
# add_definitions ("-Wno-unneeded-internal-declaration")
# add_definitions ("-Wno-unused-private-field")
# add_definitions ("-Wno-tautological-compare")
# disable warning about unused command line arguments
# add_definitions ("-Qunused-arguments")
# Don't warn if we ask it not to warn about warnings it doesn't know
# add_definitions ("-Wunknown-warning-option")
	if (CLANG_VERSION_STRING VERSION_GREATER 3.5 OR APPLECLANG_VERSION_STRING VERSION_GREATER 6.0)
		add_definitions ("-Wno-unused-local-typedefs")
	endif ()
elseif(CMAKE_COMPILER_IS_GNUCC)
	add_definitions("-fvisibility-inlines-hidden")
	add_definitions("-Wno-error=unused-local-typedefs")
	add_definitions("-Wno-unused-local-typedefs")
endif ()

set(CMAKE_OSX_ARCHITECTURES x86_64)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_subdirectory(proto)
add_subdirectory(src)

## for templates
INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/templates/ DESTINATION share/identt)
