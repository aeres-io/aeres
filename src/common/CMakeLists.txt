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

cmake_minimum_required(VERSION 3.3)

project(libaeres-common)

set(ROOT ${PROJECT_SOURCE_DIR}/../..)

include(${ROOT}/Config.cmake)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(MACOSX TRUE)
else ()
  if (DEFINED UNIX)
    set(LINUX TRUE)
  endif (DEFINED UNIX)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


set(
  AERES_COMMON_SRC

  Buffer.cpp
  BufferedInputStream.cpp
  BufferedOutputStream.cpp
  BufferStream.cpp
  Config.cpp
  ConnectionFactory.cpp
  HandshakeInstruction.cpp
  IInputStream.cpp
  IOutputStream.cpp
  Log.cpp
  ScopeGuard.cpp
  SocketConnection.cpp
)


if (DEFINED MACOSX)
  set(
    AERES_COMMON_SRC ${AERES_COMMON_SRC}

    IoEventQueueMac.cpp
    SocketDispatcherPosix.cpp
  )
  add_definitions(-DOS_MACOSX)
endif (DEFINED MACOSX)

if (DEFINED LINUX)
  set(
    AERES_COMMON_SRC ${AERES_COMMON_SRC}

    IoEventQueueLinux.cpp
    SocketDispatcherPosix.cpp
  )
  add_definitions(-DOS_LINUX)
endif (DEFINED LINUX)

if (DEFINED WINDOWS)
  set(
    AERES_COMMON_SRC ${AERES_COMMON_SRC}

    SocketDispatcherWin.cpp
  )
endif (DEFINED WINDOWS)

add_library(aeres-common STATIC ${AERES_COMMON_SRC})

include_directories("../include")

