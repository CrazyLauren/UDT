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
	${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/${${PROJECT_NAME}_DEFAULT_PLUGIN_DIR}
	CACHE PATH  "UDT plugins path"
	FORCE)

configure_file(CMakeModules/config.h.cmake 
				${CMAKE_BINARY_DIR}/include/udt/config/config.h
				ESCAPE_QUOTES
				)
				
install(FILES ${CMAKE_BINARY_DIR}/include/udt/config/config.h
				DESTINATION ${CMAKE_INSTALL_PREFIX}/include/udt/config/
		)
