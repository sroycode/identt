include_directories(${CMAKE_CURRENT_BINARY_DIR})

# identt_keygen

add_executable(identt_keygen
	KeyGen.cc
	../crypto/CryptoBase.cc
	../crypto/Ed25519.cc
)
target_link_libraries(identt_keygen
	${GLOG_LIBRARIES}
	${GFLAGS_LIBRARIES}
	${Sodium_LIBRARIES}
	identt_crypto
)
set(TOOL_TARGETS ${TOOL_TARGETS} identt_keygen)

# identt_getmail

set(IDENTT_GETMAIL_LIB_DEPS
	${CMAKE_THREAD_LIBS_INIT}
	${GLOG_LIBRARIES}
	${GFLAGS_LIBRARIES}
	${Boost_LIBRARIES}
	${PROTOBUF_LIBRARIES}
	${Sodium_LIBRARIES}
	${CURL_LIBRARIES}
)

add_executable(identt_getmail
	GetMail.cc
	../crypto/CryptoBase.cc
	../crypto/Ed25519.cc
	../query/JsonElem.cc
	../query/ProtoJson.cc
	../query/ProtoJsonSigned.cc
	../query/ProtoForm.cc
	../query/SignedJson.cc
)
target_link_libraries(identt_getmail
	${IDENTT_GETMAIL_LIB_DEPS}
	identt_proto
	identt_crypto
	identt_cpr
)
set(TOOL_TARGETS ${TOOL_TARGETS} identt_getmail)

# identt_dumpdb
set(IDENTT_DUMPDB_LIB_DEPS
	${GLOG_LIBRARIES}
	${GFLAGS_LIBRARIES}
	${Boost_LIBRARIES}
	${TCMALLOC_LIBRARIES}
	${SNAPPY_LIBRARIES}
	${PROTOBUF_LIBRARIES}
)

if (IDENTT_BUILD_WITH_LEVELDB)
	set(IDENTT_DUMPDB_LIB_DEPS ${IDENTT_DUMPDB_LIB_DEPS} ${LEVELDB_LIBRARIES})
elseif (IDENTT_BUILD_WITH_ROCKSDB)
	set(IDENTT_DUMPDB_LIB_DEPS ${IDENTT_DUMPDB_LIB_DEPS} ${ROCKSDB_LIBRARIES})
else()
	message(FATAL_ERROR "Either leveldb or rocksdb is required")
endif()

if (UNWIND_FOUND)
	set(IDENTT_DUMPDB_LIB_DEPS ${IDENTT_DUMPDB_LIB_DEPS} ${UNWIND_LIBRARIES})
endif()

add_executable(identt_dumpdb
	DumpDb.cc
  ../store/StoreBase.cc
  ../store/StoreLevel.cc
)

target_link_libraries(identt_dumpdb
	${IDENTT_DUMPDB_LIB_DEPS}
	identt_proto
)
set(TOOL_TARGETS ${TOOL_TARGETS} identt_dumpdb)

# all targets

install(TARGETS ${TOOL_TARGETS}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

