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

	unset(CMAKE_INSTALL_FULL_BINDIR CACHE)
	unset(CMAKE_INSTALL_FULL_SBINDIR CACHE)
	unset(CMAKE_INSTALL_FULL_LIBEXECDIR CACHE)
	unset(CMAKE_INSTALL_FULL_SYSCONFDIR CACHE)
	unset(CMAKE_INSTALL_FULL_SHAREDSTATEDIR CACHE)
	unset(CMAKE_INSTALL_FULL_LOCALSTATEDIR CACHE)
	unset(CMAKE_INSTALL_FULL_LIBDIR CACHE)
	unset(CMAKE_INSTALL_FULL_INCLUDEDIR CACHE)
	unset(CMAKE_INSTALL_FULL_OLDINCLUDEDIR CACHE)
	unset(CMAKE_INSTALL_FULL_DATAROOTDIR CACHE)
	unset(CMAKE_INSTALL_FULL_DATADIR CACHE)
	unset(CMAKE_INSTALL_FULL_INFODIR CACHE)
	unset(CMAKE_INSTALL_FULL_LOCALEDIR CACHE)
	unset(CMAKE_INSTALL_FULL_MANDIR CACHE)
	unset(CMAKE_INSTALL_FULL_DOCDIR CACHE)

	include(GNUInstallDirs)

	set(CMAKE_INSTALL_FULL_BINDIR ${CMAKE_INSTALL_FULL_BINDIR} CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_SBINDIR ${CMAKE_INSTALL_FULL_SBINDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_LIBEXECDIR ${CMAKE_INSTALL_FULL_LIBEXECDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_SYSCONFDIR ${CMAKE_INSTALL_FULL_SYSCONFDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_SHAREDSTATEDIR ${CMAKE_INSTALL_FULL_SHAREDSTATEDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_LOCALSTATEDIR ${CMAKE_INSTALL_FULL_LOCALSTATEDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_LIBDIR ${CMAKE_INSTALL_FULL_LIBDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_INCLUDEDIR ${CMAKE_INSTALL_FULL_INCLUDEDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_OLDINCLUDEDIR ${CMAKE_INSTALL_FULL_OLDINCLUDEDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_DATAROOTDIR ${CMAKE_INSTALL_FULL_DATAROOTDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_DATADIR ${CMAKE_INSTALL_FULL_DATADIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_INFODIR ${CMAKE_INSTALL_FULL_INFODIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_LOCALEDIR ${CMAKE_INSTALL_FULL_LOCALEDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_MANDIR ${CMAKE_INSTALL_FULL_MANDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_DOCDIR ${CMAKE_INSTALL_FULL_DOCDIR}  CACHE PATH "The absolute path" FORCE)

	if(NOT DEFINED CMAKE_PACKAGE_INSTALL_DIR)
		if(WIN32)
			set(CMAKE_PACKAGE_INSTALL_DIR
				"cmake"
				CACHE PATH "The directory relative to CMAKE_PREFIX_PATH
				where cmake   is installed"
				)
		else()
			set(CMAKE_PACKAGE_INSTALL_DIR
				${CMAKE_INSTALL_LIBDIR}/cmake
				CACHE PATH "The directory relative to CMAKE_PREFIX_PATH
				where cmake   is installed"
				)
		endif()
	endif()

	if(NOT ${CMAKE_INSTALL_DOCDIR})
		set(CMAKE_INSTALL_DOCDIR
			"doc"
			CACHE PATH "The directory relative to CMAKE_PREFIX_PATH
			where documetation   is installed"
			FORCE
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
		CMAKE_INSTALL_DOCDIR
    )
	set(CMAKE_MODULE_PATH_HELPER
			"${CMAKE_MODULE_PATH}/functions/"
			CACHE INTERNAL "Path to search modules"
			FORCE
			)
	set(CMAKE_MODULE_PATH 
		"${CMAKE_MODULE_PATH}"
		"${CMAKE_MODULE_PATH}/find"
		"${CMAKE_MODULE_PATH}/functions"
		CACHE STRING "Path to search modules"
		FORCE
		)

	if ("${CMAKE_INSTALL_PREFIX}" STREQUAL ""
		)
		set(${aPROJECT_NAME}_INSTALL_PREFIX "" CACHE INTERNAL "" FORCE)
	else()
		set(${aPROJECT_NAME}_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/" CACHE INTERNAL "" FORCE)
	endif()

endfunction()

function(install_target_binary_impl
		 aTARGET_NAME # - Name of the library to create.
		 aIS_EXPORT
		 )
	get_target_property(_TYPE ${aTARGET_NAME} TYPE)

	set(_INSTALL_PREFIX	${${PROJECT_NAME}_INSTALL_PREFIX})
	
	if (DEFINED DIRECTORY_TYPE)

		set(_COMONENT_TYPE ${DIRECTORY_TYPE})

		if (NOT ${_COMONENT_TYPE} STREQUAL "tests" AND NOT ${_COMONENT_TYPE} STREQUAL "examples")
			message(FATAL_ERROR "Invalid type of component" ${_COMONENT_TYPE})
		endif ()
		

		if(aIS_EXPORT)
			install(TARGETS ${aTARGET_NAME}
				EXPORT ${aTARGET_NAME}-export
				LIBRARY DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}" COMPONENT ${_COMONENT_TYPE}
				ARCHIVE DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}" COMPONENT ${_COMONENT_TYPE}
				RUNTIME DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}" COMPONENT ${_COMONENT_TYPE}
				)
		else()
			install(TARGETS ${aTARGET_NAME}
					LIBRARY DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}" COMPONENT ${_COMONENT_TYPE}
					ARCHIVE DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}" COMPONENT ${_COMONENT_TYPE}
					RUNTIME DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}" COMPONENT ${_COMONENT_TYPE}
					)
		endif()
	else ()
		if("${_TYPE}" STREQUAL "EXECUTABLE")
			set(_COMP_LIB "applications")
			set(_COMP_RT "applications")
		else()
			set(_COMP_LIB "libraries")
			set(_COMP_RT "applications")
		endif()
		
		if(aIS_EXPORT)
			install(TARGETS ${aTARGET_NAME}
				EXPORT ${aTARGET_NAME}-export
				LIBRARY DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}" COMPONENT ${_COMP_LIB}
				ARCHIVE DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}"  COMPONENT ${_COMP_LIB}
				RUNTIME DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}" COMPONENT ${_COMP_RT}
				)
		else()
			install(TARGETS ${aTARGET_NAME}
					LIBRARY DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}" COMPONENT ${_COMP_LIB}
					ARCHIVE DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}" COMPONENT ${_COMP_LIB}
					RUNTIME DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}" COMPONENT ${_COMP_RT}
					)
		endif()
	endif ()

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

# Generating  revision number and path from GIT
function(set_revision_from_git aTARGET)

	string(TOUPPER ${aTARGET}
			_TARGET_UP
			)
	include(GetGitRevisionDescription)		
			
	git_describe(_TARGET_PATH --always --long)

	get_git_head_revision(GIT_REFSPEC GIT_SHA1)

	if(GIT_EXECUTABLE AND NOT DEFINED ${PROJECT_NAME}_REVISION_VERSION)
		execute_process(COMMAND git rev-list --count ${GIT_SHA1}
                   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    RESULT_VARIABLE GIT_error
                    ERROR_VARIABLE serial_error
                    OUTPUT_VARIABLE _TARGET_REVISION_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_STRIP_TRAILING_WHITESPACE
				)
		if(GIT_error EQUAL 0)
			set(${PROJECT_NAME}_SOURCE_PATH ${_TARGET_PATH} CACHE INTERNAL "")
			set(${PROJECT_NAME}_REVISION_VERSION ${_TARGET_REVISION_VERSION} CACHE INTERNAL "")
		endif()
	endif()
		
	if(${PROJECT_NAME}_REVISION_VERSION)
			set(${_TARGET_UP}_TARGET_PATH ${${PROJECT_NAME}_SOURCE_PATH} CACHE STRING "" FORCE)
			set(${_TARGET_UP}_TARGET_REVISION_VERSION ${${PROJECT_NAME}_REVISION_VERSION} CACHE STRING "" FORCE)
	endif()
endfunction()

# Generating  revision number and path from SVN
function(set_revision_from_svn aTARGET)

	if(NOT DEFINED ${PROJECT_NAME}_REVISION_VERSION)
		Subversion_WC_INFO(${PROJECT_SOURCE_DIR} _TARGET_VERSION)

		set(${PROJECT_NAME}_SOURCE_PATH ${_TARGET_VERSION_WC_ROOT} CACHE INTERNAL "")
		set(${PROJECT_NAME}_REVISION_VERSION ${_TARGET_VERSION_WC_REVISION} CACHE INTERNAL "")
	endif()

	if(${PROJECT_NAME}_REVISION_VERSION)
		set(${_TARGET_UP}_TARGET_PATH ${${PROJECT_NAME}_SOURCE_PATH} CACHE STRING "" FORCE)
		set(${_TARGET_UP}_TARGET_REVISION_VERSION ${${PROJECT_NAME}_REVISION_VERSION} CACHE STRING "" FORCE)
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
	mark_as_advanced(
			${_TARGET_UP}_TARGET_REVISION_VERSION
			${_TARGET_UP}_TARGET_PATH
					)


	find_package(Subversion)
	if(Subversion_FOUND)
		execute_process(COMMAND svn log -rHEAD
				WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
				RESULT_VARIABLE SVN_error
				ERROR_VARIABLE serial_error
				OUTPUT_VARIABLE _TARGET_REVISION_VERSION
				OUTPUT_STRIP_TRAILING_WHITESPACE
				ERROR_STRIP_TRAILING_WHITESPACE
				OUTPUT_QUIET
				ERROR_QUIET
				)	
		if(SVN_error EQUAL 0)
			option(${_TARGET_UP}_TARGET_EMBED_REVISION_SVN "Embeds the SVN revision in the version code" ON)
			mark_as_advanced(${_TARGET_UP}_TARGET_EMBED_REVISION_SVN)
		endif()
	endif()

	find_package(Git)
	if(Git_FOUND)
		execute_process(COMMAND git status
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    RESULT_VARIABLE GIT_error
                    ERROR_VARIABLE serial_error
                    OUTPUT_VARIABLE _TARGET_REVISION_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_STRIP_TRAILING_WHITESPACE
                    OUTPUT_QUIET
					ERROR_QUIET
				)
		if(GIT_error EQUAL 0)
			option(${_TARGET_UP}_TARGET_EMBED_REVISION_GIT "Embeds the GIT SHA in the version code" ON)
			mark_as_advanced(${_TARGET_UP}_TARGET_EMBED_REVISION_GIT)
		endif()
	endif()

	if (${${_TARGET_UP}_TARGET_EMBED_REVISION_GIT})
		set_revision_from_git(${aTARGET})
	endif()

	if (${${_TARGET_UP}_TARGET_EMBED_REVISION_SVN})
		set_revision_from_svn(${aTARGET})
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
	
	if(EXISTS "${aFILE_PATH}/revision.h.in")
		configure_file(${aFILE_PATH}/revision.h.in
                "${aOUT_PATH}/${CONF_TARGET_NAME}_revision.h")
		set_source_files_properties("${aOUT_PATH}/${CONF_TARGET_NAME}_revision.h" 
					PROPERTIES GENERATED TRUE)
		install(FILES "${aOUT_PATH}/${CONF_TARGET_NAME}_revision.h" 
				DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}include/${PROJECT_NAME}/config/${CONF_TARGET_NAME}/" COMPONENT headers
		)					                
	endif()

	
	
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


	if(NOT ${PROJECT_NAME}_NAMESPACE)
		set(${PROJECT_NAME}_NAMESPACE ${PROJECT_NAME})
	endif()

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
#		if (UNIX)					
#			set_target_properties(${aTARGET_NAME} PROPERTIES
#		                      BUILD_RPATH "\$ORIGIN:../${CMAKE_INSTALL_LIBDIR}"
#		                      )
#		endif()		                      					
		add_library(${${PROJECT_NAME}_NAMESPACE}::${aTARGET_NAME}
					ALIAS
					${aTARGET_NAME})
	
		target_compile_definitions(${aTARGET_NAME}
							PRIVATE ${_TARGET_NAME_UPPER}_EXPORTS
							PRIVATE ${${aPRIVATE_DEFINITIONS}}
							
							PUBLIC ${${PROJECT_NAME}_PLATFORM_DEFENITIONS}
							PUBLIC ${${aPUBLIC_DEFINITIONS}}
							)

		foreach(_INCL ${${aPUBLIC_DIR}})
			target_include_directories (${aTARGET_NAME}
					PUBLIC $<BUILD_INTERFACE:${_INCL}>
					)
		endforeach()
		target_include_directories (${aTARGET_NAME}
								PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
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
		                            PRIVATE ${${aPRIVATE_DEFINITIONS}}
									PUBLIC ${_TARGET_NAME_UPPER}_STATIC
									PUBLIC ${${PROJECT_NAME}_PLATFORM_DEFENITIONS}
									PUBLIC ${${aPUBLIC_DEFINITIONS}}
							)
		
        foreach(_INCL ${${aPUBLIC_DIR}})
            target_include_directories (${aTARGET_NAME}_Static
                    PUBLIC $<BUILD_INTERFACE:${_INCL}>
                    )
        endforeach()

        target_include_directories (${aTARGET_NAME}_Static
                PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
                )

		target_include_directories (${aTARGET_NAME}_Static  
								PRIVATE ${${aPRIVATE_DIR}}
								)			
    endif()	

    #INSTALLATION
    if (${aINSTALL_BIN})

		if(TARGET ${aTARGET_NAME} )
			install_target_binary_impl(${aTARGET_NAME} TRUE)
		endif()

        if (TARGET ${${aTARGET_NAME}_Static})
			install_target_binary_impl(${aTARGET_NAME}_Static FALSE)
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
			set(_LIBRARY_NAME ${_LIB}#[[$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>]])
		else()
			set(_LIBRARY_NAME ${_LIB})
		endif()
		
		
		if(${${_TARGET_NAME_UPPER}_WITH_STATIC_DEPENDENCIES})
			if (TARGET ${_LIB}_Static)
				set(_LIBRARY_NAME ${_LIB}_Static#[[$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>]])
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
			set(_LIBRARY_NAME ${_LIB}#[[$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>]])
		else()
			set(_LIBRARY_NAME ${_LIB})
		endif()
				
		if(${${_TARGET_NAME_UPPER}_WITH_STATIC_DEPENDENCIES})
			if (TARGET ${_LIB}_Static)
				set(_LIBRARY_NAME ${_LIB}_Static#[[$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>]])
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
		  	#aCOMONENT_TYPE #- type of component (examples or tests)
			)
	string(TOUPPER ${aTARGET_NAME}
			_TARGET_NAME_UPPER
			)
	if(NOT ${PROJECT_NAME}_NAMESPACE)
		set(${PROJECT_NAME}_NAMESPACE ${PROJECT_NAME})
	endif()
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
		install_target_binary_impl(${aTARGET_NAME} FALSE)
	endif()
endfunction()

#Export library
function(helper_export_library
		aTARGET_NAME # - Name of the library to create.
		)

	string(TOUPPER ${aTARGET_NAME}
			_TARGET_UP
			)
			
	set(_OUT_DIRECTORY ${CMAKE_BINARY_DIR}/include/${aTARGET_NAME}/cmake)

	export(TARGETS ${aTARGET_NAME}
			FILE "${_OUT_DIRECTORY}/${aTARGET_NAME}Targets.cmake"
		   EXPORT_LINK_INTERFACE_LIBRARIES)

	set(CONF_TARGET_NAME ${aTARGET_NAME})
	set(CONF_LOOKING_FOR_FILES "deftype")

	configure_file(${CMAKE_MODULE_PATH_HELPER}/HelperFind.cmake
			"${_OUT_DIRECTORY}/Find${aTARGET_NAME}.cmake" @ONLY)

	include(CMakePackageConfigHelpers)

	if(NOT ${_TARGET_UP}_CONFIG_CMAKE_FILE_PATH)
		set(${_TARGET_UP}_CONFIG_CMAKE_FILE_PATH ${CMAKE_MODULE_PATH_HELPER}/Config.cmake.in)
	endif()

	if(WIN32)
		set(_CMAKE_PACKAGE_INSTALL_DIR
			${CMAKE_PACKAGE_INSTALL_DIR}
			)
	else()
		set(_CMAKE_PACKAGE_INSTALL_DIR
			${CMAKE_PACKAGE_INSTALL_DIR}/${aTARGET_NAME}
			)
	endif()

	configure_package_config_file( ${${_TARGET_UP}_CONFIG_CMAKE_FILE_PATH}
								  "${_OUT_DIRECTORY}/${aTARGET_NAME}Config.cmake"
								INSTALL_DESTINATION ${_CMAKE_PACKAGE_INSTALL_DIR}
								)

	set(_COMONENT_TYPE "cmake")
	if ( DEFINED DIRECTORY_TYPE)
		set(_COMONENT_TYPE ${DIRECTORY_TYPE})
	endif()


	install(FILES  "${_OUT_DIRECTORY}/${aTARGET_NAME}Config.cmake"
			DESTINATION ${_CMAKE_PACKAGE_INSTALL_DIR} COMPONENT ${_COMONENT_TYPE}
			)
	if( ${_TARGET_UP}_TARGET_VERSION)				
		set(_VERSION_CONFIG "${_OUT_DIRECTORY}/${aTARGET_NAME}ConfigVersion.cmake")
		
		write_basic_package_version_file(
			"${_VERSION_CONFIG}"
				VERSION "${${_TARGET_UP}_TARGET_VERSION}"
				COMPATIBILITY SameMajorVersion
		)
		
		install(FILES "${_VERSION_CONFIG}"
						DESTINATION ${_CMAKE_PACKAGE_INSTALL_DIR} COMPONENT ${_COMONENT_TYPE}
					)
	endif()

	install ( EXPORT ${aTARGET_NAME}-export
			FILE "${aTARGET_NAME}Targets.cmake"
			NAMESPACE ${${PROJECT_NAME}_NAMESPACE}::
			DESTINATION ${_CMAKE_PACKAGE_INSTALL_DIR} COMPONENT ${_COMONENT_TYPE}
			)

	install ( FILES
			"${_OUT_DIRECTORY}/Find${aTARGET_NAME}.cmake"
			DESTINATION ${_CMAKE_PACKAGE_INSTALL_DIR} COMPONENT ${_COMONENT_TYPE}
			)
	unset(CONF_LOOKING_FOR_FILES)
	unset(CONF_TARGET_NAME)
endfunction()

# Configure project
macro(configure_project
		 aPROJECT_NAME #	aPROJECT_NAME - Name of the project
		 )

	set(${PROJECT_NAME}_PYTHONS_MODULES ""
	    CACHE INTERNAL "Python modules"
	    FORCE)
	include (CMakeModules/functions/package_python.cmake)

	set(CMAKE_MODULE_PATH
		"${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules"		
		CACHE STRING "Path to search modules" FORCE)		 
	install_default_directory(${aPROJECT_NAME})

	# make sure that the required libraries are
	# always found independent from LD_LIBRARY_PATH and the install
	# location
	if (UNIX)

		set(CMAKE_SKIP_BUILD_RPATH  FALSE)
		set(CMAKE_SKIP_RPATH  FALSE)
		set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
		#if (NOT DEFINED CMAKE_INSTALL_RPATH)
			list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" isSystemDir)
			if ("${isSystemDir}" STREQUAL "-1")
				set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
				file(RELATIVE_PATH _BIN_RELPATH
					 "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}"
					 "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
				list(APPEND CMAKE_INSTALL_RPATH "\$ORIGIN/${_BIN_RELPATH}:../${CMAKE_INSTALL_LIBDIR}:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
				#set(CMAKE_INSTALL_RPATH "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}")
				set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
				set(CMAKE_SKIP_INSTALL_RPATH FALSE)
				message(STATUS " Setup RPATH to " "${CMAKE_INSTALL_RPATH}" )
			else()
				set(CMAKE_SKIP_INSTALL_RPATH TRUE)
			endif ()
		#endif ()
	endif ()

	# default to Release build (it's what most people will use)
	if (NOT CMAKE_BUILD_TYPE)
		set( CMAKE_BUILD_TYPE
			 Release
			 CACHE STRING
			 "Sets the configuration to build (Release, Debug, etc...)"
			 )
	endif()

	include (CMakeModules/configure_${aPROJECT_NAME}.cmake)
	configure_file(CMakeModules/config.h.cmake 
				${CMAKE_BINARY_DIR}/include/${PROJECT_NAME}/config/config.h
				ESCAPE_QUOTES
				)

	install(FILES ${CMAKE_BINARY_DIR}/include/${PROJECT_NAME}/config/config.h
				DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}include/${PROJECT_NAME}/config/" COMPONENT headers
		)
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/CMakeLists.txt")
		add_subdirectory(dependencies)
	endif()
	include (CMakeModules/dependencies_search.cmake)
	include (CMakeModules/functions/configure_doxygen.cmake)
	include (CMakeModules/functions/configure_CPack.cmake)

endmacro()

macro(end_of_project
      aPROJECT_NAME #	aPROJECT_NAME - Name of the project
      )
	if(${PROJECT_NAME}_PYTHONS_MODULES)
		pacakage_python()
	endif()
	generate_cpack()
endmacro()