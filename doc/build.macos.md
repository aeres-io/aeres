# macOS Build Instructions

This document illustrates the steps to build aeres on macOS.

## Prerequisite

 - C++ compiler that supports C++14.
 - libcurl
 - zlib
 - CMake v3.3+
 - OpenSSL
 - readline
 - nss
 - nspr

C++ compiler, libcurl and zlib can be obtained by installing XCode from App Store.

The easiest way to get other dependencies is using [brew](https://brew.sh/). For example, the dependencies can be installed using the following command after getting c++ support from XCode:

```bash
$ brew install cmake pkg-config openssl readline nss nspr
```

## Build Steps

### Clone the source code

The aeres repository contains a few submodules that are required to build aeres. Make sure all submodules are synced before starting a build.

Run the following command to sync to the latest code:

```bash
$ git submodule init
$ git submodule update
```

### Create the build

Run the following command to generate the build script first on macOS assuming the dependency packages are installed using brew and the version of OpenSSL installed is 1.0.2p and NSS installed is 3.40. *Replace the version number in the path below with the actual version installed.*

```bash
$ mkdir cmake.out
$ cd cmake.out
$ export PKG_CONFIG_PATH=/usr/local/Cellar/nss/3.40/lib/pkgconfig:/usr/local/Cellar/openssl/1.0.2p/lib/pkgconfig
$ cmake ..
$ make
```

After the build completes successfully, the `aeres` executable will be located at `out/bin`. `src/libquic/out/lib/libquic.dylib` will also be needed to run the executable from a different path or machine.
