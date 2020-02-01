FIND_PACKAGE(Ruby REQUIRED)

EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r nmatrix -e "print 'true'"
	OUTPUT_VARIABLE RUBY_HAS_NMATRIX
	ERROR_QUIET)

# find vendor arch dir
EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print RbConfig::CONFIG['vendorarchdir']"
    OUTPUT_VARIABLE RUBY_VENDOR_ARCH_DIR
    ERROR_QUIET)

# find vendor arch dir
EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print RbConfig::CONFIG['archdir']"
    OUTPUT_VARIABLE RUBY_ARCH_DIR
    ERROR_QUIET)

find_program(GEM_EXECUTABLE NAMES gem)

if (GEM_EXECUTABLE)
	EXECUTE_PROCESS(COMMAND ${GEM_EXECUTABLE} which nmatrix
		OUTPUT_VARIABLE RUBY_NMATRIX_PATH
		ERROR_QUIET)

	if(RUBY_NMATRIX_PATH)
		SET(RUBY_HAS_NMATRIX 1)
		STRING(REGEX REPLACE "(.*)/nmatrix.*$" "\\1" RUBY_NMATRIX_PATH ${RUBY_NMATRIX_PATH})

		FIND_PATH(RUBY_NMATRIX_INCLUDE_DIR nmatrix.h
			HINTS ${RUBY_NMATRIX_PATH})

		FIND_FILE(RUBY_NMATRIX_LIBRARY NAMES nmatrix.bundle nmatrix.so
			HINTS ${RUBY_NMATRIX_PATH})
	endif()
endif()

if (NOT RUBY_NMATRIX_INCLUDE_DIR)
	FIND_PATH(RUBY_NMATRIX_INCLUDE_DIR nmatrix.h
		HINTS ${RUBY_VENDOR_ARCH_DIR} ${RUBY_ARCH_DIR} ${RUBY_LIBRARY} ${RUBY_POSSIBLE_LIB_DIR})
endif()

if (NOT RUBY_NMATRIX_LIBRARY)
	FIND_FILE(RUBY_NMATRIX_LIBRARY NAMES nmatrix.bundle nmatrix.so
		HINTS ${RUBY_VENDOR_ARCH_DIR} ${RUBY_ARCH_DIR} ${RUBY_LIBRARY} ${RUBY_POSSIBLE_LIB_DIR})
endif()

INCLUDE (FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(RubyNArray DEFAULT_MSG
	RUBY_HAS_NMATRIX RUBY_NMATRIX_INCLUDE_DIR RUBY_NMATRIX_LIBRARY)

MARK_AS_ADVANCED(RUBY_NMATRIX_INCLUDE_DIR RUBY_NMATRIX_LIBRARY)
