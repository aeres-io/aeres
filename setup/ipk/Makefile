
##############################################
# OpenWrt Makefile for aeres client
#
##############################################

ifeq ($(ROOT),)
$(error "Please set ROOT variable")
endif

include $(TOPDIR)/rules.mk

AERES_VERSION_STR=$(shell cat "$(ROOT)/VERSION")

# Name and release number of this package
PKG_NAME:=aeres
PKG_VERSION:=$(AERES_VERSION_STR)
PKG_RELEASE:=1
PKG_MAINTAINER:=aeres.io
PKG_LICENSE:=MIT

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)


include $(INCLUDE_DIR)/package.mk


define Package/aeres
	SECTION:=net
	CATEGORY:=Network
	TITLE:=Aeres Client
	DEPENDS:=+libopenssl +libcurl +zlib +libstdcpp
endef


define Build/Configure
endef

define Build/Compile
endef

define Package/aeres/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_DIR) $(1)/lib
	$(INSTALL_BIN) $(ROOT)/out/bin/aeres $(1)/bin/
	$(INSTALL_BIN) $(ROOT)/src/libquic/out/lib/libquic.so $(1)/lib/
endef


$(eval $(call BuildPackage,aeres))
