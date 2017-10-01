
# - Try to find Sodium
# Once done this will define
#  Sodium_FOUND - System has Sodium
#  Sodium_INCLUDE_DIRS - The Sodium include directories
#  Sodium_LIBRARIES - The libraries needed to use Sodium
#  Sodium_DEFINITIONS - Compiler switches required for using Sodium

find_package(PkgConfig)
pkg_check_modules(PC_Sodium QUIET libsodium)
set(Sodium_DEFINITIONS ${PC_Sodium_CFLAGS_OTHER})

find_path(Sodium_INCLUDE_DIR sodium.h
       HINTS
       ${PC_Sodium_INCLUDEDIR} ${PC_Sodium_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/thirdparty/include
       /opt/local/include /usr/local/include )

find_library(Sodium_LIBRARY NAMES libsodium.a libsodium sodium
       HINTS ${PC_Sodium_LIBDIR} ${PC_Sodium_LIBRARY_DIRS}
             ${PROJECT_SOURCE_DIR}/thirdparty/lib64 ${PROJECT_SOURCE_DIR}/thirdparty/lib
       /opt/local/lib /usr/local/lib64 /usr/local/lib)

set(Sodium_LIBRARIES ${Sodium_LIBRARY} )
set(Sodium_INCLUDE_DIRS ${Sodium_INCLUDE_DIR} )

#include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set Sodium_FOUND to TRUE
# if all listed variables are TRUE

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(Sodium_PROCESS_INCLUDES Sodium_INCLUDE_DIR Sodium_INCLUDE_DIRS)
set(Sodium_PROCESS_LIBS Sodium_LIBRARY Sodium_LIBRARIES)
if (CMAKE_VERSION LESS 2.8.3)
	find_package_handle_standard_args(Sodium DEFAULT_MSG Sodium_INCLUDE_DIR Sodium_LIBRARY)
else ()
	find_package_handle_standard_args(Sodium REQUIRED_VARS Sodium_INCLUDE_DIR Sodium_LIBRARY)
endif ()
#mark_as_advanced(Sodium_INCLUDE_DIR Sodium_LIBRARY )
