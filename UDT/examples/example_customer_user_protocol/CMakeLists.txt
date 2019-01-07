SET(TARGET_NAME example_customer_user_protocol)

add_definitions(${LOGGING_DEFENITIONS})

file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file (GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

include_directories (
					${CMAKE_CURRENT_SOURCE_DIR} 
					${SHARE_INCLUDES} 
					${Boost_INCLUDE_DIRS} 
					${CUSTOMER_INCLUDES} 
					${LOGGING_INCLUDES}
					${EXAMPLE_PARSER_INCLUDES}
					${UDT_SHARE_INCLUDES})

share_add_executable(${TARGET_NAME} SOURCE_FILES HEADER_FILES TRUE)
share_target_link_libraries(${TARGET_NAME} ${PLATFORM_LIBS} )
share_add_dependency(${TARGET_NAME} ${CUSTOMER_LIBRARIES})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/example_customer2.xml 
		DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)						  