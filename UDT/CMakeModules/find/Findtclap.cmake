# - Find TCLAP
# Find the TCLAP headers
#
# TCLAP_INCLUDES - where to find the TCLAP headers
# TCLAP_FOUND       - True if TCLAP is found
if (NOT TCLAP_FOUND)
	if(NOT DEFINED TCLAP_ROOT)
		set(TCLAP_ROOT /usr /usr/local $ENV{TCLAP_ROOT})
	endif(NOT DEFINED TCLAP_ROOT)

	find_path (TCLAP_INCLUDES tclap/CmdLine.h
	HINTS ${TCLAP_ROOT} ${CMAKE_INSTALL_PREFIX}
	PATHS / "/tclap-1.2.1"
	PATH_SUFFIXES include
	)
	
	include (FindPackageHandleStandardArgs)
	find_package_handle_standard_args (tclap DEFAULT_MSG TCLAP_INCLUDES)
	
	mark_as_advanced (
		TCLAP_ROOT
		TCLAP_INCLUDES
    )
endif (NOT TCLAP_FOUND)