# OpenWRT Build Instructions

This document illustrates the steps to build aeres for OpenWRT (ARM) using cross-compile toolchain from Ubuntu 18.04 x64. Similar steps should be available for other x86/x64 Linux redistributions.

## Prerequisite

 - OpenWRT SDK which contains GCC that supports C++14.
 - CMake v3.3+
 - GNU Make

OpenWRT SDK can be downloaded from [https://downloads.openwrt.org/releases/18.06.1/targets/](https://downloads.openwrt.org/releases/18.06.1/targets/).

The following instructions uses `armvirt` as an example to create the build for ARM.

In addition, the following packages need to be installed to use the SDK and the source repository:

```bash
$ sudo apt-get install git cmake make subversion libncurses5-dev zlib1g-dev gawk flex quilt libssl-dev xsltproc libxml-parser-perl mercurial bzr ecj cvs unzip
```

## Build Environment

The downloaded OpenWRT SDK does not yet contain any required dependency packages by default. Following the steps below to create the build environment.

More details about how to use the SDK can be found at [https://openwrt.org/docs/guide-developer/using_the_sdk](https://openwrt.org/docs/guide-developer/using_the_sdk).

Assume the OpenWRT SDK is downloaded and extracted at `${sdk}`, the target architecture is `${arch}` (e.g. `arm_cortex-a15+neon-vfpv4`), and the version of gcc is `${gccver}` (e.g. `7.4.0`).

1. Set the following required environment variables to create the build environment.

    ```bash
    $ export STAGING_DIR=${sdk}/staging_dir
    $ export PATH=${STAGING_DIR}/toolchain-${arch}_gcc-${gccver}_musl_eabi/bin

    # The following two lines are required on Ubuntu 18.04
    $ export LC_ALL=C
    $ unset LANG
    ```

2. Update and install feeds for dependency packages.

    ```bash
    $ cd ${sdk}
    $ ./scripts/feeds update -a
    $ ./scripts/feeds install libopenssl libcurl zlib libstdcpp
    ```

3. Run `make menuconfig` at `${sdk}`. Make sure the following options are set.

    - In `Global build settings`, unselect all options. This can dramatically reduce the build time since we just need to build a user mode application.
    - Select `libopenssl` in `Library/SSL`.
    - Select `libcurl` and `zlib` in `Library`.
    - Save and exit.

4. Run `make` to download and build the dependency packages. This may take a few minute to complete. Run `make V=s` to see verbose logs in case something goes wrong.

## Build Steps

### Clone the source code

The aeres repository contains a few submodules that are required to build aeres. Make sure all submodules are synced before starting a build.

Run the following command to sync to the latest code:

```bash
$ git submodule init
$ git submodule update
```

### Set Required Environment Variables

The following environment variables are required.

```bash
$ export OPENWRT_TOOLCHAIN=${sdk}/staging_dir/toolchain-${arch}_gcc-${gccver}
$ export OPENWRT_SYSROOT=${sdk}/staging_dir/target-${arch}_musl_eabi

# Note - STAGING_DIR is different from the value needed to create the SDK environment above
$ export STAGING_DIR=${OPENWRT_SYSROOT}

# The following two environment variables are required on Ubuntu 18.04
$ export LC_ALL=C
$ unset LANG
```

### Create the build

Run the following command at aeres folder to create the build on OpenWRT:

```bash
$ mkdir cmake.out
$ cd cmake.out
$ cmake -DCMAKE_TOOLCHAIN_FILE=../build/OpenWRT.arm.cmake -DTARGET_ARCH_ABI=armeabi -DNO_READLINE=1 .
$ make
```

After the build completes successfully, the `aeres` executable will be located at `out/bin`, and `src/libquic/out/lib/libquic.so` will also be needed to run the executable from a different path or machine.

### Generate installation package

Scripts have been provided to generate installation package for OpenWRT.

Assume aeres repository is cloned at `${aeres}`. Run the following commands to create the `.ipk` package.

```bash
$ cd ${sdk}
$ mkdir -p ${sdk}/packages/aeres
$ cp ${aeres}/setup/ipk/Makefile ${sdk}/packages/aeres/
$ make menuconfig

# Select Network/aeres, and then save and exit.

$ make ROOT=${aeres}
```

This will generate the `.ipk` package at `${sdk}/bin/packages/${arch}/base/aeres_${version}_${arch}.ipk`. This package can then be copied to the device and installed using `opkg install ${pkg}`.

