set(_TARGET_NAME customer)

string(TOUPPER ${_TARGET_NAME}
		_TARGET_NAME_UP
		)

set(CUSTOMER_SWIG_OUTPUT_PATH "${CMAKE_BINARY_DIR}/src/${_TARGET_NAME}/swig")

file(GLOB _SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
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
		TRUE
		CACHE BOOL
		"Build ${_TARGET_NAME} with 'static loaded modules' "
		)

#adding object files of modules to project
set(${_TARGET_NAME_UP}_LIST_STATIC_MODULES		
		"sm_main_channel;tcp_client_io_manager;rtc_for_modeling"
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

			
set(${_TARGET_NAME_UP}_C_MAX_STRING_LEN
			"64"
			CACHE STRING  "Max string len for C API"
			)
set(${_TARGET_NAME_UP}_C_MAX_MESSAGE_HEADER_SIZE
			${${PROJECT_NAME}_MAX_MESSAGE_HEADER_SIZE}
			CACHE STRING  "Max message header size for C API"
			FORCE)
set(${_TARGET_NAME_UP}_C_MAX_UUID_TO_SIZE 
			"32"
			CACHE STRING  "Max number of uuids for C API"
			)
set(${_TARGET_NAME_UP}_C_MAX_PROGRAM_ID
			"64"
			CACHE STRING  "Max nubmer of programs in net for C API"
			)
set(${_TARGET_NAME_UP}_C_MAX_MESSAGE_SUBSCRIBERS
			"32"
			CACHE STRING  "Max nubmer of subcriber of program for C API"
			)
set(${_TARGET_NAME_UP}_C_MAX_RTC_NUMBER
			"32"
			CACHE STRING  "Max nubmer of RTC C API"
			)														
configure_file(include/customer_config.h.in
		${CMAKE_BINARY_DIR}/include/UDT/config/customer/customer_config.h
		ESCAPE_QUOTES
		)
list (APPEND _HEADER_FILES
		"${CMAKE_BINARY_DIR}/include/UDT/config/customer/customer_config.h"
		)

configure_file(include/receive_data_info_c.h.in
               ${CMAKE_BINARY_DIR}/include/UDT/customer/receive_data_info_c.h
               ESCAPE_QUOTES
               )
#configure_file(include/customer_c.h.in
#               ${CMAKE_BINARY_DIR}/include/UDT/customer_c.h
#               ESCAPE_QUOTES
#               )




set(_STATIC_LOAD "")
set(_EXTERN_FUNCTION "")               
if(${_TARGET_NAME_UP}_WITH_STATIC_MODULES)
	foreach(_VAR ${${_TARGET_NAME_UP}_LIST_STATIC_MODULES})	
		set(_STATIC_LOAD "${_STATIC_LOAD}\n {module_t _mod; _mod.FName = \"${_VAR}\"; _mod.FRegister = get_factory_registry_${_VAR}(); FModules.push_back(_mod); }")
		set(_EXTERN_FUNCTION "${_EXTERN_FUNCTION}\n extern  \"C\" NSHARE::factory_registry_t* get_factory_registry_${_VAR}();")
	endforeach()
endif()
configure_file(include/static_modules.h.in
               ${CMAKE_BINARY_DIR}/include/UDT/static_modules.h)                              
list (APPEND _HEADER_FILES
		"${CMAKE_BINARY_DIR}/include/UDT/static_modules.h"
		)                              


helper_add_library(${_TARGET_NAME}
		_SOURCE_FILES
		_HEADER_FILES
		_PUBLIC_INCLUDE_DIR
		_PRIVATE_INCLUDE_DIR
		_PUBLIC_DEFINITIONS
		_PRIVATE_DEFINITIONS
		TRUE)
configure_version_for(${_TARGET_NAME})

helper_target_link_libraries(${_TARGET_NAME}
		_PUBLIC_LIBRARIS
		_PRIVATE_LIBRARIS
		)

install(FILES 	${CMAKE_CURRENT_SOURCE_DIR}/include/UDT/CCustomer.h
				${CMAKE_CURRENT_SOURCE_DIR}/include/UDT/customer_export.h
				${CMAKE_CURRENT_SOURCE_DIR}/include/UDT/IRtc.h
				${CMAKE_CURRENT_SOURCE_DIR}/include/UDT/IRtcControl.h
                ${CMAKE_CURRENT_SOURCE_DIR}/include/UDT/customer_c.h                                
		DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}include/UDT/" COMPONENT headers)
install(FILES 	${CMAKE_CURRENT_SOURCE_DIR}/include/UDT/customer/customer_c_type.h				          
                ${CMAKE_BINARY_DIR}/include/UDT/customer/receive_data_info_c.h
		DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}include/UDT/customer/" COMPONENT headers)

		
install(FILES
		${CMAKE_CURRENT_SOURCE_DIR}/include/UDT/impl/customer_covert_impl.h
		DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}include/UDT/impl" COMPONENT headers)
install(FILES
		${CMAKE_CURRENT_SOURCE_DIR}/include/UDT/impl/customer_covert_impl_matlab.h
		DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}include/UDT/impl" COMPONENT headers)

		
install(FILES
		${CMAKE_CURRENT_SOURCE_DIR}/include/customer.h
		DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}include/" COMPONENT headers)

helper_export_library(${_TARGET_NAME})

set(CUSTOMER_AVAILABLE_MODULES
		"" CACHE STRING
		"list of available modules"
		FORCE
		)

#build modules
add_subdirectory(modules)
add_subdirectory(swig)
add_subdirectory(matlab)

include(create_default_config.cmake)
