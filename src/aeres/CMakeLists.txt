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

cmake_minimum_required(VERSION 3.1)

project(aeres)

set(ROOT ${PROJECT_SOURCE_DIR}/../..)

include(${ROOT}/Config.cmake)

link_directories(${ROOT_LIBQUIC}/out/lib)


add_executable(
  aeres

  Main.cpp
  Options.cpp
  AeresSession.cpp
  AeresApplication.cpp
  AeresEndpoint.cpp
  AeresRule.cpp
  AeresListener.cpp
  AeresTunnel.cpp
)

include_directories("../include")
include_directories("${ROOT_LIBQUIC}/src")
include_directories("${ROOT_LIBQUIC}/src/quic")
include_directories("${ROOT_LIBQUIC}/src/quic/gen")
include_directories("${ROOT_LIBQUIC}/src/quic/third_party/protobuf/src")
include_directories("${ROOT_LIBQUIC}/src/third_party/boringssl/src/include")

add_definitions(-DSSL_LIB=\"boringssl.so\")

aeres_lib(aeres libaeres-client ${LIBDIR}/libaeres-client.a)
aeres_lib(aeres libaeres-quic ${LIBDIR}/libaeres-quic.a)
aeres_lib(aeres libaeres-common ${LIBDIR}/libaeres-common.a)

aeres_sys_lib(aeres quic)
aeres_sys_lib(aeres boringssl)
aeres_sys_lib(aeres dl)
aeres_sys_lib(aeres z)

aeres_use_pthread(aeres)