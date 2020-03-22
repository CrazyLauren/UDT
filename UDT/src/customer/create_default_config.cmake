
#[[	foreach(_source
            ${${_TARGET_NAME_UP}_LIST_STATIC_MODULES}
            )
#[=[        list(APPEND _PRIVATE_LIBRARIS
                "${_source}_Static"
                )
        string(TOUPPER	${_source}
                _LIB_NAME_UP
                )
        list(APPEND DEF_MODULES
                ${_LIB_NAME_UP}_STATIC
                )]=]
    endforeach()]]

   #[[ foreach(_source
            ${CUSTOMER_AVAILABLE_MODULES}
            )

    endforeach()
]]
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/default_customer_config.json
        DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}" COMPONENT config)
