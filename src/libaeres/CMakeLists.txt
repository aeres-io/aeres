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

project(libaeres-client)

set(ROOT ${PROJECT_SOURCE_DIR}/../..)

include(${ROOT}/Config.cmake)


add_library(
  aeres-client STATIC

  QuicClient.cpp
  QuicClientSession.cpp
  AsyncResult.cpp
  Base64Encoder.cpp
  AeresObject.cpp
  AeresSession.cpp
  AeresTypes.cpp
  Buffer.cpp
  HttpCookies.cpp
  HttpRequest.cpp
  Event.cpp
  AeresSystemSecurityApi.cpp
  AeresApplicationApi.cpp
  AeresUserApi.cpp
)

include_directories("../include")
include_directories("${ROOT_LIBQUIC}/src")
include_directories("${ROOT_LIBQUIC}/src/quic")
include_directories("${ROOT_LIBQUIC}/src/quic/gen")
include_directories("${ROOT_LIBQUIC}/src/quic/third_party/protobuf/src")
include_directories("${ROOT_LIBQUIC}/src/third_party/boringssl/src/include")
include_directories("${ROOT_JSONCPP}/src/include")

add_definitions(-DSSL_LIB=\"boringssl.so\")

if (DEFINED DEBUG_HTTP)
  add_definitions(-DDEBUG_HTTP)
endif (DEFINED DEBUG_HTTP)

if (DEFINED DEBUG_HTTP_RELAY)
  add_definitions(-DDEBUG_HTTP_RELAY)
endif (DEFINED DEBUG_HTTP_RELAY)

