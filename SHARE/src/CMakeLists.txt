
include(CheckFunctionExists)


				
file(GLOB_RECURSE TARGET_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/revision.c.in
                ${CMAKE_CURRENT_SOURCE_DIR}/revision.c)				
list (APPEND TARGET_SRC    ${CMAKE_CURRENT_SOURCE_DIR}/revision.c)

file (GLOB CORE_HEADER_FILES ../include/*.h ../include/*.hpp ../include/logging/*.h ../include/logging/*.hpp)
list (APPEND CORE_HEADER_FILES    ${CMAKE_CURRENT_SOURCE_DIR}/../include/deftype)

file (GLOB SOCKET_HEADER_FILES ../include/Socket/*.h ../include/Socket/*.hpp)
file (GLOB UTYPE_HEADER_FILES ../include/UType/*.h ../include/UType/*.hpp)
file (GLOB UTYPE_EXT_HEADER_FILES ../include/UType/ext/*.h ../include/UType/ext/*.hpp)

set (ALL_HEADER_FILES ${CORE_HEADER_FILES} ${SOCKET_HEADER_FILES} ${UTYPE_HEADER_FILES} ${UTYPE_EXT_HEADER_FILES})


IF (NOT SHARE_DEX_SOCKET)
	list (REMOVE_ITEM TARGET_SRC CDex.cpp)
ENDIF ()





ADD_LIBRARY(
	${TARGET_NAME} ${USER_DEFINED_DYNAMIC_OR_STATIC}
    ${TARGET_SRC} ${ALL_HEADER_FILES} )

IF (WIN32)

	target_link_libraries(${TARGET_NAME} ws2_32 ole32 oleaut32 Psapi Setupapi uuid)
	SET(${PLATFORM_LIBS} ws2_32 ole32 oleaut32 Psapi Setupapi uuid)
ELSEIF (UNIX)
	IF(QNX OR APPLE)
		target_link_libraries(${TARGET_NAME} pthread socket)
		SET(${PLATFORM_LIBS} pthread socket)
	ELSE()
		target_link_libraries(${TARGET_NAME} pthread dl rt)
		SET(${PLATFORM_LIBS} pthread dl rt)
	ENDIF()

ENDIF()
	
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../include ${Boost_INCLUDE_DIRS} ${RAPIDJSON_INCLUDES} ${TCLAP_INCLUDES})

install(TARGETS ${TARGET_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
                          ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
                          RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES ${CORE_HEADER_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(FILES ${SOCKET_HEADER_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/Socket)
install(FILES ${UTYPE_HEADER_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/UType)
install(FILES ${UTYPE_EXT_HEADER_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/UType/ext)

		
		
