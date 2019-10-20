# - Find SHARE
# Find the SHARE headers
#
#  SHARE_FOUND      = Set to true, if all components of SHARE have been found.
#  SHARE_INCLUDES   = Include path for the header files of SHARE
#  SHARE_LIBRARIES  = Link these to use SHARE
if (NOT SHARE_FOUND)
	if (NOT SHARE_ROOT)
		set (SHARE_ROOT ${CMAKE_INSTALL_PREFIX})
	endif (NOT SHARE_ROOT)

	find_path (SHARE_INCLUDES
		NAMES deftype
		HINTS ${SHARE_ROOT} ${CMAKE_INSTALL_PREFIX}
		PATH_SUFFIXES include
    )
	
	find_library (SHARE_LIBRARIES SHARE
    HINTS ${SHARE_ROOT} ${CMAKE_INSTALL_PREFIX}
    PATH_SUFFIXES lib bin
    )

	include (FindPackageHandleStandardArgs)
	 
	find_package_handle_standard_args (SHARE DEFAULT_MSG SHARE_LIBRARIES SHARE_INCLUDES)

	if (SHARE_FOUND)
		if (NOT SHARE_FIND_QUIETLY)
			message (STATUS "Found components for log4cplus")
			message (STATUS "SHARE_ROOT  = ${SHARE_ROOT}")
			message (STATUS "SHARE_INCLUDES  = ${SHARE_INCLUDES}")
			message (STATUS "SHARE_LIBRARIES = ${SHARE_LIBRARIES}")
		endif (NOT SHARE_FIND_QUIETLY)
	else (SHARE_FOUND)
		if (SHARE_FIND_REQUIRED)
			message (FATAL_ERROR "Could not find SHARE!")
		endif (SHARE_FIND_REQUIRED)
	endif (SHARE_FOUND)
	
	#IF (WIN32)
	#	set (PLATFORM_LIBS ws2_32 ole32 oleaut32 Psapi Setupapi uuid ${SHARE_LIBRARIES} CACHE STRING     "SHARE_LIBRARIES" FORCE)
	#ELSEIF (UNIX)
	#	IF(QNX)
	#		SET(PLATFORM_LIBS socket m ${SHARE_LIBRARIES} CACHE STRING     "SHARE_LIBRARIES" FORCE)
	#	ELSE()
	#		SET(PLATFORM_LIBS  pthread dl rt ${SHARE_LIBRARIES} CACHE STRING     "SHARE_LIBRARIES" FORCE)
	#	ENDIF()
	#ENDIF()
	
	mark_as_advanced (
		SHARE_ROOT
		SHARE_INCLUDES
		SHARE_LIBRARIES
    )
endif (NOT SHARE_FOUND)