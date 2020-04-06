set(_OUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/${_TARGET_NAME})
set(_CONFIG_PLUGIN_DIR ${${PROJECT_NAME}_DEFAULT_CUSTOMER_PLUGIN_DIR})

configure_file(default_customer_config.json.in
               "${_OUT_DIRECTORY}/default_customer_config.json" @ONLY)

unset(_CONFIG_PLUGIN_DIR)

install(FILES ${_OUT_DIRECTORY}/default_customer_config.json
        DESTINATION
        "${UDT_INSTALL_CONFIG_DEFAULT_PATH}" COMPONENT config)
