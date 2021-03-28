macro(pacakage_matlab)

    string(TOLOWER ${PROJECT_NAME}
       _PROJECT_LOWER_NAME
       )
	set(${PROJECT_NAME}_MATLAB_PROJECT_NAME ${_PROJECT_LOWER_NAME}
	    CACHE INTERNAL "Matlab project name")
	set(MATLAB_PROJECT_NAME ${${PROJECT_NAME}_MATLAB_PROJECT_NAME})
	set(MATLAB_PACKAGE_PROJECT_AUTHORS ${${PROJECT_NAME}_AUTHORS})
	set(MATLAB_PACKAGE_PROJECT_SUPPORT_ADDRESS ${${PROJECT_NAME}_SUPPORT_ADDRESS})
	set(MATLAB_PACKAGE_PROJECT_VERSION ${${PROJECT_NAME}_VERSION})

	set(MATLAB_PACKAGE_VERSION ${Matlab_VERSION_STRING})
    string(UUID MATLAB_PACKAGE_UUID NAMESPACE 9735D882-D2F8-4E1D-88C9-A0A4F1F6ECA4
           NAME ${${PROJECT_NAME}_MATLAB_PROJECT_NAME} TYPE SHA1)

	if(NOT ${PROJECT_NAME}_MATLAB_PACKAGE_PROJECT_FILE_PATH)
		set(MATLAB_PACKAGE_PROJECT_FILE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules/functions/matlab/matlab.prj.in)
	else()
		set(MATLAB_PACKAGE_PROJECT_FILE_PATH ${${PROJECT_NAME}_MATLAB_PACKAGE_PROJECT_FILE_PATH})
	endif()

	set(MATLAB_PACKAGE_BUILD_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/matlab_package)
	set(MATLAB_PACKAGE_PRJ ${MATLAB_PACKAGE_BUILD_LOCATION}/${MATLAB_PROJECT_NAME}.prj)
	set(MATLAB_PACKAGE_TOOLBOX_ROOT ${MATLAB_PACKAGE_BUILD_LOCATION}/+${MATLAB_PROJECT_NAME})
    set(MATLAB_PACKAGE_DATA_ROOT ${MATLAB_PACKAGE_BUILD_LOCATION}/data)

	set(MATLAB_PACKAGE_MLTBX ${MATLAB_PACKAGE_BUILD_LOCATION}/${MATLAB_PROJECT_NAME}.mltbx)

	set(MATLAB_SRCS )

	set(_MATLAB_COPY_LIBRARY_COMMAND "")

	set(_MATLAB_CREATE_DIRECTORIES
	    "COMMAND \${CMAKE_COMMAND} -E make_directory \${MATLAB_PACKAGE_TOOLBOX_ROOT}
		\n COMMAND \${CMAKE_COMMAND} -E make_directory \${MATLAB_PACKAGE_DATA_ROOT}"
	    )

	set(MATLAB_PACAKGE_TOOLBOX_SOURCES "")
	set(MATLAB_PACAKGE_SRC_ITEM_XML "")
	set(_MATLAB_COPY_LIBRARY_DEPS "")

	foreach(_TARGET ${${PROJECT_NAME}_MATLAB_MODULES})
		search_library_dependencies(${_TARGET} ${_TARGET} _MATLAB_COPY_LIBRARY_DEPS)
