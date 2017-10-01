# - Try to find Glog
#
# The following variables are optionally searched for defaults
#  GFLAGS_ROOT_DIR:            Base directory where all GFLAGS components are found
#
# The following are set after configuration is done: 
#  GFLAGS_FOUND
#  GFLAGS_INCLUDE_DIRS
#  GFLAGS_LIBRARIES
#  GFLAGS_LIBRARYRARY_DIRS

include(FindPackageHandleStandardArgs)

set(GFLAGS_ROOT_DIR "" CACHE PATH "Folder contains Google glog")

find_path(GFLAGS_INCLUDE_DIR gflags/gflags.h
       HINTS ${PC_GFLAGS_INCLUDEDIR} ${PC_GFLAGS_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/thirdparty/include
       /usr/include /opt/local/include /usr/local/include )

find_library(GFLAGS_LIBRARY gflags libflags
       HINTS ${PC_GFLAGS_LIBDIR} ${PC_GFLAGS_LIBRARY_DIRS} ${PROJECT_SOURCE_DIR}/thirdparty/lib
       /usr/lib64 /usr/lib /opt/local/lib /usr/local/lib64 /usr/local/lib)

if (CMAKE_VERSION LESS 2.8.3)
	find_package_handle_standard_args(GFLAGS DEFAULT_MSG GFLAGS_INCLUDE_DIR GFLAGS_LIBRARY)
else ()
	find_package_handle_standard_args(GFLAGS REQUIRED_VARS GFLAGS_INCLUDE_DIR GFLAGS_LIBRARY)
endif ()

if(GFLAGS_FOUND)
    set(GFLAGS_INCLUDE_DIRS ${GFLAGS_INCLUDE_DIR})
    set(GFLAGS_LIBRARIES ${GFLAGS_LIBRARY})
endif()
