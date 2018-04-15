include(ExternalProject)
SET(STRINGVIEWLITE_SOURCE_DIR )
ExternalProject_Add(
	StringViewLite
	URL https://github.com/martinmoene/string-view-lite/archive/v1.1.0.tar.gz
	URL_MD5 aac103bb404f0b2b688320ed70ec4cb3
	TIMEOUT 10
	PREFIX ${CMAKE_BINARY_DIR}/StringViewLite
	DOWNLOAD_DIR ${THIRD_PARTY_DIR}/StringViewLite
	INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_BINARY_DIR}/StringViewLite/src/StringViewLite/include
		${THIRD_PARTY_INCLUDE_DIR}/
)

add_dependencies(libshogun StringViewLite)
