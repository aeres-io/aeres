# Copyright 2018 - aeres.io - All Rights Reserved

ifeq ($(ROOT),)
$(error "Please set ROOT variable")
endif

ifeq ($(TARGET),)
$(error "Missing TARGET value")
endif

SRCROOT = $(ROOT)/src
OUTDIR = $(ROOT)/out
OBJDIR = $(OUTDIR)/rpm/$(TARGET)

AERES_VERSION_STR=$(shell cat "$(ROOT)/VERSION")
AERES_VERSION_COMPONENTS=$(subst ., ,$(AERES_VERSION_STR))

AERES_VERSION_MAJOR=$(word 1, $(AERES_VERSION_COMPONENTS))
AERES_VERSION_MINOR=$(word 2, $(AERES_VERSION_COMPONENTS))
AERES_VERSION_BUILD=$(word 3, $(AERES_VERSION_COMPONENTS))

AERES_VERSION=$(AERES_VERSION_MAJOR).$(AERES_VERSION_MINOR)

BUILDARCH ?= $(shell uname -m)
BUILDHOST ?= $(shell uname -m)
THISBUILDTYPE ?= $(BUILDTYPE)

PACKAGE = $(TARGET)-$(AERES_VERSION)
RPMNAME = $(PACKAGE)-$(AERES_VERSION_BUILD).$(BUILDARCH).rpm

OBJTGT = $(OBJDIR)/$(TARGET)
RPMSDIR = $(OBJTGT)/RPMS
SPECSDIR = $(OBJTGT)/SPECS
SRPMSDIR = $(OBJTGT)/SRPMS
BUILDDIR = $(OBJTGT)/BUILD
SOURCESDIR = $(OBJTGT)/SOURCES
PACKAGEDIR = $(SOURCESDIR)/$(PACKAGE)
RESOURCESDIR = $(PACKAGEDIR)/res

RPM_ = $(OUTDIR)/$(RPMNAME)

SPECS_ = $(patsubst %,$(SPECSDIR)/%,$(SPECS))

SOURCES_ = $(patsubst %,$(PACKAGEDIR)/%,$(SOURCES))

SOURCESTGZ_ = $(patsubst %,$(SOURCESDIR)/%.tar.gz,$(PACKAGE))

.PHONY: all mkdir clean $(PREBUILD)

all: mkdir $(PREBUILD) $(RPM_)

$(RPM_): $(PREBUILD) $(SOURCESTGZ_) $(SPECS_)
	@echo "rpmbuild: $@"
	@export RPMDIR=$(OBJTGT);export RPMNAME=$(TARGET);export RPMVER=$(AERES_VERSION);export RPMREL=$(AERES_VERSION_BUILD);export RPMARCH=$(BUILDARCH);export RPMVERMAJOR=$(AERES_VERSION_MAJOR);export RPMVERMINOR=$(AERES_VERSION_MINOR);rpmbuild --target=$(BUILDARCH) -v -bb --clean $(SPECS_)
	@mv -f $(OBJTGT)/RPMS/$(BUILDARCH)/$(RPMNAME) $@

$(SOURCESTGZ_): $(SOURCES_)
	@echo "tgz: $@"
	@pushd $(SOURCESDIR) > /dev/null; tar -czf $@ $(PACKAGE); popd > /dev/null

$(SOURCES_): $(PACKAGEDIR)/%: %
	@echo "copy: $@"
	@mkdir -p $$(dirname $@)
	@cp -f $< $@

$(SPECS_): $(SPECSDIR)/%: %
	@echo "copy: $@"
	@cp -f $< $@

clean:
	@for d in $(RPM_); do ( rm -f $$d ); done
	@for d in $(SPECS_); do ( rm -f $$d ); done
	@for d in $(SOURCES_); do ( rm -f $$d ); done
	@for d in $(RESOURCES_); do ( rm -f $$d ); done

mkdir:
	@mkdir -p $(OBJTGT)
	@mkdir -p $(OBJTGT)/RPMS
	@mkdir -p $(OBJTGT)/SPECS
	@mkdir -p $(RESOURCESDIR)
	@mkdir -p $(OBJTGT)/SRPMS
	@mkdir -p $(OBJTGT)/BUILD
	@for d in $(LANGUAGE_PACKS); do ( mkdir -p $(PACKAGEDIR)/$$d ); done

