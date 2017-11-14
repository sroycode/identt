include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(IDENTT_UTILS_SOURCES
	CfgFileOptions.cc
)

add_library(identt_utils STATIC ${IDENTT_UTILS_SOURCES})
