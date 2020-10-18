cmake_minimum_required(VERSION 3.1)

project(UDT C CXX)

set(${PROJECT_NAME}_ORGANIZATION "Free Software" CACHE STRING "Organization ")
set(${PROJECT_NAME}_AUTHORS "https://github.com/CrazyLauren" CACHE STRING "Authors")
set(${PROJECT_NAME}_SUPPORT_ADDRESS "https://github.com/CrazyLauren" CACHE 
												STRING "Address to support")
set(${PROJECT_NAME}_NAMESPACE "CRAZY_LAUREN" CACHE STRING "Namespace of project")
set(${PROJECT_NAME}_LICENCE ${PROJECT_SOURCE_DIR}/LICENCE.txt CACHE PATH "Path to license")

set(${PROJECT_NAME}_VERSION "0.6" CACHE STRING "Version" FORCE)

include(CMakeModules/functions/helper_cmake_function.cmake)
configure_project(${PROJECT_NAME})

################################################################################
# descend into subdirs
################################################################################
add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(examples)
add_subdirectory(tests)

end_of_project(${PROJECT_NAME})

