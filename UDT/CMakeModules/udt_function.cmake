#
# udt_function.cmake
#
# Copyright © 2016  https://github.com/CrazyLauren
#
#  Created on: 12.11.2016
#      Author:  https://github.com/CrazyLauren
#
# Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
# https://www.mozilla.org/en-US/MPL/2.0)

#Define loadable module - this is loaded dynamically at runtime
function (add_loadable_module aTARGET_NAME)

	string(TOUPPER ${aTARGET_NAME}
			_TARGET_UP
			)

	set(CUSTOMER_AVAILABLE_MODULES
			${CUSTOMER_AVAILABLE_MODULES}
			${aTARGET_NAME}
			CACHE STRING "list of available modules"
			FORCE
			)


	configure_target_version(${aTARGET_NAME}
			${PATH_TO_REVISION_IN}
			"${CMAKE_BINARY_DIR}/src/${aTARGET_NAME}/"
			)

	file(GLOB _SOURCE_FILES
			${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
			)
	list (APPEND _SOURCE_FILES
		  ${CMAKE_BINARY_DIR}/src/${aTARGET_NAME}/revision.c
			)

	file (GLOB _HEADER_FILES
			${CMAKE_CURRENT_SOURCE_DIR}/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
			)

	set (_PUBLIC_INCLUDE_DIR
			${CMAKE_CURRENT_SOURCE_DIR}
			)
	set (_PRIVATE_INCLUDE_DIR
			${Boost_INCLUDE_DIRS}
            ${CMAKE_CURRENT_SOURCE_DIR}/../../include
            ${CMAKE_BINARY_DIR}/include/UDT/
			)

	set (_PRIVATE_DEFINITIONS
			BOOST_ALL_NO_LIB
			)

	set (_PUBLIC_DEFINITIONS
			#
			)

	set(_IS_OBJECT false)

	#create objects files for for customer
	if(${CUSTOMER_WITH_STATIC_MODULES})
		list (FIND CUSTOMER_LIST_STATIC_MODULES ${aTARGET_NAME} _index)
		if (${_index} GREATER -1)
			set(_IS_OBJECT true)
		endif()
	endif()

	if(_IS_OBJECT)
		set(${_TARGET_UP}_NO_DYNAMIC
				TRUE
				CACHE BOOL ""
				FORCE
				)

		set(${_TARGET_UP}_BUILD_STATIC_TOO
				TRUE
				CACHE BOOL "Build ${aTARGET_NAME} as static library too"
				FORCE
				)

		get_property(CUSTOMER_COMPILE_DEF TARGET ${CUSTOMER_LIBRARIES} PROPERTY COMPILE_DEFINITIONS)

		list(APPEND _PRIVATE_DEFINITIONS
				${_TARGET_UP}_STATIC
				${CUSTOMER_COMPILE_DEF}
				CUSTOMER_STATIC
				)

		list(APPEND _PRIVATE_INCLUDE_DIR
				${CUSTOMER_INCLUDES}
				${UDT_SHARE_INCLUDES}
				)
		helper_add_library(${aTARGET_NAME}
				_SOURCE_FILES
				_HEADER_FILES
				_PUBLIC_INCLUDE_DIR
				_PRIVATE_INCLUDE_DIR
				_PUBLIC_DEFINITIONS
				_PRIVATE_DEFINITIONS				
				FALSE)
		set (_PUBLIC_LIBRARIS
		     udt_share
		     )

		set (_PRIVATE_LIBRARIS
		     #
		     )
		helper_target_link_libraries(${aTARGET_NAME}_Static
		                             _PUBLIC_LIBRARIS
		                             _PRIVATE_LIBRARIS
		                             )
	else()
		set(${_TARGET_UP}_NO_DYNAMIC
				false
				CACHE BOOL ""
				FORCE
				)

		set(${_TARGET_UP}_BUILD_STATIC_TOO
				false
				CACHE BOOL "Build ${aTARGET_NAME} as static library too"
				FORCE
				)
		helper_add_library(${aTARGET_NAME}
				_SOURCE_FILES
				_HEADER_FILES
				_PUBLIC_INCLUDE_DIR
				_PRIVATE_INCLUDE_DIR
				_PRIVATE_DEFINITIONS
				_PUBLIC_DEFINITIONS
				FALSE)

		set (_PUBLIC_LIBRARIS
		     ${CUSTOMER_LIBRARIES}
		     )

		set (_PRIVATE_LIBRARIS
		     #
		     )

		helper_target_link_libraries(${aTARGET_NAME}
		                             _PUBLIC_LIBRARIS
		                             _PRIVATE_LIBRARIS
		                             )
		install(TARGETS ${aTARGET_NAME}
			LIBRARY DESTINATION ${${PROJECT_NAME}_INSTALL_CUSTOMER_PLUGIN_PATH} COMPONENT libraries
			ARCHIVE DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}" COMPONENT libraries
			RUNTIME DESTINATION ${${PROJECT_NAME}_INSTALL_CUSTOMER_PLUGIN_PATH} COMPONENT libraries
			)
	endif()



endfunction()
