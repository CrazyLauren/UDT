#
# share_function.cmake
#
# Copyright © 2016  https://github.com/CrazyLauren
#
#  Created on: 12.11.2016
#      Author:  https://github.com/CrazyLauren
#
# Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
# https://www.mozilla.org/en-US/MPL/2.0)



# Configuration logging
#
#	aTARGET_NAME - Name of the TARGET
function(configure_logging aTARGET_NAME)

	string(TOUPPER ${aTARGET_NAME}
			_TARGET_UP
			)
	option(${_TARGET_UP}_LOGGING_TO_GLOG "Using glog for loging" OFF)
	option(${_TARGET_UP}_LOGGING_TO_LOG4CPLUS "Using log4cplus for loging" ON)
	option(${_TARGET_UP}_LOGGING_TO_COUT "Logging to cout" OFF)
	option(${_TARGET_UP}_LOGGING_HAS_TO_BE_REMOVED "Removed logging" OFF)
	option(${_TARGET_UP}_LOGGING_IS_DISABLED_IN "Disable logging in ${aTARGET_NAME} library" OFF)
	
	set(${_TARGET_UP}_LOGGING_COMMAND_FLAG "v" CACHE STRING  "")
	set(${_TARGET_UP}_LOGGING_COMMAND_LONG_NAME "verbose" CACHE STRING  "")

	set(USE_GLOG 0 CACHE INTERNAL "" FORCE)
	set(USE_CPLUS 0 CACHE INTERNAL "" FORCE)
	

	if(NOT ${_TARGET_UP}_LOGGING_HAS_TO_BE_REMOVED)
		if (${_TARGET_UP}_LOGGING_TO_GLOG)
			if(NOT GLOG_FOUND)
				message(FATAL_ERROR "Failed to find GLOG.")	
				return()
			endif()

			target_compile_definitions(${aTARGET_NAME}
										PUBLIC GLOG)
			
			target_link_libraries(${aTARGET_NAME}
										PUBLIC ${GLOG_LIBRARIES})
			target_include_directories( ${aTARGET_NAME} PRIVATE
										${GLOG_INCLUDE_DIRS}
										)
			set(USE_GLOG 1 CACHE INTERNAL "" FORCE)		
		elseif (${_TARGET_UP}_LOGGING_TO_LOG4CPLUS)
			if(NOT LOG4CPLUS_FOUND)
				message(FATAL_ERROR "Failed to find log4cplus.")	
				return()
			endif()
		
			target_compile_definitions(${aTARGET_NAME}
										PUBLIC CPLUS_LOG)

			if(NOT MSVC_VERSION)

				get_property(_IS_STATIC TARGET log4cplus
						 PROPERTY STATIC_LIBRARY
						 )
				#if(${_IS_STATIC})
					target_link_libraries(${aTARGET_NAME}
										  PRIVATE log4cplus)

					get_property(LOG4CPLUS_INCLUDES TARGET log4cplus
								 PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
					foreach(_INCL ${LOG4CPLUS_INCLUDES})
						target_include_directories (${aTARGET_NAME}
													PUBLIC ${_INCL}
													)
					endforeach()
				#[[else()
					target_link_libraries(${aTARGET_NAME}
										  PUBLIC log4cplus)
				endif()]]
			else()
				target_link_libraries(${aTARGET_NAME}
									  PUBLIC log4cplus)
				foreach(_INCL ${LOG4CPLUS_INCLUDES})
					target_include_directories (${aTARGET_NAME}
												PUBLIC $<BUILD_INTERFACE:${_INCL}>
												)
				endforeach()
			endif()


			set(USE_CPLUS 1 CACHE INTERNAL "" FORCE)
		elseif (${_TARGET_UP}_LOGGING_TO_COUT)
			target_compile_definitions(${aTARGET_NAME}
									   PUBLIC OUT_LOG)
		else()
			target_compile_definitions(${aTARGET_NAME}
									   PUBLIC NOLOG)
		endif ()
		
		if (${_TARGET_UP}_LOGGING_IS_DISABLED_IN)
			target_compile_definitions(${aTARGET_NAME}
										PRIVATE NOLOG)
		endif()
	else()
		target_compile_definitions(${aTARGET_NAME}
										PRIVATE REMOVE_LOG)
	endif()
	
	set(${_TARGET_UP}_LOGGING_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../CMakeModules/share_trace_config.h.cmake" CACHE PATH "")
	
	if(NOT EXISTS ${${_TARGET_UP}_LOGGING_CONFIG_FILE})
		message(FATAL_ERROR "Cannot find file share_trace_config.h.cmake")
	endif()
	
	configure_file(${${_TARGET_UP}_LOGGING_CONFIG_FILE}
				${CMAKE_BINARY_DIR}/include/${PROJECT_NAME}/logging/share_trace_config.h
				ESCAPE_QUOTES
				)
	install(FILES ${CMAKE_BINARY_DIR}/include/${PROJECT_NAME}/logging/share_trace_config.h
				DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}include/${PROJECT_NAME}/logging/" COMPONENT headers
		)			
endfunction()
