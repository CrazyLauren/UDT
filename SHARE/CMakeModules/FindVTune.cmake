#
# SHAREMacros.cmake
#
# Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
#
#  Created on: 10.12.2016
#      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
#
# Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
# https://www.mozilla.org/en-US/MPL/2.0)
# 
set(dirs
  "C:/Program Files (x86)/Intel/VTune Amplifier XE/"
  "$ENV{VTUNE_AMPLIFIER_XE_2016_DIR}/"
  "C:/Program Files (x86)/Intel/VTune Amplifier XE 2016/"
  "$ENV{VTUNE_AMPLIFIER_XE_2015_DIR}/"
  "C:/Program Files (x86)/Intel/VTune Amplifier XE 2015/"
  "$ENV{VTUNE_AMPLIFIER_XE_2013_DIR}/"
  "C:/Program Files (x86)/Intel/VTune Amplifier XE 2013/"
  "$ENV{VTUNE_AMPLIFIER_XE_2011_DIR}/"
  "C:/Program Files (x86)/Intel/VTune Amplifier XE 2011/"
  )
  
find_path(VTune_INCLUDE_DIRS ittnotify.h
    PATHS ${dirs}
    PATH_SUFFIXES include)
	
if (CMAKE_SIZEOF_VOID_P MATCHES "8")
  set(vtune_lib_dir lib64)
else()
  set(vtune_lib_dir lib32)
endif()

find_library(VTune_LIBRARIES libittnotify
    HINTS "${VTune_INCLUDE_DIRS}/.."
    PATHS ${dirs}
    PATH_SUFFIXES ${vtune_lib_dir})
include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    VTune DEFAULT_MSG VTune_LIBRARIES VTune_INCLUDE_DIRS)
	
if(UNIX)
  list(APPEND VTUNE_LIBRARIES dl)
endif()

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DINTEL_NO_ITTNOTIFY_API")
set(CMAKE_CXX_FLAGS_RELEASE
  "${CMAKE_CXX_FLAGS_RELEASE} -DINTEL_NO_ITTNOTIFY_API")
  
	