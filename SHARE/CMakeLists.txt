CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)



if(COMMAND cmake_policy)#i don't know what is it
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0005 OLD)
endif(COMMAND cmake_policy)

# default to Release build (it's what most people will use)
if (NOT CMAKE_BUILD_TYPE)
    set( CMAKE_BUILD_TYPE Release CACHE STRING "Sets the configuration to build (Release, Debug, etc...)")
endif()


################################################################################
# Start of main Project definitions
################################################################################
if(DEFINED PROJECT_NAME)
	PROJECT(${PROJECT_NAME}_SHARE_LIB C CXX)
else()
	PROJECT(SHARE_LIB C CXX)
endif()

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

INCLUDE(CheckIncludeFiles)
INCLUDE(FindPackageHandleStandardArgs)
INCLUDE(SHAREMacros)

SET(SHARE_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
SET(SHARE_DEPENDENCIES_PATH ${SHARE_ROOT_PATH}/dependencies CACHE PATH  "share dependencies")
list( APPEND CMAKE_FIND_ROOT_PATH ${SHARE_DEPENDENCIES_PATH} )

SET(CMAKE_MODULE_PATH "${SHARE_ROOT_PATH}/CMakeModules;${CMAKE_MODULE_PATH}")

SET(CMAKE_DEBUG_POSTFIX  "" CACHE STRING "add a postfix, usually d on windows")
SET(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")

SET(SHARE_LIBRARIES "" CACHE STRING "Link these to use SHARE")
SET(PLATFORM_LIBS ""  CACHE STRING "Platform library")
option(ENABLE_VTUNE "Enable Vtune profiling instrumentation" OFF)


mark_as_advanced (
	SHARE_LIBRARIES
	PLATFORM_LIBS
	SHARE_DEPENDENCIES_PATH
	CMAKE_DEBUG_POSTFIX
	CMAKE_RELEASE_POSTFIX
    )
	
################################################################################
# Create bin and lib directories if required
################################################################################
IF("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin ${CMAKE_BINARY_DIR}/lib)
ENDIF("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
MAKE_DIRECTORY(${OUTPUT_BINDIR})
SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT_BINDIR})

SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
MAKE_DIRECTORY(${OUTPUT_LIBDIR})
SET(LIBRARY_OUTPUT_PATH ${OUTPUT_LIBDIR})

LINK_DIRECTORIES( ${LINK_DIRECTORIES} ${OUTPUT_LIBDIR} )


SET(CMAKE_SHARE_INCLUDE_PATH ${CMAKE_SHARE_INCLUDE_PATH} CACHE STRING "You may add additional search paths here. Use ; to separate multiple paths.")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} CACHE STRING "You may add additional search paths here. Use ; to separate multiple paths.")

include (CMakeModules/configure_share.cmake)

################################################################################
#Look for packages
################################################################################

if(NOT DEFINED BOOST_ROOT)
	SET(BOOST_ROOT ${SHARE_DEPENDENCIES_PATH} CACHE PATH "boost path")
endif(NOT DEFINED BOOST_ROOT)
	
FIND_PACKAGE(Boost REQUIRED)

if(NOT Boost_FOUND)
	message(FATAL "Failed to find boost.")	
	return()
endif()
add_definitions(-DBOOST_ALL_NO_LIB)

FIND_PACKAGE(rapidjson REQUIRED)
if(NOT RAPIDJSON_FOUND)
	message(FATAL "Failed to find rapidjson.")	
	return()
endif()

FIND_PACKAGE(tclap REQUIRED)
if(NOT TCLAP_FOUND)
	message(FATAL "TCLAP (http://tclap.sourceforge.net/) could not be found. Set TCLAP_INCLUDE_PATH to point to the headers adding '-DTCLAP_INCLUDE_PATH=/path/to/tclap' to the cmake command.")	
	return()
endif()

# Windows specific config
IF (WIN32)
   if (MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4200") #disabling of 4200 warning
		add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE -DNOMINMAX)
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
		CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.conf.in 	${CMAKE_INSTALL_PREFIX}/doc/share/doxygen.conf)
		SET(DOXY_CONFIG "${CMAKE_INSTALL_PREFIX}/doc/share/doxygen.conf")
	else()
		MESSAGE(WARNING "WARNING: cannot find file:" "${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.conf.in")	
	ENDIF()
	
	ADD_CUSTOM_TARGET(doc_share_lib COMMAND ${DOXYGEN_EXECUTABLE}  ${DOXY_CONFIG} COMMENT "Generating documentation of UDT" VERBATIM)
else()
	MESSAGE(WARNING "WARNING: cannot create documentation as doxygen is not found.")	
endif()

