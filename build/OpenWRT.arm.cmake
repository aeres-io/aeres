#
# MIT License
#
# Copyright (c) 2018 aeres.io
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# =============================================================================
#

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

if (NOT DEFINED ENV{OPENWRT_TOOLCHAIN})
  message(FATAL_ERROR, "Please define OPENWRT_TOOLCHAIN in environment variable.")
else()
  set (TOOLCHAIN $ENV{OPENWRT_TOOLCHAIN})
endif (NOT DEFINED ENV{OPENWRT_TOOLCHAIN})

if (NOT DEFINED ENV{OPENWRT_SYSROOT})
  message(FATAL_ERROR, "Please define OPENWRT_SYSROOT in environment variable.")
else()
  set(SYSROOT $ENV{OPENWRT_SYSROOT})
endif (NOT DEFINED ENV{OPENWRT_SYSROOT})

if (NOT DEFINED ENV{STAGING_DIR})
  message(FATAL_ERROR, "Please define STAGING_DIR in envrionment variable.")
endif (NOT DEFINED ENV{STAGING_DIR})

set(CMAKE_STAGING_PREFIX $ENV{STAGING_DIR})

if (DEFINED ENV{OPENWRT_COMPILER_PREFIX})
  set(OPENWRT_COMPILER_PREFIX $ENV{OPENWRT_COMPILER_PREFIX})
else()
  set(OPENWRT_COMPILER_PREFIX "arm-openwrt-linux-muslgnueabi-")
endif (DEFINED ENV{OPENWRT_COMPILER_PREFIX})

set(CMAKE_C_COMPILER ${TOOLCHAIN}/bin/${OPENWRT_COMPILER_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN}/bin/${OPENWRT_COMPILER_PREFIX}g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

