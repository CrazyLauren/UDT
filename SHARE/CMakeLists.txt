cmake_minimum_required(VERSION 3.1)

project(SHARE C CXX)

set(${PROJECT_NAME}_NAMESPACE "CRAZY_LAUREN" CACHE STRING "Namespace ")

set(${PROJECT_NAME}_ORGANIZATION "Free Software" CACHE STRING "Organization ")
set(${PROJECT_NAME}_AUTHORS "https://github.com/CrazyLauren" CACHE STRING "Authors")
set(${PROJECT_NAME}_SUPPORT_ADDRESS "https://github.com/CrazyLauren" CACHE 
												STRING "Address to support")
set(${PROJECT_NAME}_NAMESPACE "CRAZY_LAUREN" CACHE STRING "Namespace of project")
set(${PROJECT_NAME}_LICENCE ${PROJECT_SOURCE_DIR}/LICENCE.txt CACHE PATH "Path to license")


include(CMakeModules/helper_cmake_function.cmake)
configure_project(${PROJECT_NAME})
include (CMakeModules/share_function.cmake)


########################################
# descend into subdirs
########################################
add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(examples)
add_subdirectory(tests)


		  