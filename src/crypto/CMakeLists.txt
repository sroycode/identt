include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(IDENTT_CRYPTO_SOURCES
	CryptoBase.cc
	Ed25519.cc
)

add_library(identt_crypto STATIC ${IDENTT_CRYPTO_SOURCES})
target_link_libraries(identt_crypto
	${Sodium_LIBRARIES}
)

