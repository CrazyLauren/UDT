set (_TARGET_NAME Kernel)

configure_target_version(${_TARGET_NAME}
		${PATH_TO_REVISION_IN}
		"${CMAKE_BINARY_DIR}/src/${_TARGET_NAME}/"
		)
file(GLOB_RECURSE _SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/*.c
		)
list (APPEND _SOURCE_FILES    "${CMAKE_BINARY_DIR}/src/${_TARGET_NAME}/revision.c")

file (GLOB_RECURSE _HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
		)

set (_INCLUDE_DIR
		${CMAKE_CURRENT_SOURCE_DIR}
		${Boost_INCLUDE_DIRS}
		${TCLAP_INCLUDES}
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
		${UDT_SHARE_LIBRARIES}
		)

helper_target_link_libraries(${_TARGET_NAME}
		_PUBLIC_LIBRARIS
		_PRIVATE_LIBRARIS
		)


install(DIRECTORY
			${CMAKE_CURRENT_SOURCE_DIR}/gui/
		DESTINATION
			"${UDT_INSTALL_GUI_PATH}/gui" COMPONENT applications
		)
include(create_default_config.cmake)
