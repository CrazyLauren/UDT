#Set common info
macro(generate_cpack)
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
	
	if(DEFINED ${PROJECT_NAME}_VERSION)
		if("${${PROJECT_NAME}_REVISION_VERSION}" STREQUAL "")
			set(CPACK_PACKAGE_VERSION
					"${${PROJECT_NAME}_VERSION}"
				)
		else()
			set(CPACK_PACKAGE_VERSION
				"${${PROJECT_NAME}_VERSION}.${${PROJECT_NAME}_REVISION_VERSION}"
			)
		endif()			
	else()
		set(CPACK_PACKAGE_VERSION
			"${${PROJECT_NAME}_REVISION_VERSION}"
			)
	endif()			
	set(CPACK_PACKAGE_VERSION_PATH
		"${${PROJECT_NAME}_SOURCE_PATH}"
		)
	
	
	#Set directories
	set(CPACK_PACKAGE_INSTALL_DIRECTORY
		${PROJECT_NAME}
		)
	if(NOT "${CMAKE_INSTALL_PREFIX}" STREQUAL "")
		set(CPACK_OUTPUT_FILE_PREFIX
			${CMAKE_INSTALL_PREFIX}/package
			CACHE PATH
			"Path to save packages"
			FORCE
			)
	endif()
	
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
	set(_LICENSE_INSTALL_PATH "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/license")
	
	if (WIN32)
		set(_LICENSE_INSTALL_PATH_DEV "${_LICENSE_INSTALL_PATH}")
		set(_LICENSE_INSTALL_PATH_RUNTIME "${_LICENSE_INSTALL_PATH}")
	else()
		set(_LICENSE_INSTALL_PATH_DEV "${_LICENSE_INSTALL_PATH}/${PROJECT_NAME}-Development")
		set(_LICENSE_INSTALL_PATH_RUNTIME "${_LICENSE_INSTALL_PATH}/${PROJECT_NAME}-Runtime")
	endif ()
	
	install(FILES
			${${PROJECT_NAME}_LICENCE}
			DESTINATION
			"${_LICENSE_INSTALL_PATH_RUNTIME}"
			COMPONENT applications)
	install(FILES
			${${PROJECT_NAME}_LICENCE}
			DESTINATION
			"${_LICENSE_INSTALL_PATH_RUNTIME}"
			COMPONENT libraries)
	install(FILES
			${${PROJECT_NAME}_LICENCE}
			DESTINATION
			"${_LICENSE_INSTALL_PATH_RUNTIME}"
			COMPONENT config)
	
	install(FILES
			${${PROJECT_NAME}_LICENCE}
			DESTINATION
			"${_LICENSE_INSTALL_PATH_DEV}"
			COMPONENT cmake)
	install(FILES
			${${PROJECT_NAME}_LICENCE}
			DESTINATION
			"${_LICENSE_INSTALL_PATH_DEV}"
			COMPONENT headers)
	
	install(FILES
			${${PROJECT_NAME}_LICENCE}
			DESTINATION
			"${_LICENSE_INSTALL_PATH_DEV}"
			COMPONENT examples)
	install(FILES
			${${PROJECT_NAME}_LICENCE}
			DESTINATION
			"${_LICENSE_INSTALL_PATH_DEV}"
			COMPONENT tests)
	install(FILES
			${${PROJECT_NAME}_LICENCE}
			DESTINATION
			"${_LICENSE_INSTALL_PATH_DEV}"
			COMPONENT docs)
	
	set(CPACK_COMPONENT_GROUP_DEVELOPMENT_DESCRIPTION
		"All tools you'll ever need to develop"
		)
	set(CPACK_COMPONENT_GROUP_RUNTIME_DESCRIPTION
		"For user only"
	)
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
		"Development"
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
	set(CPACK_DEB_COMPONENT_INSTALL ON)
	set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS OFF)
	set(CPACK_DEBIAN_RUNTIME_PACKAGE_SHLIBDEPS ON)
	set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
	
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER
		${CPACK_PACKAGE_VENDOR}
		)
	
	string(TOLOWER "${PROJECT_NAME}" _PROJECT_NAME_LOWERCASE)
	set(CPACK_DEBIAN_DEVELOPMENT_PACKAGE_DEPENDS     ${_PROJECT_NAME_LOWERCASE}-runtime)
	set(CPACK_DEBIAN_RUNTIME_PACKAGE_SECTION "libs")
	set(CPACK_DEBIAN_DEVELOPMENT_PACKAGE_SECTION "devel")
	
	if(NOT "${CPACK_PACKAGE_VERSION}" STREQUAL "")
		set(CPACK_DEBIAN_PACKAGE_VERSION
			"${CPACK_PACKAGE_VERSION}"
			)
	else()
		set(CPACK_DEBIAN_PACKAGE_VERSION
			"0"
			)
	endif()
	
	#Set variables speceified to NSIS
	if(WIN32 AND NOT UNIX)
		set(CPACK_NSIS_DISPLAY_NAME
			"${PROJECT_NAME} rev. ${${PROJECT_NAME}_REVISION_VERSION}"
			)
		set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
		set(CPACK_COMPONENT_TESTS_DISABLED ON)
		set(CPACK_COMPONENT_HEADERS_DISABLED ON)
		set(CPACK_COMPONENT_EXAMPLES_DISABLED ON)
		set(CPACK_COMPONENT_DOCS_DISABLED ON)
		set(CPACK_COMPONENT_CMAKE_DISABLED ON)
		
		 
		set(CPACK_NSIS_MODIFY_PATH ON)
		set(CPACK_NSIS_CONTACT
			${CPACK_PACKAGE_CONTACT}
			)
		set(CPACK_NSIS_COMPRESSORCPACK_NSIS_PACKAGE_NAME
			"lzma"
			)
		#Create link to cmake directory
		set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS
			"CreateShortCut '$INSTDIR\\\\${CMAKE_INSTALL_BINDIR}\\\\${CMAKE_PACKAGE_INSTALL_DIR}.lnk' '$INSTDIR\\\\${CMAKE_PACKAGE_INSTALL_DIR}\\\\'"
	     )
		set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS
			"Delete '$INSTDIR\\\\${CMAKE_INSTALL_BINDIR}\\\\${CMAKE_PACKAGE_INSTALL_DIR}.lnk'"
			)
	
		list(APPEND CPACK_NSIS_CREATE_ICONS_EXTRA
			 "CreateShortCut '$DESKTOP\\\\${PROJECT_NAME}.lnk' '$INSTDIR\\\\'")
	
		list(APPEND CPACK_NSIS_DELETE_ICONS_EXTRA
			 "Delete '$DESKTOP\\\\${PROJECT_NAME}.lnk'")
	
		if (MINGW)
			get_filename_component(CXX_PATH ${CMAKE_CXX_COMPILER} DIRECTORY)
	
			set(_MINGW_RUNTIME_LIBS "")
	
			if (EXISTS ${CXX_PATH}/libatomic-1.dll)
				list(APPEND _MINGW_RUNTIME_LIBS ${CXX_PATH}/libatomic-1.dll)
			endif ()
			
			if (EXISTS ${CXX_PATH}/libgcc_s_dw2-1.dll)
				list(APPEND _MINGW_RUNTIME_LIBS ${CXX_PATH}/libgcc_s_dw2-1.dll)
			endif ()
	
			if (EXISTS ${CXX_PATH}/libgomp-1.dll)
				list(APPEND _MINGW_RUNTIME_LIBS ${CXX_PATH}/libgomp-1.dll)
			endif ()
			
			if (EXISTS ${CXX_PATH}/libquadmath-0.dll)
				list(APPEND _MINGW_RUNTIME_LIBS ${CXX_PATH}/libquadmath-0.dll)
			endif ()
	
			if (EXISTS ${CXX_PATH}/libssp-0.dll)
				list(APPEND _MINGW_RUNTIME_LIBS ${CXX_PATH}/libssp-0.dll)
			endif ()
			
			if (EXISTS ${CXX_PATH}/libstdc++-6.dll)
				list(APPEND _MINGW_RUNTIME_LIBS ${CXX_PATH}/libstdc++-6.dll)
			endif ()
	
			if (EXISTS ${CXX_PATH}/mingwm10.dll)
				list(APPEND _MINGW_RUNTIME_LIBS ${CXX_PATH}/mingwm10.dll)
			endif ()
	
			if (EXISTS ${CXX_PATH}/libwinpthread-1.dll)
				list(APPEND _MINGW_RUNTIME_LIBS ${CXX_PATH}/libwinpthread-1.dll)
			endif ()
	
			if (EXISTS ${CXX_PATH}/libgcc_s_seh-1.dll)
				list(APPEND _MINGW_RUNTIME_LIBS ${CXX_PATH}/libgcc_s_seh-1.dll)
			endif ()				
	
	
	
			
			if (_MINGW_RUNTIME_LIBS)
				install(FILES
						${_MINGW_RUNTIME_LIBS}
						DESTINATION
						"${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}"
						COMPONENT applications)
			endif ()
		endif()
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
					set(CPACK_GENERATOR "DEB")
				elseif(_DISTIBUTION MATCHES "RedHat.*")
					set(CPACK_GENERATOR "RPM")
				endif()
			else()
				set(CPACK_GENERATOR "TGZ")			
			endif()
		else()
			set(CPACK_GENERATOR "TGZ")		
		endif()
	endif()
	if(${PROJECT_NAME}_PYTHONS_MODULES)
		list(APPEND CPACK_COMPONENTS_ALL python_package)
		
		if(WIN32)
			set(_LICENSE_INSTALL_PATH_PYTHON "${_LICENSE_INSTALL_PATH}")
		else()
			set(_LICENSE_INSTALL_PATH_PYTHON "${_LICENSE_INSTALL_PATH}/${PROJECT_NAME}-Python")
		endif()
		
		install(FILES
				${${PROJECT_NAME}_LICENCE}
				DESTINATION
				"${_LICENSE_INSTALL_PATH_PYTHON}"
				COMPONENT python_package)
				
		set(CPACK_COMPONENT_GROUP_PYTHON_DESCRIPTION
			"Python modules"
		)
		set(CPACK_COMPONENT_PYTHON_PACKAGE_GROUP
			"Python"
		)
		set(CPACK_DEBIAN_PYTHON_PACKAGE_SECTION "python")
		if(Python3_VERSION_MAJOR)
			list(APPEND CPACK_DEBIAN_PYTHON_PACKAGE_DEPENDS "python${Python3_VERSION_MAJOR} (>= ${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR})")
		endif()											
	endif()	

	if(${PROJECT_NAME}_MATLAB_MODULES)
		list(APPEND CPACK_COMPONENTS_ALL matlab_package)

		if(WIN32)
			set(_LICENSE_INSTALL_PATH_MATLAB "${_LICENSE_INSTALL_PATH}")
		else()
			set(_LICENSE_INSTALL_PATH_MATLAB "${_LICENSE_INSTALL_PATH}/${PROJECT_NAME}-Matlab")
		endif()

		install(FILES
		        ${${PROJECT_NAME}_LICENCE}
		        DESTINATION
		        "${_LICENSE_INSTALL_PATH_MATLAB}"
		        COMPONENT matlab_package)

		set(CPACK_COMPONENT_GROUP_MATLAB_DESCRIPTION
		    "Matlab modules"
		    )
		set(CPACK_COMPONENT_MATLAB_PACKAGE_GROUP
		    "Matlab"
		    )
		set(CPACK_DEBIAN_MATLAB_PACKAGE_SECTION "matlab")

		list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS
		     "
