# Linux Build Instructions

This document illustrates the steps to build aeres on Linux.

## Prerequisite

 - C++ compiler that supports C++14.
 - CMake v3.3+
 - OpenSSL
 - libcurl
 - readline
 - zlib
 - nss
 - nspr

For example, the dependencies can be installed using the following command on Ubuntu 18:

```bash
$ sudo apt-get install gcc cmake libssl-dev libcurl4-openssl-devl zlib1g-dev libreadline-dev libnss3-dev libnspr4-dev
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

Run the following command to create the build on linux:

```bash
$ mkdir cmake.out
$ cd cmake.out
$ cmake ..
$ make
```

After the build completes successfully, the `aeres` executable will be located at `out/bin`. `src/libquic/out/lib/libquic.so` will also be needed to run the executable from a different path or machine.

### Generate installation package

Scripts have been provided to generate installation package on a few Linux redistributions.

#### Ubuntu

*Installation packages have been verified on Ubuntu 16.04 and Ubuntu 18.04*

The following command can be used to create the `.deb` package on Ubuntu:

```bash
$ cd setup/deb
$ make
```

The output deb package will be located at `out`.