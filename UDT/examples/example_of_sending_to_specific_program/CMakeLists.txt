set (_TARGET_NAME example_of_sending_to_specific_program)

file (GLOB _SOURCE_FILES *.cpp)
file (GLOB _HEADER_FILES *.hpp)

set (_INCLUDE_DIR
		${CMAKE_CURRENT_SOURCE_DIR}
		${Boost_INCLUDE_DIRS}
		${EXAMPLE_PARSER_INCLUDES}
		)

set (_PRIVATE_DEFINITIONS
		#
		)

helper_add_executable(${_TARGET_NAME}
		_SOURCE_FILES
		_HEADER_FILES
		_INCLUDE_DIR
		_PRIVATE_DEFINITIONS
		true
		)
set (_PUBLIC_LIBRARIS
		#
		)

set (_PRIVATE_LIBRARIS
		${CUSTOMER_LIBRARIES}
		)

helper_target_link_libraries(${_TARGET_NAME}
		_PUBLIC_LIBRARIS
		_PRIVATE_LIBRARIS
		)

