if(${CMAKE_VERSION} VERSION_LESS "3.1.3")
	message(WARNING "\nCannot build tests as cmake version less than 3.1.3")
else()
	include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
	
	if(COMPILER_SUPPORTS_CXX11 OR MSVC_VERSION GREATER 1800)		
		add_subdirectory(test_logging)		
	else()
		message(WARNING "\nCannot build tests as c++11 is not available")		
	endif()	
endif()

				  