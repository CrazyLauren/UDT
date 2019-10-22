set(_TARGET_NAME customer)

string(TOUPPER ${_TARGET_NAME}
		_TARGET_NAME_UP
		)

configure_target_version(${_TARGET_NAME}
		${PATH_TO_REVISION_IN}
		"${CMAKE_BINARY_DIR}/src/${_TARGET_NAME}/"
		)

file(GLOB _SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
		)
list (APPEND _SOURCE_FILES
		"${CMAKE_BINARY_DIR}/src/${_TARGET_NAME}/revision.c"
		)

file (GLOB _HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
		)

set (_PUBLIC_INCLUDE_DIR
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${SHARE_INCLUDES}
		)
set (_PRIVATE_INCLUDE_DIR
		${Boost_INCLUDE_DIRS}
		${CMAKE_BINARY_DIR}/include/UDT/
		)
set (_PRIVATE_DEFINITIONS
		BOOST_ALL_NO_LIB
		)

set (_PUBLIC_DEFINITIONS
		#
		)



set(${_TARGET_NAME_UP}_WITH_STATIC_MODULES
		true
		CACHE BOOL
		"Build ${_TARGET_NAME} with 'static loaded modules' "
		)

#adding object files of modules to project
set(${_TARGET_NAME_UP}_LIST_STATIC_MODULES
		#"sm_main_channel;tcp_client_io_manager"
		""
		CACHE STRING
		"list of modules which will linked static"

		)

set (_PUBLIC_LIBRARIS
		${UDT_SHARE_LIBRARIES}
		)

set (_PRIVATE_LIBRARIS
		#
		)

if(${${_TARGET_NAME_UP}_WITH_STATIC_MODULES})

	foreach(_source
			${${_TARGET_NAME_UP}_LIST_STATIC_MODULES}
			)
			list(APPEND _PRIVATE_LIBRARIS
					"${_source}_Static"
					)
			string(TOUPPER	${_source}
					_LIB_NAME_UP
					)
			list(APPEND DEF_MODULES
					${_LIB_NAME_UP}_STATIC
					)
	endforeach()

	list(APPEND _PRIVATE_DEFINITIONS
			DEF_MODULES
			)
endif()
configure_file(include/customer_config.h.in
		${CMAKE_BINARY_DIR}/include/UDT/config/customer/customer_config.h
		ESCAPE_QUOTES
		)
list (APPEND _HEADER_FILES
		"${CMAKE_BINARY_DIR}/include/UDT/config/customer/customer_config.h"
		)
helper_add_library(${_TARGET_NAME}
		_SOURCE_FILES
		_HEADER_FILES
		_PUBLIC_INCLUDE_DIR
		_PRIVATE_INCLUDE_DIR
		_PUBLIC_DEFINITIONS
		_PRIVATE_DEFINITIONS
		TRUE)


helper_target_link_libraries(${_TARGET_NAME}
		_PUBLIC_LIBRARIS
		_PRIVATE_LIBRARIS
		)

install(FILES 	${CMAKE_CURRENT_SOURCE_DIR}/include/udt/CCustomer.h
				${CMAKE_CURRENT_SOURCE_DIR}/include/udt/customer_export.h
				${CMAKE_CURRENT_SOURCE_DIR}/include/udt/IRtc.h
				${CMAKE_CURRENT_SOURCE_DIR}/include/udt/IRtcControl.h
		DESTINATION ${CMAKE_INSTALL_PREFIX}/include/udt/)

install(FILES
		${CMAKE_CURRENT_SOURCE_DIR}/include/customer.h
		DESTINATION ${CMAKE_INSTALL_PREFIX}/include/)

helper_export_library(${_TARGET_NAME})

set(${_TARGET_NAME}_AVAILABLE_MODULES
		"" CACHE STRING
		"list of available modules"
		FORCE
		)

#build modules
add_subdirectory(modules)

include(create_default_config.cmake)