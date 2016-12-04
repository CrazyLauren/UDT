PROJECT(examples C CXX)	

add_subdirectory(example_parser)
add_subdirectory(example_customer)	
add_subdirectory(example_customer2)	

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/kernel.json 
		DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)						  