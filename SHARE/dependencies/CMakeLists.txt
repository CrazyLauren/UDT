
set(LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE true CACHE BOOL "Build log4cplus from source")

if(LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE)
    if((NOT LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE_OK) AND  LOG4CPLUS_INCLUDES)
        message(WARNING "Log4cplus library is exist. See  ${LOG4CPLUS_INCLUDES}")
        if(NOT LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE_FORCE)
            option(LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE_FORCE OFF "If On checking for exist log4cplus is ignored ")
            message(FATAL_ERROR "Cannot continue build log4cplus. If You insist on continuing, Set up option LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE_FORCE as true")
            return()
        endif()
    endif()
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(BUILD_SHARED_LIBS FALSE CACHE BOOL "If TRUE, log4cplus is built as a shared library, otherwise as a static library")
    option(LOG4CPLUS_ENABLE_DECORATED_LIBRARY_NAME  "Turns on resulting file name decoration for static and UNICODE builds." OFF)

    set(config_install_dir  ${CMAKE_PACKAGE_INSTALL_DIR}  CACHE INTERNAL "" FORCE)
    add_subdirectory(log4cplus-1.2.x)

	set(LOG4CPLUS_FOUND true CACHE INTERNAL "" FORCE)
    target_include_directories (log4cplus
                                PUBLIC $<BUILD_INTERFACE:${log4cplus_SOURCE_DIR}/include>
                                        $<BUILD_INTERFACE:${log4cplus_BINARY_DIR}/include>
                                )
    target_include_directories (log4cplus
                                PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
                                )
    set(_OUT_DIRECTORY ${CMAKE_BINARY_DIR}/include/log4cplus/cmake)
    export(TARGETS log4cplus
           FILE "${_OUT_DIRECTORY}/log4cplusTargets.cmake"
           EXPORT_LINK_INTERFACE_LIBRARIES)

    configure_file(${CMAKE_MODULE_PATH_HELPER}/HelperFind.cmake
                   "${_OUT_DIRECTORY}/Findlog4cplus.cmake" @ONLY)

    install ( FILES
              "${_OUT_DIRECTORY}/Findlog4cplus.cmake"
              DESTINATION ${CMAKE_PACKAGE_INSTALL_DIR}
              )

    set(LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE_OK true CACHE INTERNAL "" FORCE)

    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(-Wno-deprecated HAVE_GCC_NO_DEPRECATED)
    if(HAVE_GCC_NO_DEPRECATED)
        set_property(TARGET log4cplus APPEND PROPERTY COMPILE_OPTIONS -Wno-deprecated)
    endif(HAVE_GCC_NO_DEPRECATED)
endif()