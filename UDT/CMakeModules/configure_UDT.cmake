include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckIncludeFileCXX)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckCXXSourceCompiles)
include(CheckTypeSize)
include(FindPackageHandleStandardArgs)

if (WIN32)
	# This option is to enable the /MP switch for Visual Studio 2005 and above compilers
	option(BUILD_BY_MP "Set to ON to build  with the /MP option (Visual Studio 2005 and above)." OFF)

	mark_as_advanced(BUILD_BY_MP)

	if(BUILD_BY_MP)
    	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	endif(BUILD_BY_MP)

endif()

set(${PROJECT_NAME}_DEFAULT_PLUGIN_DIR
	"./plugins"
	CACHE PATH  "UDT plugins directory")

set(${PROJECT_NAME}_DEFAULT_PLUGIN_PATH
	"${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}/${${PROJECT_NAME}_DEFAULT_PLUGIN_DIR}"
	CACHE PATH  "UDT plugins path"
	FORCE)

if(NOT DEFINED INSTALL_PREFIX_OLD OR
   NOT ("${INSTALL_PREFIX_OLD}" STREQUAL "${${PROJECT_NAME}_INSTALL_PREFIX}")
   )

	if(NOT WIN32 AND ("${${PROJECT_NAME}_INSTALL_PREFIX}" STREQUAL ""))

		set(${PROJECT_NAME}_GUI_PATH
			"/var/lib/${PROJECT_NAME}/"
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
		set(${PROJECT_NAME}_GUI_PATH "./" CACHE STRING "The absolute path for GUI" FORCE)
		set(UDT_INSTALL_GUI_PATH
			${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}
			CACHE STRING "The absolute path to install GUI"
			FORCE)
		set(${PROJECT_NAME}_CONFIG_DEFAULT_PATH "./" CACHE STRING "The absolute path for default config path" FORCE)
		set(UDT_INSTALL_CONFIG_DEFAULT_PATH
			${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}
			CACHE STRING "The absolute path to install config"
			FORCE)
	endif()
	set(INSTALL_PREFIX_OLD ${${PROJECT_NAME}_INSTALL_PREFIX} CACHE INTERNAL "Copy of INSTALL_PREFIX" FORCE)
endif()
