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
	message(FATAL_ERROR "Failed to find share. set SHARE_ROOT direcoty")
	return()
endif()

find_package(tclap REQUIRED)
if(NOT TCLAP_FOUND)
	message(FATAL_ERROR "TCLAP (http://tclap.sourceforge.net/) could not be found. Set TCLAP_INCLUDE_PATH to point to the headers adding '-DTCLAP_INCLUDE_PATH=/path/to/tclap' to the cmake command.")	
	return()
endif()


if(${CMAKE_VERSION} GREATER_EQUAL "3.12")
	find_package(Python3 COMPONENTS Interpreter Development)
else()
	set(Python3_FOUND FALSE)
endif()
	
if(NOT Python3_FOUND)
	find_package(PythonLibs)
	find_package(PythonInterp)
	if(NOT PYTHONLIBS_FOUND)		
		message(STATUS "Python 3 Language could not be found http://www.python.org/")
	else()
		set(Python3_FOUND TRUE)
	endif()
endif()

find_package(SWIG)
if(NOT SWIG_FOUND)
	message(STATUS "SWIG could not be found http://www.swig.org")
endif()