#Set common info
set(CPACK_PACKAGE_NAME
	${PROJECT_NAME}
	)
set(CPACK_PACKAGE_VENDOR
	${${PROJECT_NAME}_ORGANIZATION}
	CACHE STRING
	"Organization name"
	)
set(CPACK_PACKAGE_CONTACT
	${${PROJECT_NAME}_SUPPORT_ADDRESS}
	CACHE STRING
	"Contact information"
	)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
	"Development by ${${PROJECT_NAME}_AUTHORS}"
	CACHE STRING
	"Description"
	)
set(CPACK_RESOURCE_FILE_LICENSE
	${${PROJECT_NAME}_LICENCE}
	)

#Set version info

set(CPACK_PACKAGE_VERSION
	"${${PROJECT_NAME}_REVISION_VERSION}"
	)
set(CPACK_PACKAGE_VERSION_PATH
	"${${PROJECT_NAME}_SOURCE_PATH}"
	)


#Set directories
set(CPACK_PACKAGE_INSTALL_DIRECTORY
	${PROJECT_NAME}
	)
#if(NOT "${CMAKE_INSTALL_PREFIX}" STREQUAL "")
	set(CPACK_OUTPUT_FILE_PREFIX
		${${PROJECT_NAME}_INSTALL_PREFIX}/package
		CACHE PATH
		"Path to save packages"
		)
#endif()

if ("${CPACK_OUTPUT_FILE_PREFIX}" STREQUAL ""
		)
	set(CPACK_OUTPUT_FILE_PREFIX
		"./package"
		CACHE PATH
		"Path to save packages"
		FORCE
		)
endif()
mark_as_advanced (CPACK_OUTPUT_FILE_PREFIX
				  CPACK_PACKAGE_VENDOR
				  CPACK_PACKAGE_CONTACT
				  CPACK_PACKAGE_DESCRIPTION_SUMMARY
				  )

#set components
set(CPACK_COMPONENTS_ALL
	applications
	libraries
	cmake
	headers
	config
	examples
	tests
	docs
	)

set(CPACK_COMPONENT_GROUP_DEVELOPMENT_DESCRIPTION
	"All tools you'll ever need to develop"
	)
set(CPACK_COMPONENT_GROUP_RUNTIME_DESCRIPTION
	"For user only
")

set(CPACK_COMPONENT_APPLICATIONS_GROUP
	"Runtime"
	)
set(CPACK_COMPONENT_CONFIG_GROUP
	"Runtime"
	)
set(CPACK_COMPONENT_LIBRARIES_GROUP
	"Runtime"
	)
set(CPACK_COMPONENT_TESTS_GROUP
	"Runtime"
	)

set(CPACK_COMPONENT_CMAKE_GROUP
	"Development"
	)
set(CPACK_COMPONENT_HEADERS_GROUP
	"Development"
	)
set(CPACK_COMPONENT_EXAMPLES_GROUP
	"Development"
	)
set(CPACK_COMPONENT_DOCS_GROUP
	"Development"
	)

set(CPACK_COMPONENT_HEADERS_DEPENDS
	libraries
	)
set(CPACK_COMPONENT_APPLICATIONS_DEPENDS
	libraries
	config
	)
set(CPACK_COMPONENT_EXAMPLES_DEPENDS
	applications
	headers
	cmake
	)
set(CPACK_COMPONENT_TESTS_DEPENDS
	libraries
	)

#Set variables speceified to DEB
set(CPACK_DEB_PACKAGE_COMPONENT ON)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER
	${CPACK_PACKAGE_VENDOR}
	)
#Set variables speceified to NSIS
if(WIN32 AND NOT UNIX)
	set(CPACK_NSIS_DISPLAY_NAME
		"${PROJECT_NAME} rev. ${${PROJECT_NAME}_REVISION_VERSION}"
		)
	set(CPACK_NSIS_MODIFY_PATH ON)
	set(CPACK_NSIS_CONTACT
		${CPACK_PACKAGE_CONTACT}
		)
	set(CPACK_NSIS_COMPRESSOR
		"lzma"
		)
endif()

#set generators
if(NOT CPACK_GENERATOR)
	if(WIN32)
		set(CPACK_GENERATOR "NSIS")
	elseif(UNIX)
		find_program(LSB_RELEASE_EXEC lsb_release)
		if(EXISTS ${LSB_RELEASE_EXEC})
			execute_process(COMMAND
							${LSB_RELEASE_EXEC}
							-si
							OUTPUT_VARIABLE _DISTIBUTION
							OUTPUT_STRIP_TRAILING_WHITESPACE
							)
			if(_DISTIBUTION STREQUAL "Debian"
			   OR _DISTIBUTION STREQUAL "Ubuntu"
			   OR _DISTIBUTION MATCHES "AstraLinux.*"
			   )
				set(CPACK_GENERATOR "DEB;TGZ")
			elseif(_DISTIBUTION MATCHES "RedHat.*")
				set(CPACK_GENERATOR "RPM")
			endif()
		endif()
	endif()
endif()

include(CPack)
