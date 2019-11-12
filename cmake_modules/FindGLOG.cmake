# - Try to find Glog
#
# The following variables are optionally searched for defaults
#  GLOG_ROOT_DIR:            Base directory where all GLOG components are found
#
# The following are set after configuration is done: 
#  GLOG_FOUND
#  GLOG_INCLUDE_DIRS
#  GLOG_LIBRARIES
#  GLOG_LIBRARYRARY_DIRS

include(FindPackageHandleStandardArgs)

set(GLOG_ROOT_DIR "" CACHE PATH "Folder contains Google glog")

find_path(GLOG_INCLUDE_DIR glog/logging.h
       HINTS ${PC_GLOG_INCLUDEDIR} ${PC_GLOG_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/thirdparty/include
       /usr/include /opt/local/include /usr/local/include )

find_library(GLOG_LIBRARY glog libglog
       HINTS ${PC_GLOG_LIBDIR} ${PC_GLOG_LIBRARY_DIRS} ${PROJECT_SOURCE_DIR}/thirdparty/lib
       /usr/lib64 /usr/lib /opt/local/lib /usr/local/lib64 /usr/local/lib /usr/lib/x86_64-linux-gnu)

if (CMAKE_VERSION LESS 2.8.3)
	find_package_handle_standard_args(GLOG DEFAULT_MSG GLOG_INCLUDE_DIR GLOG_LIBRARY)
else ()
	find_package_handle_standard_args(GLOG REQUIRED_VARS GLOG_INCLUDE_DIR GLOG_LIBRARY)
endif ()

if(GLOG_FOUND)
    set(GLOG_INCLUDE_DIRS ${GLOG_INCLUDE_DIR})
    set(GLOG_LIBRARIES ${GLOG_LIBRARY})
endif()
