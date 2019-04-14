PROJECT(${PROJECT_NAME}_customer C CXX)		  

#add_subdirectory(src)						
SET(TARGET_NAME customer)

SET(CUSTOMER_TARGET_NAME ${TARGET_NAME} CACHE INTERNAL "" FORCE)

string(TOUPPER ${TARGET_NAME} _NAME_UP)
SET(CUSTOMER_TARGET_NAME_UP ${_NAME_UP} CACHE INTERNAL "" FORCE)

SET(CUSTOMER_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL  "Include path for the header files of customer" )


set(${CUSTOMER_TARGET_NAME_UP}_WITH_STATIC_MODULES true CACHE BOOL "Build ${TARGET_NAME} with 'static loaded modules' ")

#build library
add_definitions(${LOGGING_DEFENITIONS})

configure_version(${TARGET_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src 0 3)

file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)			
list (APPEND SOURCE_FILES    ${CMAKE_CURRENT_SOURCE_DIR}/src/revision.c)

file (GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include ${SHARE_INCLUDES} ${Boost_INCLUDE_DIRS} ${TCLAP_INCLUDES} ${UDT_SHARE_INCLUDES} ${LOGGING_INCLUDES})


#adding object files of modules to project
SET(${CUSTOMER_TARGET_NAME_UP}_LIST_STATIC_MODULES "sm_main_channel;tcp_client_io_manager"  CACHE STRING "list of modules which will linked static")

if(${${CUSTOMER_TARGET_NAME_UP}_WITH_STATIC_MODULES})
	add_definitions(-D${CUSTOMER_TARGET_NAME_UP}_WITH_STATIC_MODULES)
	
	foreach(_source ${${CUSTOMER_TARGET_NAME_UP}_LIST_STATIC_MODULES})
		list(APPEND SOURCE_FILES "$<TARGET_OBJECTS:${_source}_Object>")
		string(TOUPPER ${_source} _LIB_NAME_UP)
		list(APPEND DEF_MODULES ${_LIB_NAME_UP}_STATIC)
	endforeach()
endif()

share_add_library(${TARGET_NAME} FALSE SOURCE_FILES HEADER_FILES TRUE)

share_target_link_libraries(${TARGET_NAME} ${PLATFORM_LIBS}   )

share_add_dependency(${TARGET_NAME} ${SHARE_LIBRARIES}  ${UDT_SHARE_LIBRARIES})

#adding "static" defines for modules
set_property(TARGET ${TARGET_NAME} APPEND PROPERTY COMPILE_DEFINITIONS ${DEF_MODULES})

#configure deps
#if(${${CUSTOMER_TARGET_NAME_UP}_WITH_STATIC_DEPENDENCIES})
SET(CUSTOMER_LIBRARIES  ${TARGET_NAME} CACHE INTERNAL "CUSTOMER_LIBRARIES" FORCE)#share and udt_share is not need as it's static 
#else()	
#	SET(CUSTOMER_LIBRARIES  ${SHARE_LIBRARIES} ${UDT_SHARE_LIBRARIES} ${TARGET_NAME}  CACHE STRING "CUSTOMER_LIBRARIES" FORCE)
#endif()

						  
install(FILES 	${CMAKE_CURRENT_SOURCE_DIR}/include/CCustomer.h
				${CMAKE_CURRENT_SOURCE_DIR}/include/customer.h
				${CMAKE_CURRENT_SOURCE_DIR}/include/customer_export.h
		DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
		
SET(${CUSTOMER_TARGET_NAME_UP}_AVAILABLE_MODULES "" CACHE STRING "list of available modules" FORCE)	


#build modules


add_subdirectory(modules)

#SET(${CUSTOMER_TARGET_NAME_UP}_LIST_STATIC_MODULES ${${CUSTOMER_TARGET_NAME_UP}_AVAILABLE_MODULES}  CACHE STRING "list of modules which will linked static")

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/default_customer_config.xml 
		DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)	