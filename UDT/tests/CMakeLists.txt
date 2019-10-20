if(${CMAKE_VERSION} VERSION_LESS "3.1.3")
	message(WARNING "\nCannot build tests as cmake version less than 3.1.3")
else()
	include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
	
	if(COMPILER_SUPPORTS_CXX11 OR MSVC_VERSION GREATER 1800)
		enable_testing()

		add_subdirectory(test_selection_nearest_subscribe)
		add_subdirectory(speed_test)
		add_subdirectory(test_errors)
		add_subdirectory(test_inheritance)
		add_subdirectory(test_rtc)
		
		install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/test_kernel.json 
		DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/test)
	else()
		message(WARNING "\nCannot build tests as c++11 is not available")		
	endif()	
endif()

				  