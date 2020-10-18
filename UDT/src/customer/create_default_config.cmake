set(_OUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/${_TARGET_NAME})
set(_CONFIG_PLUGIN_DIR ${${PROJECT_NAME}_DEFAULT_CUSTOMER_PLUGIN_DIR})

set(_LIBRARIES "")

if(NOT ${_TARGET_NAME_UP}_WITH_STATIC_MODULES)
	foreach(_VAR ${CUSTOMER_AVAILABLE_MODULES})
		
		if(_LIBRARIES)
	  		set(_LIBRARIES "${_LIBRARIES},\n				")
		endif()
		
	  	set(_LIBRARIES "${_LIBRARIES}\"${_VAR}\":{}")
	endforeach()
else()
	foreach(_VAR ${${_TARGET_NAME_UP}_AVAILABLE_MODULES})
		list (FIND ${_TARGET_NAME_UP}_LIST_STATIC_MODULES ${_VAR} _index)
		if (${_index} EQUAL -1)
			if(_LIBRARIES)
				set(_LIBRARIES "${_LIBRARIES},\n				")
			endif()

			set(_LIBRARIES "${_LIBRARIES}\"${_VAR}\":{}")
		endif()
	endforeach()
endif()

configure_file(default_customer_config.json.in
               "${_OUT_DIRECTORY}/default_customer_config.json" @ONLY)

unset(_CONFIG_PLUGIN_DIR)
unset(_LIBRARIES)

install(FILES ${_OUT_DIRECTORY}/default_customer_config.json
        DESTINATION
        "${UDT_INSTALL_CONFIG_DEFAULT_PATH}" COMPONENT config)
