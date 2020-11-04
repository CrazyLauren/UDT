macro(pacakage_python)
set(PYTHON_SETUP_PROJECT_NAME ${PROJECT_NAME} )
set(PYTHON_SETUP_PROJECT_VERSION ${${PROJECT_NAME}_VERSION})
set(PYTHON_SETUP_PROJECT_AUTHORS ${${PROJECT_NAME}_AUTHORS})
set(PYTHON_SETUP_PROJECT_SUPPORT_ADDRESS ${${PROJECT_NAME}_SUPPORT_ADDRESS})
set(PYTHON_SETUP_PROJECT_URL ${${PROJECT_NAME}_SUPPORT_ADDRESS})
set(PYTHON_SETUP_PYTHONS_MODULES ${${PROJECT_NAME}_PYTHONS_MODULES})


set(PYTHON_SETUP_PACKAGE_DATA "")
set(_COPY_LIBRARY_COMMAND "")
set(_COPY_LIBRARY_DEPS )
set(_DEPENDS_PROJECT "")
set(_CREATE_DIRECTORIES "COMMAND \${CMAKE_COMMAND} -E make_directory \${PROJECT_NAME}/${CMAKE_INSTALL_LIBDIR}")

set(_PACKET_PYTHON_REQUIREMENT )

foreach(_PACKET ${PYTHON_SETUP_PYTHONS_MODULES})
	set(_NEW_VAL "'${_PACKET}/*'")	
	string(APPEND PYTHON_SETUP_PACKAGE_DATA
		       ", ${_NEW_VAL}")

	set(_NEW_COPY_VAL "COMMAND \${CMAKE_COMMAND} -E copy \$<TARGET_FILE:${_PACKET}> \${PROJECT_NAME}/${_PACKET}/")
	string(APPEND _COPY_LIBRARY_COMMAND
	    "\n${_NEW_COPY_VAL}")
	if(DEFINED ${_PACKET}_PYTHONS_MODULES)

		foreach(_PVAL ${${_PACKET}_PYTHONS_MODULES})
			string(APPEND _COPY_LIBRARY_COMMAND
			       "\nCOMMAND \${CMAKE_COMMAND} -E copy \"${_PVAL}\" \${PROJECT_NAME}/${_PACKET}/")
		endforeach()
	endif()

	if(DEFINED ${_PACKET}_PYTHONS_DEPENDS_MODULES)
		list(APPEND _PACKET_PYTHON_REQUIREMENT ${${_PACKET}_PYTHONS_DEPENDS_MODULES})
	endif()
	
	search_library_dependencies(${_PACKET} ${_PACKET} _COPY_LIBRARY_DEPS)

	string(APPEND _DEPENDS_PROJECT
	    " ${_PACKET}")

	string(APPEND _CREATE_DIRECTORIES
	       "\nCOMMAND \${CMAKE_COMMAND} -E make_directory \${PROJECT_NAME}/${_PACKET}")
	set(_INIT_CODE "${_INIT_CODE}\nfrom .${_PACKET} import ${_PACKET}")	       
endforeach()

if(_PACKET_PYTHON_REQUIREMENT)

	list(GET _PACKET_PYTHON_REQUIREMENT 0 _FIRST )
	set(PYTHON_SETUP_INSTALL_REQUIREMENTS "install_requires=['${_FIRST}'")
	list(REMOVE_AT _PACKET_PYTHON_REQUIREMENT 0)
			
	if(_PACKET_PYTHON_REQUIREMENT)			
		foreach(_DEP ${_PACKET_PYTHON_REQUIREMENT})
			string(APPEND PYTHON_SETUP_INSTALL_REQUIREMENTS
		    	", '${_DEP}'")		
		endforeach()
	endif()
	string(APPEND PYTHON_SETUP_INSTALL_REQUIREMENTS
	    "],")
endif()	

