include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(IDENTT_ASYNCPP_SOURCES
	scheduler.cpp
	threadpool_scheduler.cpp
)

add_library(identt_async STATIC ${IDENTT_ASYNCPP_SOURCES})
