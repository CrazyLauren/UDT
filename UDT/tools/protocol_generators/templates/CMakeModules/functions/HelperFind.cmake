# - Find @CONF_TARGET_NAME@
# Find the @CONF_TARGET_NAME@ headers
#
#  @CONF_TARGET_NAME@_FOUND      = Set to true, if all components of @CONF_TARGET_NAME@ have been found.
#  @CONF_TARGET_NAME@_INCLUDES   = Include path for the header files of @CONF_TARGET_NAME@
#  @CONF_TARGET_NAME@_LIBRARIES  = Link these to use @CONF_TARGET_NAME@

string(TOUPPER "@CONF_TARGET_NAME@"
       _TARGET_UP)
	   
if (NOT ${_TARGET_UP}_ROOT)
    set(${_TARGET_UP}_ROOT ${CMAKE_INSTALL_PREFIX})
endif ()
	
find_path(@CONF_TARGET_NAME@_EXPORT
          NAMES @CONF_TARGET_NAME@Targets.cmake
          HINTS ${${_TARGET_UP}_ROOT} ${CMAKE_INSTALL_PREFIX}
          PATH_SUFFIXES @CMAKE_PACKAGE_INSTALL_DIR@
          )
if (@CONF_TARGET_NAME@_EXPORT)

    include("${@CONF_TARGET_NAME@_EXPORT}/@CONF_TARGET_NAME@Targets.cmake")

    set(${_TARGET_UP}_LIBRARIES
        @CONF_TARGET_NAME@
        CACHE STRING "Libraries of target @CONF_TARGET_NAME@"
        )

    get_target_property(_TARGETS_PUBLIC_HEADERS
                        @CONF_TARGET_NAME@
                        INTERFACE_INCLUDE_DIRECTORIES)

    set(${_TARGET_UP}_INCLUDES
        ${_TARGETS_PUBLIC_HEADERS}
        CACHE STRING
        "Inclludes for library  @CONF_TARGET_NAME@")

    set(@CONF_TARGET_NAME@_FOUND
        true
        CACHE BOOL
        ""
        )
endif ()

if (NOT @CONF_TARGET_NAME@_FOUND)



    find_path(${_TARGET_UP}_INCLUDES
              NAMES @CONF_LOOKING_FOR_FILES@
              HINTS ${${_TARGET_UP}_ROOT} ${CMAKE_INSTALL_PREFIX}
              PATH_SUFFIXES include
              )

    find_library(${_TARGET_UP}_LIBRARIES @CONF_TARGET_NAME@
                 HINTS ${${_TARGET_UP}_ROOT} ${CMAKE_INSTALL_PREFIX}
                 PATH_SUFFIXES lib bin
                 )

    include(FindPackageHandleStandardArgs)

    find_package_handle_standard_args(@CONF_TARGET_NAME@
                                      DEFAULT_MSG
                                      ${_TARGET_UP}_LIBRARIES
                                      ${_TARGET_UP}_INCLUDES
                                      )

    if (NOT @CONF_TARGET_NAME@_FOUND)
        if (@CONF_TARGET_NAME@_FIND_REQUIRED)
            message(FATAL_ERROR "Could not find @CONF_TARGET_NAME@! set up @CONF_TARGET_NAME@_ROOT variable")
        endif ()
    endif ()

    mark_as_advanced(
            ${_TARGET_UP}_ROOT
    )

endif ()

message(STATUS "@CONF_TARGET_NAME@_ROOT  = ${${_TARGET_UP}_ROOT}")
message(STATUS "${_TARGET_UP}_INCLUDES  = ${${_TARGET_UP}_INCLUDES}")
message(STATUS "${_TARGET_UP}_LIBRARIES = ${${_TARGET_UP}_LIBRARIES}")
mark_as_advanced(
        ${_TARGET_UP}_LIBRARIES
        ${_TARGET_UP}_INCLUDES)