configure_file(
		${PROJECT_SOURCE_DIR}/CMakeModules/functions/python/setup.py.in
		${CMAKE_BINARY_DIR}/${PROJECT_NAME}/python/setup.py.in
		@ONLY ESCAPE_QUOTES)		
file(GENERATE
     OUTPUT ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/python/package/setup.py
     INPUT ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/python/setup.py.in)

if(_COPY_LIBRARY_DEPS)
	set(_COPY_LIBRARY_DEPS_COMMAND "" )
	foreach(_LB ${_COPY_LIBRARY_DEPS})
		if(WIN32)
			string(APPEND _COPY_LIBRARY_DEPS_COMMAND
       			"\nCOMMAND \${CMAKE_COMMAND} -E copy \$<TARGET_FILE:${_LB}> \${PROJECT_NAME}/${CMAKE_INSTALL_LIBDIR}/")
		else()
			string(APPEND _COPY_LIBRARY_DEPS_COMMAND
       			"\nCOMMAND \${CMAKE_COMMAND} -E copy \$<TARGET_FILE:${_LB}> \${PROJECT_NAME}/${CMAKE_INSTALL_LIBDIR}/\$<TARGET_SONAME_FILE_NAME:${_LB}> ")		
		endif()       					    					
	endforeach()
	
	if(WIN32)
		set(_INIT_CODE "import os\nimport pathlib\n_dll_path=os.path.join(pathlib.Path(__file__).parent.absolute(),'${CMAKE_INSTALL_LIBDIR}')\ntry:\n	os.environ['PATH']+=';'\n	os.environ['PATH']+=_dll_path\n	os.add_dll_directory(_dll_path)\nexcept AttributeError:\n	pass\n${_INIT_CODE}")
	endif()		
endif()
configure_file(
		${PROJECT_SOURCE_DIR}/CMakeModules/functions/python/__init__.py.in
		${CMAKE_BINARY_DIR}/${PROJECT_NAME}/python/package/${PROJECT_NAME}/__init__.py
		@ONLY)

