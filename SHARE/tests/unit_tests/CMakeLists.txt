SET(TARGET_NAME unit_tests)

add_definitions(${LOGGING_DEFENITIONS})

file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file (GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)


share_add_executable(${TARGET_NAME} SOURCE_FILES HEADER_FILES TRUE)
share_target_link_libraries(${TARGET_NAME} ${PLATFORM_LIBS} )
share_add_dependency(${TARGET_NAME} ${SHARE_TARGET_NAME})
#set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 11)

target_include_directories( ${TARGET_NAME} PRIVATE
					${CMAKE_CURRENT_SOURCE_DIR} 
					${SHARE_INCLUDES} 
					${Boost_INCLUDE_DIRS} 
					${LOGGING_INCLUDES}	
					${CMAKE_BINARY_DIR}/include/
					)  