#		get_target_property(_LOCATION ${_TARGET} IMPORTED_LOCATION)
		set(_NEW_COPY_VAL
		    "COMMAND \${CMAKE_COMMAND} -E copy \$<TARGET_FILE:${_TARGET}> \${MATLAB_PACKAGE_TOOLBOX_ROOT}/"
		    )
		string(APPEND _MATLAB_COPY_LIBRARY_COMMAND "\n${_NEW_COPY_VAL}")

		string(APPEND MATLAB_PACAKGE_SRC_ITEM_XML
		       "<file>${MATLAB_PACKAGE_TOOLBOX_ROOT}/$<TARGET_FILE_NAME:${_TARGET}></file>\n"
		       )
	endforeach()

	if(_MATLAB_COPY_LIBRARY_DEPS)
		foreach(_LB ${_MATLAB_COPY_LIBRARY_DEPS})
			if(WIN32)
				string(APPEND _MATLAB_COPY_LIBRARY_COMMAND
				       "\nCOMMAND \${CMAKE_COMMAND} -E copy \$<TARGET_FILE:${_LB}> \${MATLAB_PACKAGE_TOOLBOX_ROOT}/")
				string(APPEND MATLAB_PACAKGE_SRC_ITEM_XML
				       "<file>${MATLAB_PACKAGE_TOOLBOX_ROOT}/$<TARGET_FILE_NAME:${_LB}></file>\n"
				       )
			else()
				string(APPEND _MATLAB_COPY_LIBRARY_COMMAND
				       "\nCOMMAND \${CMAKE_COMMAND} -E copy \$<TARGET_FILE:${_LB}> \${MATLAB_PACKAGE_TOOLBOX_ROOT}/\$<TARGET_SONAME_FILE_NAME:${_LB}> ")
				string(APPEND MATLAB_PACAKGE_SRC_ITEM_XML
				       "<file>${MATLAB_PACKAGE_TOOLBOX_ROOT}/$<TARGET_SONAME_FILE_NAME:${_LB}></file>\n"
				       )
			endif()
		endforeach()

	endif()
    foreach(_DATA ${${PROJECT_NAME}_MATLAB_DATAS})
	    #		get_target_property(_LOCATION ${_TARGET} IMPORTED_LOCATION)
	    set(_NEW_COPY_VAL
	        "COMMAND \${CMAKE_COMMAND} -E copy ${_DATA} \${MATLAB_PACKAGE_DATA_ROOT}/"
	        )
	    string(APPEND _MATLAB_COPY_LIBRARY_COMMAND "\n${_NEW_COPY_VAL}")

	    get_filename_component(_FILE_NAME ${_DATA} NAME)
	    string(APPEND MATLAB_PACAKGE_SRC_ITEM_XML
	           "<file>data/${_FILE_NAME}</file>\n"
	           )
    endforeach()


	configure_file(${MATLAB_PACKAGE_PROJECT_FILE_PATH}
	               ${MATLAB_PACKAGE_PRJ}.tmp @ONLY)

	file(GENERATE OUTPUT  ${MATLAB_PACKAGE_PRJ}
	     INPUT ${MATLAB_PACKAGE_PRJ}.tmp)

	configure_file(
			${PROJECT_SOURCE_DIR}/CMakeModules/functions/matlab/matlab_package.cmake.in
			${CMAKE_BINARY_DIR}/${PROJECT_NAME}/cmake/matlab/matlab_package_${PROJECT_NAME}.cmake
			@ONLY)
	include(${CMAKE_BINARY_DIR}/${PROJECT_NAME}/cmake/matlab/matlab_package_${PROJECT_NAME}.cmake)


endmacro()
macro (helper_add_matlab_library
       aTARGET_NAME # - Name of the library to create.
       aSOURCE_FILES_VAR
       aHEADER_FILES_VAR

       aPUBLIC_DIR # - public include dir
       aPRIVATE_DIR # - private include dir

       aPUBLIC_DEFINITIONS
       aPRIVATE_DEFINITIONS

       aPUBLIC_LIBRARIS

       aOUTPUT_NAME #  - the output path.
       aINSTALL_PATH # install path
       )

set(_TARGET_NAME ${aTARGET_NAME})
string(TOUPPER ${_TARGET_NAME}
       _TARGET_NAME_UP
       )

if(NOT ${PROJECT_NAME}_NAMESPACE)
	set(${PROJECT_NAME}_NAMESPACE ${PROJECT_NAME})
endif()
if(NOT aOUTPUT_NAME)
	set(aOUTPUT_NAME ${_TARGET_NAME})
endif()

matlab_add_mex(
		NAME ${_TARGET_NAME}
		OUTPUT_NAME ${aOUTPUT_NAME}
		SRC ${${aSOURCE_FILES_VAR}}
		LINK_TO ${${aPUBLIC_LIBRARIS}})

target_compile_definitions(${_TARGET_NAME}
                           PRIVATE ${_TARGET_NAME_UPPER}_EXPORTS
                           PRIVATE ${${aPRIVATE_DEFINITIONS}}

                           PUBLIC ${${PROJECT_NAME}_PLATFORM_DEFINITIONS}
                           PUBLIC ${${aPUBLIC_DEFINITIONS}}
                           )

foreach(_INCL ${${aPUBLIC_DIR}})
	target_include_directories (${aTARGET_NAME}
	                            PUBLIC $<BUILD_INTERFACE:${_INCL}>
	                            )
endforeach()

foreach(_INCL ${${aPRIVATE_DIR}})
	target_include_directories (${aTARGET_NAME}
	                            PRIVATE $<BUILD_INTERFACE:${_INCL}>
	                            )
endforeach()

add_library(${${PROJECT_NAME}_NAMESPACE}::${_TARGET_NAME} ALIAS ${_TARGET_NAME})

if (${aINSTALL_PATH})

	install(TARGETS ${_TARGET_NAME}
	        LIBRARY DESTINATION ${aINSTALL_PATH} COMPONENT libraries
	        ARCHIVE DESTINATION ${aINSTALL_PATH} COMPONENT libraries
	        RUNTIME DESTINATION ${aINSTALL_PATH} COMPONENT libraries
	        )
endif()
set(${PROJECT_NAME}_MATLAB_MODULES ${${PROJECT_NAME}_MATLAB_MODULES} ${_TARGET_NAME}
    CACHE INTERNAL "Matlab modules"
    FORCE)
endmacro()