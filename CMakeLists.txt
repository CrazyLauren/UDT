CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)
cmake_policy(SET CMP0022 NEW)

if(DEFINED PROJECT_NAME)
	PROJECT(${PROJECT_NAME}_UDT_NET C CXX)
else()
	PROJECT(UDT_NET C CXX)
endif()

#add_definitions(-DBOOST_ALL_NO_LIB)

SET(LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE true CACHE BOOL "Build log4cplus from source")

if(LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE)
	if((NOT LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE_OK) AND  LOG4CPLUS_INCLUDES)
		message(WARNING "Log4cplus library is exist. See  ${LOG4CPLUS_INCLUDES}")
		if(NOT LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE_FORCE)
			option(LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE_FORCE OFF "If On checking for exist log4cplus is ignored ")
			message(FATAL_ERROR "Cannot continue build log4cplus. If You insist on continuing, Set up option LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE_FORCE as true")
			return()
		endif()
	endif()
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	set(BUILD_SHARED_LIBS FALSE CACHE BOOL "If TRUE, log4cplus is built as a shared library, otherwise as a static library")
	option(LOG4CPLUS_ENABLE_DECORATED_LIBRARY_NAME  "Turns on resulting file name decoration for static and UNICODE builds." OFF)
	
	add_subdirectory(log4cplus-1.2.x)
	
	SET(LOG4CPLUS_LIBRARIES  log4cplus CACHE STRING "CUSTOMER_LIBRARIES" FORCE)#fixme must be filepath
	SET(LOG4CPLUS_INCLUDES ${log4cplus_SOURCE_DIR}/include; 
                     ${log4cplus_BINARY_DIR}/include CACHE INTERNAL  "Include path for the header files of log4cplus" )
	SET(LOG4CPLUS_FOUND true CACHE INTERNAL "" FORCE)

	mark_as_advanced (
		LOG4CPLUS_LIBRARIES
		LOG4CPLUS_INCLUDES
    )
	set(LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE_OK true CACHE INTERNAL "" FORCE)
endif()	

add_subdirectory(SHARE)
add_subdirectory(UDT)
