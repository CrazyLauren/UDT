set(_TARGET_NAME SHARE)

string(TOUPPER ${_TARGET_NAME}
		_TARGET_UP
		)


option(${_TARGET_UP}_NO_STATRUP_TEST "Disable  start up tests " ON)
mark_as_advanced(${_TARGET_UP}_NO_STATRUP_TEST)


file(GLOB_RECURSE TARGET_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

if (NOT HAVE_DEX)
	list (REMOVE_ITEM TARGET_SRC CDex.cpp)
endif ()

file (GLOB CORE_HEADER_FILES ../include/${_TARGET_NAME}/*.h ../include/${_TARGET_NAME}/*.hpp)
set (DEFTYPE_HEADER_FILES    ../include/deftype)


file (GLOB LOGGING_HEADER_FILES ../include/${_TARGET_NAME}/logging/*.h ../include/${_TARGET_NAME}/logging/*.hpp)
file (GLOB SOCKET_HEADER_FILES ../include/${_TARGET_NAME}/Socket/*.h ../include/${_TARGET_NAME}/Socket/*.hpp)
file (GLOB UTYPE_HEADER_FILES ../include/${_TARGET_NAME}/UType/*.h ../include/${_TARGET_NAME}/UType/*.hpp)
file (GLOB UTYPE_HEADER_FILES_IMPL ../include/${_TARGET_NAME}/UType/impl/*.h ../include/${_TARGET_NAME}/UType/impl/*.hpp)
file (GLOB UTYPE_HEADER_CONFIG ../include/${_TARGET_NAME}/config/*.h ../include/${_TARGET_NAME}/config/*.hpp)

set (SHARE_HEADERS
	 				${DEFTYPE_HEADER_FILES}
	 				${CORE_HEADER_FILES}
					${SOCKET_HEADER_FILES}
					${UTYPE_HEADER_FILES}
					${UTYPE_EXT_HEADER_FILES}
					${UTYPE_HEADER_FILES_IMPL}
					${LOGGING_HEADER_FILES}
	 				${UTYPE_HEADER_CONFIG}
	 )
set (SHARE_PUBLIC_DIR
					${CMAKE_CURRENT_SOURCE_DIR}/../include
					${Boost_INCLUDE_DIRS}
					${LOGGING_INCLUDES}
					${CMAKE_BINARY_DIR}/include
					)

set (SHARE_PRIVATE_DIR
					${RAPIDJSON_INCLUDES}
					${TCLAP_INCLUDES}
					)
set (SHARE_PRIVATE_DEFINITIONS
					BOOST_ALL_NO_LIB
					)

set (SHARE_PUBLIC_DEFINITIONS

					)


if (${${_TARGET_UP}_NO_STATRUP_TEST})
	list(APPEND SHARE_PRIVATE_DEFINITIONS NO_SHARE_TEST)
endif ()

helper_add_library(${_TARGET_UP}
					TARGET_SRC
					SHARE_HEADERS
					SHARE_PUBLIC_DIR
					SHARE_PRIVATE_DIR
					SHARE_PUBLIC_DEFINITIONS
					SHARE_PRIVATE_DEFINITIONS
					TRUE)

configure_logging(${_TARGET_NAME})
configure_version_for(${_TARGET_NAME})

if (WIN32)
	set (${_TARGET_UP}_PLATFORM_LIBS
					${${_TARGET_UP}_PLATFORM_LIBS}
					ws2_32
					ole32
					oleaut32
					Psapi
					Setupapi
					uuid
					CACHE STRING "${_TARGET_NAME} platform libraries" FORCE)

	if(LIBADVAPI32)
		if(NOT MINGW)
			list(APPEND ${_TARGET_UP}_PLATFORM_LIBS
						advapi32
						)
		endif()
	endif()

	if(HAVE_IPHLPAPI_H)
		list(APPEND ${_TARGET_UP}_PLATFORM_LIBS
					iphlpapi
			)
	endif()

elseif (UNIX)
	if(QNX)
		set(${_TARGET_UP}_PLATFORM_LIBS
					${${_TARGET_UP}_PLATFORM_LIBS}
					socket
					m
					CACHE STRING "${_TARGET_NAME} platform libraries" FORCE)
	else()
		set(${_TARGET_UP}_PLATFORM_LIBS
					${${_TARGET_UP}_PLATFORM_LIBS}
					pthread
					dl
					rt
					CACHE STRING "${_TARGET_NAME} platform libraries" FORCE)

		if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
			if(HAVE_BACKTRACE_SYMBOLS)
				set(CMAKE_SHARED_LINKER_FLAGS_DEBUG
						"${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -rdynamic"
					)
			endif()
		endif()
	endif()
endif()

if(HAVE_LIBEXECINFO)
	list(APPEND ${_TARGET_UP}_PLATFORM_LIBS
					execinfo
			)
endif()
if(HAVE_DBGHELP)
	list(APPEND ${_TARGET_UP}_PLATFORM_LIBS
					dbghelp
			)
endif()

set(_PLATFORM_LIBS ${${_TARGET_UP}_PLATFORM_LIBS})
list(REMOVE_DUPLICATES _PLATFORM_LIBS)

set(${_TARGET_UP}_PLATFORM_LIBS
			${_PLATFORM_LIBS}
			CACHE STRING "${_TARGET_NAME} platform libraries" FORCE)

set(PUBLIC_LIBRARIES
			${${_TARGET_NAME}_PLATFORM_LIBS}
			)

set(PRIVATE_LIBRARIES
			#
			)

helper_target_link_libraries(${_TARGET_NAME}
							PUBLIC_LIBRARIES
							PRIVATE_LIBRARIES
							)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(${_TARGET_NAME} PRIVATE
        -Wno-deprecated
		)
endif()

install(FILES ${DEFTYPE_HEADER_FILES} DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT headers)
install(FILES ${CORE_HEADER_FILES} DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_INCLUDEDIR}/${_TARGET_NAME}" COMPONENT headers)
install(FILES ${LOGGING_HEADER_FILES} DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_INCLUDEDIR}/${_TARGET_NAME}/logging" COMPONENT headers)
install(FILES ${SOCKET_HEADER_FILES} DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_INCLUDEDIR}/${_TARGET_NAME}/Socket" COMPONENT headers)
install(FILES ${UTYPE_HEADER_FILES} DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_INCLUDEDIR}/${_TARGET_NAME}/UType" COMPONENT headers)
install(FILES ${UTYPE_HEADER_CONFIG} DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_INCLUDEDIR}/${_TARGET_NAME}/config" COMPONENT headers)

helper_export_library(${_TARGET_NAME})

		
