include(FindPackageHandleStandardArgs)
find_package(Doxygen)
if(DOXYGEN_FOUND)
	set(DOC_PATH ${CMAKE_BINARY_DIR}/doc/${PROJECT_NAME}/)

	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.conf.in")

		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.conf.in
					   ${CMAKE_BINARY_DIR}/doc/${PROJECT_NAME}/doxygen.conf
					   )
		set(DOXY_CONFIG "${CMAKE_BINARY_DIR}/doc/${PROJECT_NAME}/doxygen.conf")
	else()
		message(WARNING "WARNING: cannot find file:" "${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.conf.in")	
	endif()
	
	add_custom_target(doc_${PROJECT_NAME}
					  COMMAND ${DOXYGEN_EXECUTABLE}  ${DOXY_CONFIG}
					  COMMENT "Generating documentation of UDT"
					  VERBATIM
					  )

	set_property(DIRECTORY
				 APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
				 ${DOC_PATH}
				 )

	install(DIRECTORY
			${DOC_PATH}
			DESTINATION ${CMAKE_INSTALL_DOCDIR}/${PROJECT_NAME}
			COMPONENT docs)
else()
	message(WARNING "WARNING: cannot create documentation as doxygen is not found.")	
endif()