SearchPath $0 matlab.exe
\\\${If} \\\${FileExists} \\\$0
	ExecWait '\\\"\\\$0\\\" -wait -nodisplay -nodesktop -r \\\"try, matlab.addons.toolbox.installToolbox(\$\\\\'$INSTDIR\\\\${CMAKE_INSTALL_DATAROOTDIR}\\\\${PROJECT_NAME}\\\\package\\\\${${PROJECT_NAME}_MTBLX_FILE_NAME}\$\\\\'); catch ME, warning(getReport(ME)); quit(1); end; quit\\\"'
	DetailPrint \\\"Matlab return \\\$0 \\\"
\\\${Else}
      MessageBox MB_OK|MB_ICONEXCLAMATION \\\
      \\\"No Matlab installation has been found. (Install Matlab Package manually)\\\"
\\\${Endif}
"
		     )
		list(APPEND CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS
		     "
SearchPath $0 matlab.exe
\\\${If} \\\${FileExists} \\\$0
	ExecWait '\\\"\\\$0\\\" -wait -nodisplay -nodesktop -r \\\"try, tbxs = matlab.addons.toolbox.installedToolboxes; for i = 1:length(tbxs), if strcmp(lower(tbxs(i).Name),lower(\$\\\\'${${PROJECT_NAME}_MATLAB_PROJECT_NAME}\$\\\\')), matlab.addons.toolbox.uninstallToolbox(tbxs(i)); end, end, catch ME, warning(getReport(ME)); quit(1); end; quit\\\"'
	DetailPrint \\\"Matlab return \\\$0 \\\"
\\\${Else}
      MessageBox MB_OK|MB_ICONEXCLAMATION \\\
      \\\"No Matlab installation has been found. (Uninstall Matlab Package manually)\\\"
\\\${Endif}
"
		     )
	endif()

	if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../${PROJECT_NAME}_CPack.cmake")
		include("${CMAKE_CURRENT_LIST_DIR}/../${PROJECT_NAME}_CPack.cmake")
	endif()

	if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(_SYSTEM_PACK "win64")
		else()
			set(_SYSTEM_PACK "win32")
		endif()
	else()
		set(_SYSTEM_PACK "${CMAKE_SYSTEM_NAME}")
	endif()

	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${_SYSTEM_PACK}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}")
	include(CPack)
endmacro()
