# Raspberry Pi Build Instructions

This document illustrates the steps to build aeres on Raspberry Pi.

## Prerequisite

 - C++ compiler that supports C++14.
 - CMake v3.3+
 - OpenSSL
 - libcurl
 - readline
 - zlib
 - nss
 - nspr

For example, the dependencies can be installed using the following command on Raspbian Stretch (Lite):

```bash
$ sudo apt-get install git gcc g++ cmake pkg-config libssl-dev libcurl4-openssl-dev zlib1g-dev libreadline-dev libnss3-dev libnspr4-dev
```

*Unfortunately, the official cross compilation toolchain from [Raspberry Pi tools](https://github.com/raspberrypi/tools) only provides an older version of GCC (4.8) which does not support C++14. To build AERES from an x86 PC, QEMU with raspbian image, [architectural chroot](http://sentryytech.blogspot.com/2013/02/faster-compiling-on-emulated-raspberry.html), or custom cross compilation toolchain will be required.*

## Build Steps

### Clone the source code

The aeres repository contains a few submodules that are required to build aeres. Make sure all submodules are synced before starting a build.

Run the following command to sync to the latest code:

```bash
$ git submodule init
$ git submodule update
```

### Create the build

Run the following command to create the build on Raspberry Pi:

```bash
$ mkdir cmake.out
$ cd cmake.out
$ cmake -DTARGET_ARCH_ABI=armeabi ..
$ make
```

After the build completes successfully, the `aeres` executable will be located at `out/bin`. `src/libquic/out/lib/libquic.so` will also be needed to run the executable from a different path or machine.

### Generate installation package

Scripts have been provided to generate installation package for Raspberry Pi.

The following command can be used to create the `.deb` package:

```bash
$ cd setup/deb
$ make TARGET_PLATFORM=raspbian
```

The output deb package will be located at `out`.
