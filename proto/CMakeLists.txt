PROTOBUF_GENERATE_CPP(IDENTT_PB_SRCS IDENTT_PB_HDRS
	Store.proto
	Query.proto
	Mail.proto
	Hrpc.proto
)

set(PROTO_SOURCES ${IDENTT_PB_SRCS})
set(IDENTT_SOURCES ${PROTO_SOURCES})

add_library(identt_proto STATIC ${IDENTT_SOURCES})
include_directories(${IDENTT_PB_HDRS})
