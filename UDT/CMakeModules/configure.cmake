include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckIncludeFileCXX)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckCXXSourceCompiles)
include(CheckTypeSize)
include(FindPackageHandleStandardArgs)

if (MSVC)
	# This option is to enable the /MP switch for Visual Studio 2005 and above compilers
	option(BUILD_BY_MP "Set to ON to build  with the /MP option (Visual Studio 2005 and above)." ON)

	mark_as_advanced(BUILD_BY_MP)

	if(BUILD_BY_MP)
    	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	endif(BUILD_BY_MP)

endif()
set(UDT_NO_ROUTE_IS_ERROR
	false 
	CACHE BOOL  "Is no route for packet is error?")

if(NOT DEFINED INSTALL_PREFIX_OLD OR
   NOT ("${INSTALL_PREFIX_OLD}" STREQUAL "${${PROJECT_NAME}_INSTALL_PREFIX}")
   )

	if(NOT WIN32 AND ("${${PROJECT_NAME}_INSTALL_PREFIX}" STREQUAL ""))

		set(${PROJECT_NAME}_DEFAULT_PLUGIN_DIR
			"/var/lib/${PROJECT_NAME}/plugins"
			CACHE PATH  "UDT plugins directory"
			FORCE)

		set(${PROJECT_NAME}_INSTALL_PLUGIN_PATH
			"${${PROJECT_NAME}_DEFAULT_PLUGIN_DIR}"
			CACHE PATH  "UDT plugins path"
			FORCE)

		set(${PROJECT_NAME}_DEFAULT_CUSTOMER_PLUGIN_DIR
			"/var/lib/${PROJECT_NAME}/plugins_customer"
			CACHE PATH  "UDT plugins directory"
			FORCE)

		set(${PROJECT_NAME}_INSTALL_CUSTOMER_PLUGIN_PATH
			"${${PROJECT_NAME}_DEFAULT_CUSTOMER_PLUGIN_DIR}"
			CACHE PATH  "UDT plugins path"
			FORCE)

		set(${PROJECT_NAME}_GUI_PATH
			"/var/lib/${PROJECT_NAME}/gui"
			CACHE STRING "The absolute path for GUI"
			FORCE
			)
		set(UDT_INSTALL_GUI_PATH
			${${PROJECT_NAME}_GUI_PATH}
			CACHE STRING "The absolute path to install GUI"
			FORCE)
		set(${PROJECT_NAME}_CONFIG_DEFAULT_PATH
			"${CMAKE_INSTALL_FULL_SYSCONFDIR}/${PROJECT_NAME}/"
			CACHE STRING "The absolute path for default config path"
			FORCE
			)
		set(UDT_INSTALL_CONFIG_DEFAULT_PATH
			${${PROJECT_NAME}_CONFIG_DEFAULT_PATH}
			CACHE STRING "The absolute path to install config"
			FORCE)
	else()
		if(NOT WIN32)
			set(${PROJECT_NAME}_DEFAULT_PLUGIN_DIR	
				"${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}/plugins"	
				CACHE PATH  "UDT plugins directory" FORCE)
			set(${PROJECT_NAME}_INSTALL_PLUGIN_PATH
			"${${PROJECT_NAME}_DEFAULT_PLUGIN_DIR}"
			CACHE PATH  "UDT plugins path"
			FORCE)				
		else()
			set(${PROJECT_NAME}_DEFAULT_PLUGIN_DIR	
				"./plugins"	
				CACHE PATH  "UDT plugins directory" FORCE)
			set(${PROJECT_NAME}_INSTALL_PLUGIN_PATH
				"${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}/plugins"
				CACHE PATH  "UDT plugins path"
				FORCE)				
		endif()
		
		if(NOT WIN32)
			set(${PROJECT_NAME}_DEFAULT_CUSTOMER_PLUGIN_DIR	
				"${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}/plugins_customer"
				 CACHE PATH  "UDT plugins directory" FORCE)
			set(${PROJECT_NAME}_INSTALL_CUSTOMER_PLUGIN_PATH
				"${${PROJECT_NAME}_DEFAULT_CUSTOMER_PLUGIN_DIR}"
				CACHE PATH  "UDT plugins path"
				FORCE)				 		
		else()
			set(${PROJECT_NAME}_DEFAULT_CUSTOMER_PLUGIN_DIR	"./plugins_customer" 
				CACHE PATH  "UDT plugins directory" FORCE)
			set(${PROJECT_NAME}_INSTALL_CUSTOMER_PLUGIN_PATH
				"${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}/plugins_customer"
				CACHE PATH  "UDT plugins path"
				FORCE)				
		endif()	
		
		
		if(NOT WIN32)
			set(${PROJECT_NAME}_GUI_PATH "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}/gui" 
				CACHE STRING "The absolute path for GUI" FORCE)
		else()
			set(${PROJECT_NAME}_GUI_PATH "./gui"
			 CACHE STRING "The absolute path for GUI" FORCE)
		endif()			
		set(UDT_INSTALL_GUI_PATH
			"${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}/gui"
			CACHE STRING "The absolute path to install GUI"
			FORCE)
		
		if(NOT WIN32)
			set(${PROJECT_NAME}_CONFIG_DEFAULT_PATH 
				${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}/ 
				CACHE STRING "The absolute path for default config path" FORCE)				
		else()
			set(${PROJECT_NAME}_CONFIG_DEFAULT_PATH 
				"./" CACHE STRING "The absolute path for default config path" FORCE)
		endif()
						
		set(UDT_INSTALL_CONFIG_DEFAULT_PATH
			${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}
			CACHE STRING "The absolute path to install config"
			FORCE)
	endif()
	set(INSTALL_PREFIX_OLD ${${PROJECT_NAME}_INSTALL_PREFIX} CACHE INTERNAL "Copy of INSTALL_PREFIX" FORCE)
endif()
set(${PROJECT_NAME}_MAX_MESSAGE_HEADER_SIZE
			"32"
			CACHE STRING  "Max message header size"
			)	

set(${PROJECT_NAME}_BUILD_COMPONENT_PYTHON
    TRUE
    CACHE BOOL
    "Build Python bindings")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)	