include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(IDENTT_QUERY_SOURCES
	JsonElem.cc
	ProtoJson.cc
	ProtoJsonSigned.cc
	ProtoForm.cc
	SignedJson.cc
	HttpClient.cc
)

add_library(identt_query STATIC ${IDENTT_QUERY_SOURCES})
target_link_libraries(identt_query
	identt_hrpc
)
