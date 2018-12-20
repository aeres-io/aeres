# Copyright 2016 FileGear - All Rights Reserved

ROOT = $(realpath ../..)

VPATH = $(ROOT)/out/bin $(ROOT)/src/libquic/out/lib

TARGET = aeres

ifeq ($(TARGET_PLATFORM),raspbian)
PKGCFG = \
	control.pi
else
PKGCFG = \
  control
endif

PKG = \
  postinst

LIB = \
  libquic.so

BIN = \
  aeres

include Setup.mk
