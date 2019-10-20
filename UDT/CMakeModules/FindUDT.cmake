# - Find UDT
# Find the UDT headers
#
#  UDT_FOUND      = Set to true, if all components of UDT have been found.
#  UDT_INCLUDES   = Include path for the header files of UDT
#  UDT_LIBRARIES  = Link these to use UDT
if (NOT UDT_FOUND)
	if (NOT UDT_ROOT)
		set (UDT_ROOT ${CMAKE_INSTALL_PREFIX})
	endif (NOT UDT_ROOT)

	find_path (UDT_INCLUDES
		NAMES customer.h
		HINTS ${UDT_ROOT}
			${CMAKE_INSTALL_PREFIX}
		PATH_SUFFIXES include
    )
	
	find_library (UDT_LIBRARIES customer
			HINTS ${UDT_ROOT}
				${CMAKE_INSTALL_PREFIX}
			PATH_SUFFIXES lib
				bin
    )

	include (FindPackageHandleStandardArgs)
	 
	find_package_handle_standard_args (UDT
			DEFAULT_MSG
			UDT_LIBRARIES
			UDT_INCLUDES)

	if (UDT_FOUND)
		if (NOT UDT_FIND_QUIETLY)
			message (STATUS "UDT_ROOT  = ${UDT_ROOT}")
			message (STATUS "UDT_INCLUDES  = ${UDT_INCLUDES}")
			message (STATUS "UDT_LIBRARIES = ${UDT_LIBRARIES}")
		endif (NOT UDT_FIND_QUIETLY)
	else (UDT_FOUND)
		if (UDT_FIND_REQUIRED)
			message (FATAL_ERROR "Could not find UDT!")
		endif (UDT_FIND_REQUIRED)
	endif (UDT_FOUND)
	
	mark_as_advanced (
		UDT_ROOT
		UDT_INCLUDES
		UDT_LIBRARIES
    )
endif (NOT UDT_FOUND)