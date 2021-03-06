#
# toolchain-QNX-6.3.0.cmake
#
# Copyright © 2016  https://github.com/CrazyLauren
#
#  Created on: 12.11.2016
#      Author:  https://github.com/CrazyLauren
#
# Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
# https://www.mozilla.org/en-US/MPL/2.0)

set (QNX_VERSION        630)
set (QNX_VERSION_STRING 6.3.0 CACHE STRING "QNX version")
set (QNX_PLATFORM arm CACHE STRING "")
set (ADITIONAL_FLAGS "-EL -D_LITTLE_ENDIAN" CACHE STRING "")
#set (ADITIONAL_FLAGS "-EB -D_BIG_ENDIAN" CACHE STRING "")
set (QNX_COMPILER_VERSION "3.3.5" CACHE STRING "QCC version")

set (SHARE_TOOLCHAIN_DIR ${CMAKE_CURRENT_LIST_DIR})

include ("${SHARE_TOOLCHAIN_DIR}/toolchain-QNX-common.cmake")
