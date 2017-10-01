include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(IDENTT_HTTP_SOURCES
	WebServer.cc
)

if (IDENTT_BUILD_WITH_SSL)
set(IDENTT_HTTP_SOURCES ${IDENTT_HTTP_SOURCES} SecureWebServer.cc)
endif()

add_library(identt_http STATIC ${IDENTT_HTTP_SOURCES})
target_link_libraries(identt_http
	identt_query
)
