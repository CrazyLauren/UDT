#
# toolchain-QNX-6.3.0.cmake
#
# Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
#
#  Created on: 12.11.2016
#      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
#
# Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
# https://www.mozilla.org/en-US/MPL/2.0)

set (QNX_VERSION        630)
set (QNX_VERSION_STRING 6.3.0 CACHE STRING "QNX version")
set (QNX_PLATFORM mips CACHE STRING "")
#set (ADITIONAL_FLAGS "-EL" CACHE STRING "")
set (ADITIONAL_FLAGS "-BL" CACHE STRING "")
set (QNX_COMPILER_VERSION "3.3.5" CACHE STRING "QCC version")

set (CISST_TOOLCHAIN_DIR ${CMAKE_CURRENT_LIST_DIR})

include ("${CISST_TOOLCHAIN_DIR}/toolchain-QNX-common.cmake")
