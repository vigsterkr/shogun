set(BITSERY_PREFIX ${CMAKE_BINARY_DIR}/bitsery)
set(BITSERY_INCLUDE_DIR "${BITSERY_PREFIX}/src/bitsery/include")

include(ExternalProject)
ExternalProject_Add(
	bitsery
	PREFIX ${BITSERY_PREFIX}
	DOWNLOAD_DIR ${THIRD_PARTY_DIR}/bitsery
	URL https://github.com/fraillt/bitsery/archive/v5.0.1.tar.gz
	URL_MD5 479b4a1a444af215bfc70b4e395e5886
	INSTALL_COMMAND ""
)

LIST(APPEND SHOGUN_DEPENDS bitsery)
