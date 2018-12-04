# Copyright 2016 FileGear - All Rights Reserved

ifeq ($(ROOT),)
$(error "Please set ROOT variable")
endif

ifeq ($(PKGCFG),)
$(error "Missing PKGCFG value")
endif

ifeq ($(TARGET),)
$(error "Missing TARGET value")
endif

SRCROOT = $(ROOT)/src
OUTDIR = $(ROOT)/out

AERES_VERSION_MAJOR = 1
AERES_VERSION_MINOR = 0
AERES_VERSION_BUILD = 2

LINUX_CODENAME=$(shell lsb_release -c -s)
LINUX_RELEASE=$(shell lsb_release -r -s)
LINUX_DISTRO=$(shell lsb_release -i -s | tr "[:upper:]" "[:lower:]")
MACHINE_ARCH=$(shell uname -m)

PACKAGE_VERSION ?= $(AERES_VERSION_MAJOR).$(AERES_VERSION_MINOR)-$(AERES_VERSION_BUILD)

ifeq ($(OBJTGT),)
OBJTGT = $(OUTDIR)/$(TARGET)_$(PACKAGE_VERSION)_$(LINUX_DISTRO)_$(LINUX_RELEASE)_$(MACHINE_ARCH)
endif

TARGETDEB = $(OBJTGT).deb

PKG_ = $(patsubst %,$(OBJTGT)/DEBIAN/%,$(PKG))
PKGCFG_ = $(patsubst %,$(OBJTGT)/DEBIAN/control,$(PKGCFG))
BIN_ = $(patsubst %,$(OBJTGT)/usr/bin/%,$(BIN))
LIB_ = $(patsubst %,$(OBJTGT)/usr/lib/%,$(LIB))
LIBPRI_ = $(patsubst %,$(OBJTGT)/usr/lib/aeres/%,$(LIBPRI))
CFG_ = $(patsubst %,$(OBJTGT)/etc/aeres/%,$(CFG))
NGX_ = $(patsubst %,$(OBJTGT)/etc/nginx/sites-enabled/%,$(NGX))
HTML_ = $(patsubst %,$(OBJTGT)/var/aeres/html/%,$(HTML))
DATA_ = $(patsubst %,$(OBJTGT)/var/aeres/data/%,$(DATA))
SQL_ = $(patsubst %,$(OBJTGT)/var/aeres/sql/%,$(SQL))
SVC_ = $(patsubst %,$(OBJTGT)/lib/systemd/system/%,$(SVC))
SCFG_ = $(patsubst %,$(OBJTGT)/etc/systemd/%,$(SCFG))
UDEV_ = $(patsubst %,$(OBJTGT)/etc/udev/rules.d/%,$(UDEV))
DHEN_ = $(patsubst %,$(OBJTGT)/etc/dhcp/dhclient-enter-hooks.d/%,$(DHEN))
DHEX_ = $(patsubst %,$(OBJTGT)/etc/dhcp/dhclient-exit-hooks.d/%,$(DHEX))

.PHONY: all clean $(DIRS) $(PREBUILD) $(POSTCLEAN)

all: $(DIRS) $(TARGETDEB)

$(TARGETDEB): $(PKGCFG_) $(PKG_) $(BIN_) $(LIB_) $(LIBPRI_) $(NGX_) $(CFG_) $(SQL_) $(HTML_) $(DATA_) $(SVC_) $(SCFG_) $(UDEV_) $(DHEN_) $(DHEX_)
	@dpkg --build $(OBJTGT)

clean: $(DIRS) $(POSTCLEAN)
	@rm -rf $(OBJTGT)

$(DIRS):
	@$(MAKE) -C $@ $(MAKECMDGOALS)

$(PKGCFG_): $(PKGCFG); @mkdir -p $(dir $@); sed "s/PACKAGE_VERSION/$(PACKAGE_VERSION)/g" $< > $@
$(PKG_): $(OBJTGT)/DEBIAN/%: %; @echo "copy: $< => $@"; mkdir -p $(dir $@); cp -f $< $@
$(BIN_): $(OBJTGT)/usr/bin/%: %; @echo "copy: $< => $@"; mkdir -p $(dir $@); cp -f $< $@
$(LIB_): $(OBJTGT)/usr/lib/%: %; @echo "copy: $< => $@"; mkdir -p $(dir $@); cp -f $< $@
$(LIBPRI_): $(OBJTGT)/usr/lib/aeres/%: %; @echo "copy: $< => $@"; mkdir -p $(dir $@); cp -f $< $@
$(CFG_): $(OBJTGT)/etc/aeres/%: %; @echo "copy: $< => $@"; mkdir -p $(dir $@); cp -f $< $@
$(NGX_): $(OBJTGT)/etc/nginx/sites-enabled/%: %; @echo "copy: $< => $@"; mkdir -p $(dir $@); cp -f $< $@
$(SQL_): $(OBJTGT)/var/aeres/sql/%: %; @echo "copy: $< => $@"; mkdir -p $(dir $@); cp -f $< $@
$(HTML_): $(OBJTGT)/var/aeres/html/%: %; @echo "copy: $< => $@"; mkdir -p $(dir $@); cp -f $< $@
$(DATA_): $(OBJTGT)/var/aeres/data/%: %; @echo "copy: $< => $@"; mkdir -p $(dir $@); cp -f $< $@
$(SVC_): $(OBJTGT)/lib/systemd/system/%: %; @echo "copy: $< => $@"; mkdir -p $(dir $@); cp -f $< $@
$(SCFG_): $(OBJTGT)/etc/systemd/%: %; @echo "copy: $< => $@"; mkdir -p $(dir $@); cp -f $< $@
$(UDEV_): $(OBJTGT)/etc/udev/rules.d/%: %; @echo "copy: $< => $@"; mkdir -p $(dir $@); cp -f $< $@
$(DHEN_): $(OBJTGT)/etc/dhcp/dhclient-enter-hooks.d/%: %; @echo "copy: $< => $@"; mkdir -p $(dir $@); cp -f $< $@
$(DHEX_): $(OBJTGT)/etc/dhcp/dhclient-exit-hooks.d/%: %; @echo "copy: $< => $@"; mkdir -p $(dir $@); cp -f $< $@
