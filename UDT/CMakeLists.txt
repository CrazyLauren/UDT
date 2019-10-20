cmake_minimum_required(VERSION 3.1)

project(UDT C CXX)
set(${PROJECT_NAME}_NAMESPACE "CRAZY_LAUREN" CACHE STRING "Namespace ")


set(CMAKE_MODULE_PATH
		"${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules"
		CACHE STRING "Path to search modules" FORCE)

include(helper_cmake_function)

install_default_directory(${PROJECT_NAME})

# default to Release build (it's what most people will use)
if (NOT CMAKE_BUILD_TYPE)
	set( CMAKE_BUILD_TYPE
			Release
			CACHE STRING
			"Sets the configuration to build (Release, Debug, etc...)"
			)
endif()

include (CMakeModules/configure_${PROJECT_NAME}.cmake)
include (CMakeModules/dependencies_search.cmake)
include (CMakeModules/configure_doxygen.cmake)
#include (CMakeModules/share_function.cmake)



################################################################################
# descend into subdirs
################################################################################
add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(examples)
add_subdirectory(tests)

install ( FILES
		${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/FindUDT.cmake
		DESTINATION ${CMAKE_PACKAGE_INSTALL_DIR}
		)