set(_OUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/${aTARGET_NAME})

set(_CONFIG_PLUGIN_DIR ${${PROJECT_NAME}_DEFAULT_PLUGIN_DIR})

configure_file(default_kernel_config.json.in
                   "${_OUT_DIRECTORY}/default_kernel_config.json" @ONLY)
unset(_CONFIG_PLUGIN_DIR)

install(FILES
        ${_OUT_DIRECTORY}/default_kernel_config.json
        DESTINATION
        ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}
        )
