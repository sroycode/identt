add_subdirectory(async++)
add_subdirectory(cpr)
add_subdirectory(crypto)
add_subdirectory(store)
add_subdirectory(utils)
add_subdirectory(hrpc)
add_subdirectory(query)
add_subdirectory(http)
add_subdirectory(main)

if (IDENTT_BUILD_TOOLS)
	add_subdirectory(tools)
endif()

