PROJECT(SHARE_OF_UDT C CXX)
SET(TARGET_NAME udt_share)

SET(UDT_SHARE_TARGET_NAME ${TARGET_NAME})
SET(UDT_SHARE_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE PATH  "Include path for the header files of udt share" )

add_definitions(${LOGGING_DEFENITIONS})

configure_version(UDT_SHARE ${CMAKE_CURRENT_SOURCE_DIR}/src 0 1)

file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
list (APPEND SOURCE_FILES    ${CMAKE_CURRENT_SOURCE_DIR}/src/revision.c)

file (GLOB HEADER_FILES ../include/*.h ../include/*.hpp)

include_directories (${SHARE_INCLUDES} ${Boost_INCLUDE_DIRS} ${UDT_SHARE_INCLUDES} ${LOGGING_INCLUDES})

share_add_library(${TARGET_NAME} FALSE SOURCE_FILES HEADER_FILES TRUE)

share_target_link_libraries(${TARGET_NAME} ${PLATFORM_LIBS}  ${LOGGING_LIBRARIES} )
share_add_dependency(${TARGET_NAME} ${SHARE_LIBRARIES})

SET(UDT_SHARE_LIBRARIES  ${TARGET_NAME} CACHE STRING "UDT_SHARE_LIBRARIES" FORCE)#fixme must be filepath

						  
install(FILES 	${CMAKE_CURRENT_SOURCE_DIR}/include/udt_types.h
				${CMAKE_CURRENT_SOURCE_DIR}/include/udt_share_macros.h
				${CMAKE_CURRENT_SOURCE_DIR}/include/programm_id.h
				${CMAKE_CURRENT_SOURCE_DIR}/include/IExtParser.h
				${CMAKE_CURRENT_SOURCE_DIR}/include/CParserFactory.h
		DESTINATION ${CMAKE_INSTALL_PREFIX}/include)						  
						  
