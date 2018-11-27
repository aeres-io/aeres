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

project(aeres)

set(ROOT ${PROJECT_SOURCE_DIR}/../..)

include(${ROOT}/Config.cmake)

link_directories(${ROOT_LIBQUIC}/out/lib)


add_executable(
  aeres

  Main.cpp
  Options.cpp
  Config.cpp
  AeresApplicationCli.cpp
  AeresEndpointCli.cpp
  AeresRuleCli.cpp
  AeresListener.cpp
  AeresTunnel.cpp
)

include_directories("../include")
include_directories("../libaeres")
include_directories("${ROOT_LIBQUIC}/src")
include_directories("${ROOT_LIBQUIC}/src/quic")
include_directories("${ROOT_LIBQUIC}/src/quic/gen")
include_directories("${ROOT_LIBQUIC}/src/quic/third_party/protobuf/src")
include_directories("${ROOT_JSONCPP}/src/include")

aeres_lib(aeres libaeres-client ${LIBDIR}/libaeres-client.a)
aeres_lib(aeres libaeres-quic ${LIBDIR}/libaeres-quic.a)
aeres_lib(aeres libaeres-common ${LIBDIR}/libaeres-common.a)
aeres_lib(aeres jsoncpp ${ROOT_JSONCPP}/out/lib/libjsoncpp.a)

if (DEFINED APPLE) #DARWIN
  if ((NOT DEFINED OPENSSL_INCLUDE_DIR) AND (NOT DEFINED OPENSSL_LIBRARIES))
    find_package(OPENSSL REQUIRED)
    if (NOT OPENSSL_FOUND)
      message(FATAL_ERROR "openssl not found. Please define OPENSSL_INCLUDE_DIR and OPENSSL_LIBRARIES.")
    else()
      include_directories(${OPENSSL_INCLUDE_DIR})
      link_directories(${OPENSSL_LIBRARIES})
    endif (NOT OPENSSL_FOUND)
  endif ((NOT DEFINED OPENSSL_INCLUDE_DIR) AND (NOT DEFINED OPENSSL_LIBRARIES))
endif (DEFINED APPLE) #DARWIN

if (NOT DEFINED OPENSSL_LIBRARIES)
  set(OPENSSL_LIBRARIES ssl)
endif (NOT DEFINED OPENSSL_LIBRARIES)


aeres_sys_lib(aeres curl)
aeres_sys_lib(aeres ${OPENSSL_LIBRARIES})
aeres_sys_lib(aeres quic)
aeres_sys_lib(aeres dl)
aeres_sys_lib(aeres z)
aeres_sys_lib(aeres readline)

aeres_use_pthread(aeres)
