include(FindPackageHandleStandardArgs)

################################################################################
#Look for packages
################################################################################

if(NOT DEFINED BOOST_ROOT)
	SET(BOOST_ROOT ${${PROJECT_NAME}_DEPENDENCIES_PATH} CACHE PATH "boost path")
endif(NOT DEFINED BOOST_ROOT)
	
find_package(Boost REQUIRED)

if(NOT Boost_FOUND)
	message(FATAL_ERROR "Failed to find boost.")	
	return()
endif()

find_package(SHARE REQUIRED)

if(NOT SHARE_FOUND)
	message(FATAL "Failed to find share. set SHARE_ROOT direcoty")
	return()
endif()

find_package(tclap REQUIRED)
if(NOT TCLAP_FOUND)
	message(FATAL_ERROR "TCLAP (http://tclap.sourceforge.net/) could not be found. Set TCLAP_INCLUDE_PATH to point to the headers adding '-DTCLAP_INCLUDE_PATH=/path/to/tclap' to the cmake command.")	
	return()
endif()