# Find if python module MODULE_NAME is available,
# if not install it to the Python user install directory.
macro(search_python_module MODULE_NAME)
	execute_process(
			COMMAND ${Python3_EXECUTABLE} -c "import ${MODULE_NAME}; print(${MODULE_NAME}.__version__)"
			RESULT_VARIABLE _RESULT
			OUTPUT_VARIABLE MODULE_VERSION
			ERROR_QUIET
			OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	if(${_RESULT} STREQUAL "0")
		message(STATUS "Found python module: ${MODULE_NAME} (found version \"${MODULE_VERSION}\")")
		set(${MODULE_NAME}_Found TRUE)
	else()
		message(STATUS "Can't find python module \"${MODULE_NAME}\", user install it using pip...")
		set(${MODULE_NAME}_Found FALSE)
	endif()
endmacro()

# Look for required python modules
search_python_module(setuptools)
search_python_module(wheel)

if(setuptools_Found AND wheel_Found)
	message(STATUS "Generate python setup tools")
	configure_file(
		${PROJECT_SOURCE_DIR}/CMakeModules/functions/python/python_package.cmake.in
		${CMAKE_BINARY_DIR}/${PROJECT_NAME}/cmake/python/python_package_${PROJECT_NAME}.cmake
		@ONLY)
	include(${CMAKE_BINARY_DIR}/${PROJECT_NAME}/cmake/python/python_package_${PROJECT_NAME}.cmake)
	if(WHL_FILE)
		
	endif()
endif()

endmacro()

macro (helper_add_swig_python_library
				aTARGET_NAME # - Name of the library to create.
				aPUBLIC_LIBRARIS
				aPUBLIC_DIR # - public include dir
				aPRIVATE_DIR # - private include dir
                aOUTPUT_PATH #  - the output path.
                aINSTALL_PATH # Path of isntall
		)
	set(CMAKE_SWIG_FLAGS "")

	set(_TARGET_NAME ${aTARGET_NAME})
	string(TOUPPER ${_TARGET_NAME}
	       _TARGET_NAME_UP
	       )
	set_source_files_properties("${_TARGET_NAME}.i" PROPERTIES CPLUSPLUS ON)
	set_source_files_properties("${_TARGET_NAME}.i" PROPERTIES SWIG_FLAGS "-includeall")
	set_property(SOURCE "${_TARGET_NAME}.i" PROPERTY SWIG_MODULE_NAME ${_TARGET_NAME})

	swig_add_library(${_TARGET_NAME}
	                 TYPE MODULE
	                 LANGUAGE python
	                 OUTPUT_DIR "${aOUTPUT_PATH}/${_TARGET_NAME}/"
	                 SOURCES ${_TARGET_NAME}.i
	                 )
	#swig_add_module(${_TARGET_NAME} python ../customer_python.i)
	set_target_properties(${SWIG_MODULE_${_TARGET_NAME}_REAL_NAME} PROPERTIES	                      
	                      SWIG_USE_TARGET_INCLUDE_DIRECTORIES TRUE
	                      )


	foreach(_INCL ${${aPUBLIC_DIR}})
		target_include_directories (${SWIG_MODULE_${_TARGET_NAME}_REAL_NAME}
					PUBLIC $<BUILD_INTERFACE:${_INCL}>
					)
	endforeach()
	target_include_directories (${SWIG_MODULE_${_TARGET_NAME}_REAL_NAME}
								PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
								)

    target_include_directories (${SWIG_MODULE_${_TARGET_NAME}_REAL_NAME}
								PRIVATE ${${aPRIVATE_DIR}}
								)
									                      
	if (UNIX)
		set_target_properties(${SWIG_MODULE_${_TARGET_NAME}_REAL_NAME} PROPERTIES
		                      INSTALL_RPATH "\$ORIGIN/:\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}:../../${PROJECT_NAME}/${CMAKE_INSTALL_LIBDIR}"
		                      )
		set_target_properties(${SWIG_MODULE_${_TARGET_NAME}_REAL_NAME} PROPERTIES
		                      BUILD_RPATH "\$ORIGIN/:\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}:../../${PROJECT_NAME}/${CMAKE_INSTALL_LIBDIR}"
		                      )
	endif ()

	if (PYTHONLIBS_FOUND)
		target_include_directories(${SWIG_MODULE_${_TARGET_NAME}_REAL_NAME}
		                           PRIVATE ${PYTHON_INCLUDE_PATH})
		swig_link_libraries(${_TARGET_NAME}
				 ${${aPUBLIC_LIBRARIS}} 
				 ${PYTHON_LIBRARIES})
	else ()
		swig_link_libraries(${_TARGET_NAME} 
				${${aPUBLIC_LIBRARIS}}
				 Python3::Python)
	endif ()

	set(${PROJECT_NAME}_PYTHONS_MODULES ${${PROJECT_NAME}_PYTHONS_MODULES} ${SWIG_MODULE_${_TARGET_NAME}_REAL_NAME}
	    CACHE INTERNAL "Python modules"
	    FORCE)
	set(${SWIG_MODULE_${_TARGET_NAME}_REAL_NAME}_PYTHONS_MODULES
	    "${aOUTPUT_PATH}/${_TARGET_NAME}/${_TARGET_NAME}.py"
	    CACHE INTERNAL "Python modules of ${_TARGET_NAME}"
	    FORCE)
#	install(FILES 
#			"${aOUTPUT_PATH}/${_TARGET_NAME}/${_TARGET_NAME}.py"
#	        DESTINATION ${aINSTALL_PATH} COMPONENT python_package)
#	install(TARGETS ${aTARGET_NAME}
#			LIBRARY DESTINATION "${aINSTALL_PATH}" COMPONENT python_package
#			ARCHIVE DESTINATION "${aINSTALL_PATH}" COMPONENT python_package
#			RUNTIME DESTINATION "${aINSTALL_PATH}" COMPONENT python_package
#			)	        
endmacro()