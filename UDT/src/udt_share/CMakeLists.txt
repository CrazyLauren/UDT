set(_TARGET_NAME udt_share)

configure_target_version(${_TARGET_NAME}
		${PATH_TO_REVISION_IN}
		"${CMAKE_BINARY_DIR}/src/${_TARGET_NAME}/"
		)

file(GLOB _SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
		)
list (APPEND _SOURCE_FILES
		"${CMAKE_BINARY_DIR}/src/${_TARGET_NAME}/revision.c"
		)

file (GLOB _HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
		)

set (_PUBLIC_INCLUDE_DIR
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${SHARE_INCLUDES}
		)
set (_PRIVATE_INCLUDE_DIR
		${Boost_INCLUDE_DIRS}
		)
set (_PRIVATE_DEFINITIONS
		BOOST_ALL_NO_LIB
		)

set (_PUBLIC_DEFINITIONS
		#
		)

helper_add_library(${_TARGET_NAME}
		_SOURCE_FILES
		_HEADER_FILES
		_PUBLIC_INCLUDE_DIR
		_PRIVATE_INCLUDE_DIR
		_PUBLIC_DEFINITIONS
		_PRIVATE_DEFINITIONS
		TRUE)

set (_PUBLIC_LIBRARIS
		${SHARE_LIBRARIES}
		)

set (_PRIVATE_LIBRARIS
		#
		)

helper_target_link_libraries(${_TARGET_NAME}
		_PUBLIC_LIBRARIS
		_PRIVATE_LIBRARIS
		)

install(FILES 	include/udt/udt_types.h
				include/udt/udt_share_macros.h
        		include/udt/programm_id.h
				include/udt/IExtParser.h
				include/udt/CParserFactory.h
				include/udt/udt_rtc.h
				DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/udt
		)
helper_export_library(${_TARGET_NAME})

						  
