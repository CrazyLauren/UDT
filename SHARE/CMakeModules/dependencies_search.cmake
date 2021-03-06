include(FindPackageHandleStandardArgs)

################################################################################
#Look for packages
################################################################################

find_package(Boost)

if(NOT Boost_FOUND)

	if(NOT DEFINED BOOST_ROOT)
		set(BOOST_ROOT ${${PROJECT_NAME}_DEPENDENCIES_PATH} CACHE PATH "boost path")
	endif(NOT DEFINED BOOST_ROOT)

	find_package(Boost REQUIRED)

	if(NOT Boost_FOUND)
		message(FATAL_ERROR "Failed to find boost.")
		return()
	endif()
endif()

find_package(rapidjson REQUIRED)
if(NOT RAPIDJSON_FOUND)
	message(FATAL_ERROR "Failed to find rapidjson.")	
	return()
endif()

find_package(tclap REQUIRED)
if(NOT TCLAP_FOUND)
	message(FATAL_ERROR "TCLAP (http://tclap.sourceforge.net/) could not be found. Set TCLAP_INCLUDE_PATH to point to the headers adding '-DTCLAP_INCLUDE_PATH=/path/to/tclap' to the cmake command.")
	return()
endif()

find_package(VTune)
find_package(Glog)
find_package(Log4cplus)

if (WIN32)
	find_library(LIBADVAPI32 advapi32)	
endif()