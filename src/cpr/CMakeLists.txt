include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(IDENTT_CPR_SOURCES
	auth.cpp
	cookies.cpp
	cprtypes.cpp
	digest.cpp
	error.cpp
	multipart.cpp
	parameters.cpp
	payload.cpp
	proxies.cpp
	session.cpp
	timeout.cpp
	util.cpp
	ssl_options.cpp
)

add_library(identt_cpr STATIC ${IDENTT_CPR_SOURCES})
