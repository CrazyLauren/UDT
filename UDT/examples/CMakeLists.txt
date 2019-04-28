PROJECT(${PROJECT_NAME}_examples C CXX)	

add_subdirectory(example_customer)

add_subdirectory(example_customer_for_user_protocol)
add_subdirectory(example_parser)

add_subdirectory(example_customer_receiver)
add_subdirectory(example_customer_sender)
add_subdirectory(example_customer_sniffer)
#add_subdirectory(example_customer_user_protocol)

#add_subdirectory(example_customer3)		
#add_subdirectory(example_customer4)
#add_subdirectory(example_parser_error)		
#add_subdirectory(example_parser_error2)
#add_subdirectory(example_customer_error)
#add_subdirectory(example_customer_version_error)
#add_subdirectory(example_customer_user_error)
add_subdirectory(example_customer_swap_endian)


					