CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

SET(CMAKE_MODULE_PATH "${SHARE_ROOT_PATH}/CMakeModules;${CMAKE_MODULE_PATH}")
INCLUDE(FindPackageHandleStandardArgs)

cmake_policy(SET CMP0022 NEW)
# default to Release build (it's what most people will use)
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Sets the configuration to build (Release, Debug, etc...)")
endif()


################################################################################
# Start of main Project definitions
################################################################################
if(DEFINED PROJECT_NAME)
	PROJECT(${PROJECT_NAME}_UDT C CXX)
else()
	PROJECT(UDT C CXX)
endif()

SET(KERNEL_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
SET(KERNEL_SOURCE_PATH ${KERNEL_ROOT_PATH}/src)
SET(KERNEL_DEPENDENCIES_PATH ${KERNEL_ROOT_PATH}/dependencies ${SHARE_DEPENDENCIES_PATH} CACHE PATH "share dependencies")

list( APPEND CMAKE_FIND_ROOT_PATH ${KERNEL_DEPENDENCIES_PATH})

SET(CMAKE_MODULE_PATH "${KERNEL_ROOT_PATH}/CMakeModules; ${CMAKE_MODULE_PATH}")

SET(CMAKE_DEBUG_POSTFIX  "d" CACHE STRING "add a postfix, usually d on windows")
SET(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")


SET(PLATFORM_LIBS ""  CACHE STRING "Platform library")

mark_as_advanced (
	PLATFORM_LIBS
	KERNEL_DEPENDENCIES_PATH
	CMAKE_DEBUG_POSTFIX
	CMAKE_RELEASE_POSTFIX
    )
	
################################################################################
# Create bin and lib directories if required
################################################################################
IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin ${CMAKE_BINARY_DIR}/lib)
ENDIF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
MAKE_DIRECTORY(${OUTPUT_BINDIR})
SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT_BINDIR})

SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
MAKE_DIRECTORY(${OUTPUT_LIBDIR})
SET(LIBRARY_OUTPUT_PATH ${OUTPUT_LIBDIR})

LINK_DIRECTORIES( ${LINK_DIRECTORIES} ${OUTPUT_LIBDIR} )


SET(CMAKE_KERNEL_INCLUDE_PATH ${CMAKE_KERNEL_INCLUDE_PATH} CACHE STRING "You may add additional search paths here. Use ; to separate multiple paths.")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} CACHE STRING "You may add additional search paths here. Use ; to separate multiple paths.")

################################################################################
#Look for packages
################################################################################
FIND_PACKAGE(SHARE REQUIRED)

if(NOT SHARE_FOUND)
	message(FATAL "Failed to find share.")	
	return()
endif()

if(NOT DEFINED BOOST_ROOT)
	SET(BOOST_ROOT ${KERNEL_DEPENDENCIES_PATH})
endif(NOT DEFINED BOOST_ROOT)
	
FIND_PACKAGE(Boost REQUIRED)

if(NOT Boost_FOUND)
	message(FATAL "Failed to find boost.")	
	return()
endif()

FIND_PACKAGE(tclap REQUIRED)
if(NOT TCLAP_FOUND)
	message(FATAL "TCLAP (http://tclap.sourceforge.net/) could not be found. Set TCLAP_INCLUDE_PATH to point to the headers adding '-DTCLAP_INCLUDE_PATH=/path/to/tclap' to the cmake command.")	
	return()
endif()


configure_logging(${KERNEL_DEPENDENCIES_PATH})


# Windows specific config
IF (WIN32)
   if (MSVC)
		add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE -DNOMINMAX )
        # This option is to enable the /MP switch for Visual Studio 2005 and above compilers
        OPTION(BUILD_BY_MP "Set to ON to build SHARE with the /MP option (Visual Studio 2005 and above)." OFF)
        MARK_AS_ADVANCED(BUILD_BY_MP)
        IF(BUILD_BY_MP)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
        ENDIF(BUILD_BY_MP)

    endif(MSVC)
ENDIF()



################################################################################
# descend into subdirs
################################################################################
add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(examples)
add_subdirectory(tests)

FIND_PACKAGE(Doxygen)
if(DOXYGEN_FOUND)	
	
	IF   (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.conf.in")		
		CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.conf.in 	${CMAKE_BINARY_DIR}/doc/udt/doxygen.conf)
		SET(DOXY_CONFIG "${CMAKE_BINARY_DIR}/doc/udt/doxygen.conf")
	else()
		MESSAGE(WARNING "WARNING: cannot find file:" "${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.conf.in")	
	ENDIF()
	
	ADD_CUSTOM_TARGET(doc_udt COMMAND ${DOXYGEN_EXECUTABLE}  ${DOXY_CONFIG} COMMENT "Generating documentation of SHARE" VERBATIM)
else()
	MESSAGE(WARNING "WARNING: cannot create documentation as doxygen is not found.")	
endif()
