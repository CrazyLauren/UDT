CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

INCLUDE(FindPackageHandleStandardArgs)

if(COMMAND cmake_policy)#i don't know what is it
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0005 OLD)
endif(COMMAND cmake_policy)

# default to Release build (it's what most people will use, except cegui devs)
if (NOT CMAKE_BUILD_TYPE)
    set( CMAKE_BUILD_TYPE Release CACHE STRING "Sets the configuration to build (Release, Debug, etc...)")
endif()


################################################################################
# Start of main Project definitions
################################################################################
PROJECT(UDT_SHARE C CXX)


SET(SHARE_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
SET(SHARE_SOURCE_PATH ${SHARE_ROOT_PATH}/src)
SET(SHARE_INCLUDE_PATH ${SHARE_ROOT_PATH}/include)
SET(SHARE_DEPENDENCIES_PATH ${SHARE_ROOT_PATH}/dependencies)

SET(CMAKE_MODULE_PATH "${SHARE_ROOT_PATH}/CMakeModules;${CMAKE_MODULE_PATH}")

SET(CMAKE_DEBUG_POSTFIX  "d" CACHE STRING "add a postfix, usually d on windows")
SET(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")

SET(TARGET_NAME SHARE)
SET(SHARE_TARGET_NAME ${TARGET_NAME})

# Dynamic vs Static Linking
OPTION(DYNAMIC_SHARE "Set to ON to build  for dynamic linking.  Use OFF for static." ON)
IF   (DYNAMIC_SHARE)
    SET(USER_DEFINED_DYNAMIC_OR_STATIC "SHARED")
ELSE (DYNAMIC_SHARE)
    SET(USER_DEFINED_DYNAMIC_OR_STATIC "STATIC")
ENDIF(DYNAMIC_SHARE)

# Make a version file containing the current version from git.

SET(SHARE_MAJOR_VERSION "0")
SET(SHARE_MINOR_VERSION "1")
SET(SHARE_REVISION_VERSION "0")
OPTION(EMBED_GIT_SHA "Embeds the GIT SHA in the version code" ON)
IF (EMBED_GIT_SHA)
	include(GetGitRevisionDescription)
	git_describe(VERSION --tags --dirty=-d)

	#parse the version information into pieces.
	STRING(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" SHARE_MAJOR_VERSION "${VERSION}")
	STRING(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" SHARE_MINOR_VERSION "${VERSION}")
	STRING(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" SHARE_PATH "${VERSION}")
	STRING(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" SHARE_REVISION_VERSION "${VERSION}")
	SET(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
	message ("Version " ${VERSION_SHORT})
ELSE ()
	SET(SHARE_PATH "no path")
ENDIF (EMBED_GIT_SHA)

STRING(TIMESTAMP SHARE_TIME "%H:%M:%S" )
STRING(TIMESTAMP SHARE_DATA "%Y-%m-%d" )

SET(SHARE_VERSION ${SHARE_MAJOR_VERSION}.${SHARE_MINOR_VERSION}.${SHARE_REVISION_VERSION})



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


SET(CMAKE_SHARE_INCLUDE_PATH ${CMAKE_SHARE_INCLUDE_PATH} CACHE STRING "You may add additional search paths here. Use ; to separate multiple paths.")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} CACHE STRING "You may add additional search paths here. Use ; to separate multiple paths.")

################################################################################
#Look for packages
################################################################################
SET(BOOST_ROOT ${SHARE_DEPENDENCIES_PATH}/boost_1_47_0)
FIND_PACKAGE(Boost REQUIRED)
if(NOT Boost_FOUND)
	message(FATAL "Failed to find boost.")	
	return()
endif()

SET(RAPIDJSON_ROOT_DIR ${SHARE_DEPENDENCIES_PATH}/rapidjson)
FIND_PACKAGE(rapidjson REQUIRED)
if(NOT RAPIDJSON_FOUND)
	message(FATAL "Failed to find rapidjson.")	
	return()
endif()

SET(TCLAP_ROOT ${SHARE_DEPENDENCIES_PATH}/tclap-1.2.1)
FIND_PACKAGE(tclap REQUIRED)
if(NOT TCLAP_FOUND)
	message(FATAL "TCLAP (http://tclap.sourceforge.net/) could not be found. Set TCLAP_INCLUDE_PATH to point to the headers adding '-DTCLAP_INCLUDE_PATH=/path/to/tclap' to the cmake command.")	
	return()
endif()


SET(GLOG_ROOT_DIR ${SHARE_DEPENDENCIES_PATH}/glog)
FIND_PACKAGE(glog)

SET(LOG4CPLUS_ROOT_DIR ${SHARE_DEPENDENCIES_PATH}/log4cplus)
FIND_PACKAGE(log4cplus)

OPTION(LOGGING_TO_GLOG "Using glog for loging" OFF)
OPTION(LOGGING_TO_LOG4CPLUS "Using log4cplus for loging" OFF)
OPTION(LOGGING_TO_COUT "Logging to cout" OFF)
OPTION(LOGGING_DISABLED "Disable logging" ON)
OPTION(NO_STATRUP_TEST "Disable  start up tests " ON)
OPTION(SHARE_DEX_SOCKET "Adding socket for dex" OFF)


IF (LOGGING_TO_GLOG)
	if(NOT GLOG_FOUND)
		message(FATAL "Failed to find GLOG.")	
		return()
	endif()
	add_definitions(-DGLOG)
	include_directories(${GLOG_INCLUDE_DIRS})
	target_link_libraries(${TARGET_NAME} GLOG_LIBRARIES)
	SET(${PLATFORM_LIBS} GLOG_LIBRARIES)
ELSEIF (LOGGING_TO_LOG4CPLUS)
	if(NOT LOG4CPLUS_FOUND)
		message(FATAL "Failed to find log4cplus.")	
		return()
	endif()
	add_definitions(-DCPLUS_LOG)
	include_directories(${LOG4CPLUS_INCLUDES})
	target_link_libraries(${TARGET_NAME} LOG4CPLUS_LIBRARIES)
	SET(${PLATFORM_LIBS} LOG4CPLUS_LIBRARIES)
ELSEIF (LOGGING_TO_COUT)
add_definitions(-DCOUT_LOG)
ENDIF ()

IF (LOGGING_DISABLED)
	add_definitions(-DNOLOG)
ENDIF ()

IF (NO_STATRUP_TEST)
	add_definitions(-DNO_SHARE_TEST)
ENDIF ()

IF (NOT SHARE_DEX_SOCKET)
	add_definitions(-DNODEX)
	#todo
ENDIF ()

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
