include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(IDENTT_WORK_SOURCES
	WorkServer.cc
)

add_library(identt_work STATIC ${IDENTT_WORK_SOURCES})
target_link_libraries(identt_work
	identt_hrpc
)

set(IDENTT_LIB_DEPS
	${IDENTT_LIB_DEPS}
	identt_utils
	identt_async
	identt_work
	identt_http
)

set(IDENTT_SOURCES
	DefaultServer.cc
)

add_executable(identtserver ${IDENTT_SOURCES})
target_link_libraries(identtserver ${IDENTT_LIB_DEPS})

install(TARGETS ${IDENTT_TARGET_LIBS} identtserver
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

