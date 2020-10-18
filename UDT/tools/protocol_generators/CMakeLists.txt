install(DIRECTORY
			${CMAKE_CURRENT_SOURCE_DIR}/css
			DESTINATION 
			"${_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}/protocol_generators"
			COMPONENT applications)
install(DIRECTORY
			${CMAKE_CURRENT_SOURCE_DIR}/fonts
			DESTINATION 
			"${_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}/protocol_generators"
			COMPONENT applications)
install(DIRECTORY
			${CMAKE_CURRENT_SOURCE_DIR}/js
			DESTINATION 
			"${_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}/protocol_generators"
			COMPONENT applications)
install(DIRECTORY
			${CMAKE_CURRENT_SOURCE_DIR}/py
			DESTINATION 
			"${_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}/protocol_generators"
			COMPONENT applications)
install(DIRECTORY
			${CMAKE_CURRENT_SOURCE_DIR}/schemas
			DESTINATION 
			"${_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}/protocol_generators"
			COMPONENT applications)
install(DIRECTORY
			${CMAKE_CURRENT_SOURCE_DIR}/templates
			DESTINATION 
			"${_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}/protocol_generators"
			COMPONENT applications)	
install(FILES
			${CMAKE_CURRENT_SOURCE_DIR}/gen_protocol.py
			${CMAKE_CURRENT_SOURCE_DIR}/index.html
			${CMAKE_CURRENT_SOURCE_DIR}/EasyProtocol.json
			DESTINATION 
			"${_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}/protocol_generators"
			COMPONENT applications)																	