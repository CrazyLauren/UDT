#
# helper_cmake_function.cmake
#
# Copyright © 2016  https://github.com/CrazyLauren
#
#  Created on: 12.11.2016
#      Author:  https://github.com/CrazyLauren
#
# Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
# https://www.mozilla.org/en-US/MPL/2.0)
# 
# @todo check change of  changelog.txt
# version 0.1 First release
# version 0.2 Upper case target bug fix

# Function for setting up base directory
function(install_default_directory 
			aPROJECT_NAME #	aPROJECT_NAME - Name of the project
			)

	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		set(CMAKE_INSTALL_PREFIX 
			"${CMAKE_BINARY_DIR}/binary" 
			CACHE PATH "Default installation Directory" FORCE)
	endif()
	
	set(${aPROJECT_NAME}_ROOT_DIR 
		${CMAKE_CURRENT_SOURCE_DIR} 
		CACHE PATH  "root directory")	
	
	set(${aPROJECT_NAME}_DEPENDENCIES_PATH 
		${${aPROJECT_NAME}_ROOT_DIR}/dependencies 
		CACHE PATH  "share dependencies")
		
	
	if(NOT CMAKE_DEBUG_POSTFIX)
		set(CMAKE_DEBUG_POSTFIX  
			"" 
			CACHE STRING "add a postfix, usually d on windows")
	endif()
	
	if(NOT CMAKE_RELEASE_POSTFIX)
		set(CMAKE_RELEASE_POSTFIX 
			"" 
			CACHE STRING "add a postfix, usually empty on windows")
	endif()
	
	set(${aPROJECT_NAME}_LIBRARIES 
		"" 
		CACHE STRING "Link these to use")
	

		
	set(CMAKE_INCLUDE_PATH 
		${CMAKE_INCLUDE_PATH} 
		CACHE STRING 
			"You may add additional search paths here.  
			Use ; to separate multiple paths.")
			
	set(CMAKE_LIBRARY_PATH 
		${CMAKE_LIBRARY_PATH} 
		CACHE STRING 
			"You may add additional search paths here. 
			Use ; to separate multiple paths.")
	
	list (FIND CMAKE_FIND_ROOT_PATH ${${aPROJECT_NAME}_DEPENDENCIES_PATH} _INDEX)
	if( (NOT DEFINED CMAKE_FIND_ROOT_PATH)#
		OR (_INDEX LESS 0)
		)
		set(CMAKE_FIND_ROOT_PATH 
			${CMAKE_FIND_ROOT_PATH} 
			${${aPROJECT_NAME}_DEPENDENCIES_PATH}
			CACHE STRING 
			"You may add additional search paths here. 
			Use ; to separate multiple paths."
			FORCE )
	endif()
	

	if(NOT DEFINED CMAKE_PACKAGE_INSTALL_DIR)
		set(CMAKE_PACKAGE_INSTALL_DIR 
			"${CMAKE_INSTALL_PREFIX}/cmake"
			CACHE PATH "The directory relative to CMAKE_PREFIX_PATH 
			where cmake   is installed"		
			)
	endif()
	mark_as_advanced (
		${aPROJECT_NAME}_LIBRARIES	
		${aPROJECT_NAME}_DEPENDENCIES_PATH
		CMAKE_DEBUG_POSTFIX
		CMAKE_RELEASE_POSTFIX
		CMAKE_INCLUDE_PATH
		CMAKE_LIBRARY_PATH
		CMAKE_FIND_ROOT_PATH
		CMAKE_PACKAGE_INSTALL_DIR
    )
	set(CMAKE_MODULE_PATH 
		"${CMAKE_MODULE_PATH}"
		"${CMAKE_MODULE_PATH}/find"
		CACHE STRING "Path to search modules"
		FORCE
		)	
endfunction()

#read version from file
function(read_version 
			aTARGET #target name
			aCANGELOG_PATH #Path to change log
			)

	string(TOUPPER ${aTARGET}
			_TARGET_UP
			)
	set(_FILE_VERSION "${aCANGELOG_PATH}/ChangeLog.txt")
	file(STRINGS ${_FILE_VERSION} _CHANGELOG_FILE)

	if(_CHANGELOG_FILE)
		set(_VERSION_REGEX "${aTARGET}[ \t]+version:[ \t]*[0-9]+[.][0-9]+")
	
		string(REGEX MATCHALL ${_VERSION_REGEX} _VERSION_STRING "${_CHANGELOG_FILE}")
	
		string(REGEX MATCHALL "[0-9]+[.][0-9]+" _VERSION_INFO "${_VERSION_STRING}")
	
		if(_VERSION_INFO)		
			list(GET _VERSION_INFO 0 _MAX_VERSION)
	
			foreach(_VERSION ${_VERSION_INFO})
				if(${_VERSION} VERSION_GREATER ${_MAX_VERSION}) 		
					set(_MAX_VERSION ${_VERSION})
				endif()
			endforeach()
	
			string(REGEX MATCHALL "^[0-9]+" _VERSION_MAJOR "${_MAX_VERSION}")	
			string(REGEX MATCHALL "[0-9]+$" _VERSION_MINOR "${_MAX_VERSION}")		

			set(${_TARGET_UP}_TARGET_VERSION_MAJOR ${_VERSION_MAJOR} CACHE STRING "" FORCE)
			set(${_TARGET_UP}_TARGET_VERSION_MINOR ${_VERSION_MINOR} CACHE STRING "" FORCE)
			
			set(${_TARGET_UP}_TARGET_VERSION_DESCRIPTION ${_CHANGELOG_FILE} CACHE STRING "" FORCE)

			message (STATUS "Latest Version of " ${aTARGET} " is " ${${_TARGET_UP}_TARGET_VERSION_MAJOR} "." ${${_TARGET_UP}_TARGET_VERSION_MINOR} " in ${aCANGELOG_PATH}/ChangeLog.txt")
					
		else()
			message (FATAL_ERROR "No version in ${aCANGELOG_PATH}/ChangeLog.txt file of " ${_TARGET_UP} " ." "Version fromat is:" "${_TARGET_UP} version:0.0")
		endif()
	else()
		message (FATAL_ERROR "ChangeLog file for " ${aTARGET} " is no exist. Path: " ${aCANGELOG_PATH})
	endif()
endfunction()

# Make a version
#	aTARGET - Name of the TARGET
#	aFILE_PATH - Path to revision.c.in
#	aOUT_PATH - Where to store revision.c
#	aMAJOR,_MINOR - Version of target
#	_SOVERSION - For library only, version in lib.so._SOVERSION - API Version

function(configure_version aTARGET aFILE_PATH aOUT_PATH aMAJOR aMINOR )

	string(TOUPPER ${aTARGET}
			_TARGET_UP
			)
	if (("${ARGC}" GREATER 5) AND "${ARGV6}")
		set(_SOVERSION ${ARGV6})
	endif()
	
	# Make a version file containing the current version from git.
	set(${_TARGET_UP}_TARGET_REVISION_VERSION "0" CACHE STRING "" FORCE)
	set(${_TARGET_UP}_TARGET_PATH "no path" CACHE STRING "" FORCE)

	option(${_TARGET_UP}_TARGET_EMBED_GIT_SHA "Embeds the GIT SHA in the version code" ON)
	
	mark_as_advanced(${_TARGET_UP}_TARGET_EMBED_GIT_SHA
					${_TARGET_UP}_TARGET_REVISION_VERSION
					${_TARGET_UP}_TARGET_PATH)
	
	if (${_TARGET_UP}_EMBED_GIT_SHA)
		include(GetGitRevisionDescription)
		git_describe(VERSION --always)

		#parse the version information into pieces.
		if(NOT VERSION  MATCHES  "-NOTFOUND")
			string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" ${_TARGET_UP}_TARGET_PATH "${VERSION}")
			string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" ${_TARGET_UP}_TARGET_REVISION_VERSION "${VERSION}")
		else(NOT VERSION  MATCHES  "-NOTFOUND")
			message("Cannot get revision from git:" "${VERSION}" ) 
		endif()		
	endif()

	string(TIMESTAMP ${_TARGET_UP}_TIME "%H:%M:%S" )
	string(TIMESTAMP ${_TARGET_UP}_DATA "%Y-%m-%d" )

	set(${_TARGET_UP}_TARGET_VERSION
		${aMAJOR}.${aMINOR}.${${_TARGET_UP}_TARGET_REVISION_VERSION}
		CACHE INTERNAL "" FORCE
		)
	
	if(_SOVERSION)
		set(${_TARGET_UP}_TARGET_SOVERSION
				${_SOVERSION} 
				CACHE INTERNAL "" FORCE)
	endif()
	
	message (STATUS "Version of " ${aTARGET} " is " ${${_TARGET_UP}_TARGET_VERSION} " from " ${${_TARGET_UP}_TARGET_PATH})

	set(CONF_VERSION_MAJOR 
		${aMAJOR} 
		CACHE INTERNAL "" FORCE
		)
	set(CONF_VERSION_MINOR 
		${aMINOR} 
		CACHE INTERNAL "" FORCE
		)
	set(CONF_VERSION_REVISION 
		${${_TARGET_UP}_TARGET_REVISION_VERSION}
		CACHE INTERNAL "" FORCE
		)
	set(CONF_VERSION_PATH
		${${_TARGET_UP}_TARGET_PATH}
		CACHE INTERNAL "" FORCE
		)
	set(CONF_VERSION_TIME
		${${_TARGET_UP}_TIME}
		CACHE INTERNAL "" FORCE
		)
	set(CONF_VERSION_DATA
		${${_TARGET_UP}_DATA}
		CACHE INTERNAL "" FORCE
		)
	
	set(CONF_VERSION_DESCRIPTION
		""
		CACHE INTERNAL "" FORCE
		)
			
	string(REGEX REPLACE "\"" "'" CONF_VERSION_DESCRIPTION ${${_TARGET_UP}_TARGET_VERSION_DESCRIPTION})


	set(CONF_TARGET_NAME
			${aTARGET}
			CACHE INTERNAL "" FORCE
			)

	configure_file( ${aFILE_PATH}/revision.c.in
                ${aOUT_PATH}/revision.c)
	
	set_source_files_properties(${aOUT_PATH}/revision.c PROPERTIES GENERATED TRUE)
	
	unset(CONF_VERSION_MAJOR CACHE)
	unset(CONF_VERSION_MINOR  CACHE)
	unset(CONF_VERSION_REVISION CACHE)
	unset(CONF_VERSION_PATH CACHE)
	unset(CONF_VERSION_TIME  CACHE)
	unset(CONF_VERSION_DATA  CACHE)
	unset(CONF_TARGET_NAME  CACHE)
	unset(CONF_VERSION_DESCRIPTION  CACHE)
endfunction()

# Make a version
#	aTARGET - Name of the TARGET
#	aFILE_PATH - Path to revision.c.in
#	aOUT_PATH - Path to revision.c
#	aCANGELOG_PATH - Path to changelog.txt
function(configure_target_version aTARGET aFILE_PATH aOUT_PATH)

	string(TOUPPER ${aTARGET}
			_TARGET_UP
			)

	if (("${ARGC}" GREATER 2) AND "${ARGV3}")
		set(_CANGELOG_PATH ${ARGV3})
	else()
		set(_CANGELOG_PATH ${CMAKE_CURRENT_SOURCE_DIR})
	endif()
	
	read_version(${aTARGET}
				${_CANGELOG_PATH}
				)
				
	configure_version(${aTARGET}
						${aFILE_PATH} 
						${aOUT_PATH} 
						${${_TARGET_UP}_TARGET_VERSION_MAJOR}
						${${_TARGET_UP}_TARGET_VERSION_MINOR}
						)
endfunction()


# Define _LIB_NAME library to be built 
#       _SOURCE_FILES_VAR2  
#       aHEADER_FILES_VAR   - the header file names.
#       aINSTALL_BIN        - TRUE if the lib should be installed
function (helper_add_library 
				aTARGET_NAME # - Name of the library to create.
				aSOURCE_FILES_VAR #  - the source file names.
				aHEADER_FILES_VAR # - the header file names.
				aPUBLIC_DIR # - public include dir
				aPRIVATE_DIR # - private include dir
				aPUBLIC_DEFINITIONS # - public DEFINITIONS
				aPRIVATE_DEFINITIONS # - private DEFINITIONS
				aINSTALL_BIN #- TRUE if the lib should be installed
		)


	string(TOUPPER ${aTARGET_NAME} 
			_TARGET_NAME_UPPER
		)
	
	set(${_TARGET_NAME_UPPER}_BUILD_STATIC_TOO
		false 
		CACHE BOOL "Build ${aTARGET_NAME} as static library too"
		)
	set(${_TARGET_NAME_UPPER}_WITH_STATIC_DEPENDENCIES
		false 
		CACHE BOOL "Link ${aTARGET_NAME} with static dependecies"
		)

	if(NOT ${_TARGET_NAME_UPPER}_NO_DYNAMIC)
		add_library(${aTARGET_NAME}
					SHARED
					${${aSOURCE_FILES_VAR}}
					${${aHEADER_FILES_VAR}}
					)
	
		add_library(${${PROJECT_NAME}_NAMESPACE}::${aTARGET_NAME}
					ALIAS
					${aTARGET_NAME})
	
		target_compile_definitions(${aTARGET_NAME}
							PRIVATE ${_TARGET_NAME_UPPER}_EXPORTS
							PRIVATE ${${aPRIVATE_DEFINITIONS}}
							
							PUBLIC ${${PROJECT_NAME}_PLATFORM_DEFENITIONS}
							PUBLIC ${${aPUBLIC_DEFINITIONS}}
							)
	
		target_include_directories (${aTARGET_NAME}
								PUBLIC ${${aPUBLIC_DIR}}
								)
		target_include_directories (${aTARGET_NAME}
								PRIVATE ${${aPRIVATE_DIR}}
								)

		if(DEFINED ${_TARGET_NAME_UPPER}_TARGET_SOVERSION AND DEFINED ${_TARGET_NAME_UPPER}_TARGET_VERSION)
			set_target_properties(${aTARGET_NAME} PROPERTIES
					VERSION ${${_TARGET_NAME_UPPER}_TARGET_VERSION}
					SOVERSION ${${_TARGET_NAME_UPPER}_TARGET_SOVERSION}
					)
		elseif (DEFINED ${_TARGET_NAME_UPPER}_TARGET_VERSION_MAJOR AND DEFINED ${_TARGET_NAME_UPPER}_TARGET_VERSION)
			set_target_properties(${aTARGET_NAME} PROPERTIES
					VERSION ${${_TARGET_NAME_UPPER}_TARGET_VERSION}
					SOVERSION ${${_TARGET_NAME_UPPER}_TARGET_VERSION_MAJOR}
					)
		else()
			message(STATUS "${aTARGET_NAME}. Version is not set")
		endif()

	endif()

    if (${${_TARGET_NAME_UPPER}_BUILD_STATIC_TOO})
    
        add_library(${aTARGET_NAME}_Static 
        			STATIC 
        			${${aSOURCE_FILES_VAR}}
        			${${aHEADER_FILES_VAR}}
        			)

		add_library(${${PROJECT_NAME}_NAMESPACE}::${aTARGET_NAME}_Static
				ALIAS
				${aTARGET_NAME}_Static)
				
		target_compile_definitions(${aTARGET_NAME}_Static 
									PUBLIC ${_TARGET_NAME_UPPER}_STATIC
									PRIVATE ${${aPRIVATE_DEFINITIONS}}
									
									PUBLIC ${${PROJECT_NAME}_PLATFORM_DEFENITIONS}
									PUBLIC ${${aPUBLIC_DEFINITIONS}}
							)
		
		target_include_directories (${aTARGET_NAME}_Static  
								PUBLIC ${${aPUBLIC_DIR}}
								)
		target_include_directories (${aTARGET_NAME}_Static  
								PRIVATE ${${aPRIVATE_DIR}}
								)			
    endif()	

    #INSTALLATION
    if (${aINSTALL_BIN})

		if(TARGET ${aTARGET_NAME} )
		install(TARGETS ${aTARGET_NAME} 
						LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
						ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
                        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
		endif()

        if (TARGET ${${aTARGET_NAME}_Static})
            install(TARGETS ${aTARGET_NAME}_Static
   						LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
						ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
                        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
        endif()
    endif()
	#${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)/${aTARGET_NAME}    		
	set(${_TARGET_NAME_UPPER}_LIBRARIES
			${aTARGET_NAME}
			CACHE STRING "${aTARGET_NAME} libraries" FORCE)
	

	set(${_TARGET_NAME_UPPER}_INCLUDES
			${${aPUBLIC_DIR}}
			CACHE PATH  "Include path for the header files of ${aTARGET_NAME}" FORCE)
	
	set(${_TARGET_NAME_UPPER}_FOUND TRUE CACHE BOOL  "${aTARGET_NAME} found" FORCE)
	
	mark_as_advanced (
	${_TARGET_NAME_UPPER}_LIBRARIES
    ${_TARGET_NAME_UPPER}_INCLUDES
	#${aTARGET_NAME}_DEPENDENCIES_PATH
    )

endfunction()

# Add libs to a target
function (helper_target_link_libraries
			 aTARGET_NAME #Target name
			 aPUBLIC_LIBRARIES # Libraries for with target and target which is link with target
			 aPRIVATE_LIBRARIES # Libraries only for with target
			 )

	string(TOUPPER ${aTARGET_NAME}
			_TARGET_NAME_UPPER
			)

	foreach(_LIB ${${aPUBLIC_LIBRARIES}})
		if(TARGET ${_LIB})
			set(_LIBRARY_NAME ${_LIB}$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>)
		else()
			set(_LIBRARY_NAME ${_LIB})
		endif()
		
		
		if(${${_TARGET_NAME_UPPER}_WITH_STATIC_DEPENDENCIES})
			if (TARGET ${_LIB}_Static)
				set(_LIBRARY_NAME ${_LIB}_Static$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>)	
			elseif(TARGET ${_LIB})
				set(${_LIB}_BUILD_STATIC_TOO 
						true 
						CACHE BOOL "Build ${_LIB} as static library too"
						FORCE
					)
				message(FATAL_ERROR "For ${aTARGET_NAME} requirement static library ${_LIB}. Please reconfigure project")
			endif()
		endif()

		if(TARGET ${aTARGET_NAME})
			target_link_libraries(${aTARGET_NAME}  PUBLIC ${_LIBRARY_NAME})
		endif()
    	if (TARGET ${aTARGET_NAME}_Static)        	
        	target_link_libraries(${aTARGET_NAME}_Static PUBLIC ${_LIBRARY_NAME})
    	endif()    	
    endforeach()

	foreach(_LIB ${${aPRIVATE_LIBRARIES}})
		if(TARGET ${_LIB})
			set(_LIBRARY_NAME ${_LIB}$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>)
		else()
			set(_LIBRARY_NAME ${_LIB})
		endif()
				
		if(${${_TARGET_NAME_UPPER}_WITH_STATIC_DEPENDENCIES})
			if (TARGET ${_LIB}_Static)
				set(_LIBRARY_NAME ${_LIB}_Static$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>)	
			elseif(TARGET ${_LIB})
				set(${_LIB}_BUILD_STATIC_TOO 
						true 
						CACHE BOOL "Build ${_LIB} as static library too"
						FORCE
					)
				message(FATAL_ERROR "For ${aTARGET_NAME} requirement static library ${_LIB}. Please reconfigure project")
			endif()
		endif()

		if(TARGET ${aTARGET_NAME})
			target_link_libraries(${aTARGET_NAME}  PRIVATE ${_LIBRARY_NAME})
		endif()
    	if (TARGET ${aTARGET_NAME}_Static)        	
        	target_link_libraries(${aTARGET_NAME}_Static PRIVATE ${_LIBRARY_NAME})
    	endif()    	
    endforeach()
    
endfunction ()

# Add executable
function (helper_add_executable 
			aTARGET_NAME # - Name of the library to create.
			aSOURCE_FILES_VAR #  - the source file names.
			aHEADER_FILES_VAR # - the header file names.
			aINCLUDE_DIR # - include dir			
			aDEFINITIONS # - defenitions	
			aINSTALL_BIN #- TRUE if the lib should be installed			
			)
	string(TOUPPER ${aTARGET_NAME}
			_TARGET_NAME_UPPER
			)
    #Statically Linked
	set(${_TARGET_NAME_UPPER}_WITH_STATIC_DEPENDENCIES false CACHE
	BOOL "Link with static dependecies")
	
    add_executable(${aTARGET_NAME} ${${aSOURCE_FILES_VAR}} ${${aHEADER_FILES_VAR}})

	target_include_directories (${aTARGET_NAME}
								PRIVATE ${${aINCLUDE_DIR}}
								)
	
	target_compile_definitions(${aTARGET_NAME}
							PRIVATE ${${aDEFINITIONS}})
    #Install
	if (${aINSTALL_BIN})
		install(TARGETS ${aTARGET_NAME} 
						LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
                        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
                        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

	endif()
endfunction()
