include(FindPackageHandleStandardArgs)
find_package(Doxygen)
if(DOXYGEN_FOUND)	
	
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.conf.in")		
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.conf.in 	
		${CMAKE_BINARY_DIR}/doc/${PROJECT_NAME}/doxygen.conf)
		set(DOXY_CONFIG "${CMAKE_BINARY_DIR}/doc/${PROJECT_NAME}/doxygen.conf")
	else()
		message(WARNING "WARNING: cannot find file:" "${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.conf.in")	
	endif()
	
	add_custom_target(doc_${PROJECT_NAME} COMMAND ${DOXYGEN_EXECUTABLE}  ${DOXY_CONFIG} COMMENT "Generating documentation of UDT" VERBATIM)
else()
	message(WARNING "WARNING: cannot create documentation as doxygen is not found.")	
endif()