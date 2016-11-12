
include(CheckFunctionExists)

file(GLOB_RECURSE TARGET_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
list (APPEND TARGET_SRC    ${CMAKE_CURRENT_SOURCE_DIR}/revision.c)

IF (MSVC)
	CHECK_FUNCTION_EXISTS(getopt HAVE_getopt_FUNCTION)
	if(NOT HAVE_getopt_FUNCTION)
		list (APPEND TARGET_SRC    ${CMAKE_CURRENT_SOURCE_DIR}/getopt_msvc.c)
	endif(NOT HAVE_getopt_FUNCTION)
ENDIF()

IF (WIN32)
	
	CHECK_FUNCTION_EXISTS(getsubopt HAVE_getsubopt_FUNCTION)
	if(NOT HAVE_getsubopt_FUNCTION)
		list (APPEND TARGET_SRC    ${CMAKE_CURRENT_SOURCE_DIR}/getsubopt.c)
	endif(NOT HAVE_getsubopt_FUNCTION)
	
ENDIF()


file (GLOB CORE_HEADER_FILES ../include/*.h ../include/*.hpp)
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
	
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../include)

install(TARGETS ${TARGET_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
                          ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
                          RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES ${CORE_HEADER_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(FILES ${SOCKET_HEADER_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/Socket)
install(FILES ${UTYPE_HEADER_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/UType)
install(FILES ${UTYPE_EXT_HEADER_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/UType/ext)

		
		
