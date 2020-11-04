set(_TARGET_NAME customer_matlab)

configure_target_version(${_TARGET_NAME}
		${PATH_TO_REVISION_IN}
		"${CMAKE_BINARY_DIR}/src/${_TARGET_NAME}/"
		)


file (GLOB _SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/*.c
		)
list (APPEND _SOURCE_FILES
		"${CMAKE_BINARY_DIR}/src/${_TARGET_NAME}/revision.c"
		)
				
file (GLOB _HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
		)
		
set (_PRIVATE_INCLUDE_DIR
		${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_BINARY_DIR}/include
		)

set (_PUBLIC_INCLUDE_DIR
        #
		)
set (_PRIVATE_DEFINITIONS
		#
		)
set (_PUBLIC_DEFINITIONS
		#
		)

set(_PUBLIC_LIBRARIS
        customer
    )
set(_OUTPUT_NAME customer)


helper_add_matlab_library(${_TARGET_NAME}
                        _SOURCE_FILES
                        _HEADER_FILES
    		            _PUBLIC_INCLUDE_DIR
                        _PRIVATE_INCLUDE_DIR
                        _PUBLIC_DEFINITIONS
                        _PRIVATE_DEFINITIONS
                        _PUBLIC_LIBRARIS
                        ${_OUTPUT_NAME}
                        "${_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}/+udt"
                        )

