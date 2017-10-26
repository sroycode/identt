include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(IDENTT_HRPC_SOURCES
	HrpcClient.cc
	SyncServer.cc
)

add_library(identt_hrpc STATIC ${IDENTT_HRPC_SOURCES})
target_link_libraries(identt_hrpc
	identt_store
	identt_async
	identt_cpr
)
