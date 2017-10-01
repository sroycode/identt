include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(IDENTT_WORK_SOURCES
	WorkServer.cc
)

add_library(identt_work STATIC ${IDENTT_WORK_SOURCES})
target_link_libraries(identt_work
	identt_async
	identt_crypto
	identt_store
)
