file (GLOB SOURCE_FILES *.cpp)
file (GLOB HEADER_FILES *.hpp)

set (TARGET_NAME example_sm_server)
include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (TARGET_LIB_TYPE "EXECUTABLE")
add_executable (${TARGET_NAME} ${SOURCE_FILES} ${HEADER_FILES})
include_directories (${SHARE_INCLUDE_PATH} )
target_link_libraries(${TARGET_NAME} ${PLATFORM_LIBS} ${SHARE_TARGET_NAME} )


install (TARGETS ${TARGET_NAME} 
             RUNTIME DESTINATION "bin" )	


		
		