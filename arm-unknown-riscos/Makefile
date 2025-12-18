# NetSurf build script for RISCOS cross compilation toolchain.

# Use a tested release version of GCCSDK
UPSTREAM_GCCSDK_VERSION := release_4_7_4_v6
UPSTREAM_GCCSDK_TARBALL := gccsdk_$(UPSTREAM_GCCSDK_VERSION).tar.gz

# Constituent upstream source archives for GCCSDK
GCCSDK_UPSTREAM_SOURCES := \
	autoconf_2.64.tar.bz2 \
	automake_1.11.1.tar.bz2 \
	cloog-ppl_0.15.11.tar.gz \
	binutils_2.24.tar.bz2 \
	gcc_4.7.4.tar.bz2 \
	gmp_5.0.1.tar.bz2 \
	libtool_2.4.2.tar.gz \
	mpc_1.1.0.tar.gz \
	mpfr_3.0.1.tar.bz2 \
	ppl_1.2.tar.xz

# OSLib 7.00
UPSTREAM_OSLIB_VERSION := 7.00
UPSTREAM_OSLIB_TARBALL := oslib_$(UPSTREAM_OSLIB_VERSION).tar.gz

# Known tested version of trunk for CCRes
UPSTREAM_CCRES_VERSION := 96
UPSTREAM_CCRES_TARBALL := ccres_svn.trunk.$(UPSTREAM_CCRES_VERSION).tar.gz

# Makerun
UPSTREAM_MAKERUN_VERSION := fe55b6bf
UPSTREAM_MAKERUN_TARBALL := makerun_git.$(UPSTREAM_MAKERUN_VERSION).tar.gz

# Squeeze
UPSTREAM_SQUEEZE_VERSION := d54d441
UPSTREAM_SQUEEZE_TARBALL := squeeze_git.$(UPSTREAM_SQUEEZE_VERSION).tar.gz

# Infozip
UPSTREAM_INFOZIP_VERSION := 3.0
UPSTREAM_INFOZIP_TARBALL := zip$(UPSTREAM_INFOZIP_VERSION).tar.gz

Q ?= @
TOP := $(CURDIR)
RECIPES := $(TOP)/recipes
SOURCESDIR := $(TOP)/sources
BUILDDIR := $(TOP)/builddir
BUILDSTEPS := $(BUILDDIR)/build-steps

TARGET_NAME := arm-unknown-riscos

PREFIX ?= /opt/netsurf/$(TARGET_NAME)

# tools
PRISTINE_TAR := $(TOP)/../tools/pristine-tar
REPACK := $(TOP)/../tools/repack

.PHONY: all clean distclean source-archives
all: $(BUILDSTEPS)/toolchain.d

source-archives: $(BUILDSTEPS)/sources.d

clean:
	rm -fr $(BUILDDIR)

distclean: clean
	rm -fr $(SOURCESDIR)

###
# Rules to build the full toolchain
###

$(BUILDSTEPS)/toolchain.d: $(BUILDSTEPS)/ccres.d $(BUILDSTEPS)/makerun.d $(BUILDSTEPS)/squeeze.d $(BUILDSTEPS)/infozip.d
	touch $@

###
# Rules to build and install Infozip
###

$(BUILDSTEPS)/infozip.d: $(BUILDSTEPS)/gcc.d $(SOURCESDIR)/$(UPSTREAM_INFOZIP_TARBALL)
	tar -C $(BUILDDIR) -xaf $(SOURCESDIR)/$(UPSTREAM_INFOZIP_TARBALL)
	for p in `ls $(RECIPES)/patches/infozip/*.p` ; do patch -d $(BUILDDIR)/zip$(subst .,,$(UPSTREAM_INFOZIP_VERSION)) -p0 <$$p ; done
	cd $(BUILDDIR)/zip$(subst .,,$(UPSTREAM_INFOZIP_VERSION)) && make -f unix/Makefile generic LOCAL_ZIP=-DFORRISCOS
	cp $(BUILDDIR)/zip$(subst .,,$(UPSTREAM_INFOZIP_VERSION))/zip $(PREFIX)/cross/bin/zip
	@# Yuck. Build host tooling should not be installed into the target environment
	mkdir -p $(PREFIX)/env/bin
	ln -fs $(PREFIX)/cross/bin/zip $(PREFIX)/env/bin/zip
	touch $@

###
# Rules to build and install Makerun
###

$(BUILDSTEPS)/makerun.d: $(BUILDSTEPS)/gcc.d $(SOURCESDIR)/$(UPSTREAM_MAKERUN_TARBALL)
	tar -C $(BUILDDIR) -xaf $(SOURCESDIR)/$(UPSTREAM_MAKERUN_TARBALL)
	cd $(BUILDDIR)/makerun && GCCSDK_INSTALL_CROSSBIN=$(PREFIX)/cross/bin make install
	touch $@

###
# Rules to build and install Squeeze
###

$(BUILDSTEPS)/squeeze.d: $(BUILDSTEPS)/gcc.d $(SOURCESDIR)/$(UPSTREAM_SQUEEZE_TARBALL)
	tar -C $(BUILDDIR) -xaf $(SOURCESDIR)/$(UPSTREAM_SQUEEZE_TARBALL)
	cd $(BUILDDIR)/squeeze && GCCSDK_INSTALL_CROSSBIN=$(PREFIX)/cross/bin make install
	touch $@

###
# Rules to build and install CCRes
###

$(BUILDSTEPS)/ccres.d: $(BUILDSTEPS)/oslib.d $(SOURCESDIR)/$(UPSTREAM_CCRES_TARBALL)
	tar -C $(BUILDDIR) -xaf $(SOURCESDIR)/$(UPSTREAM_CCRES_TARBALL)
	cd $(BUILDDIR)/ccres && GCCSDK_INSTALL_ENV=$(PREFIX)/env GCCSDK_INSTALL_CROSSBIN=$(PREFIX)/cross/bin make
	cd $(BUILDDIR)/ccres && GCCSDK_INSTALL_ENV=$(PREFIX)/env GCCSDK_INSTALL_CROSSBIN=$(PREFIX)/cross/bin make install
	touch $@

###
# Rules to build and install OSLib
###

$(BUILDSTEPS)/oslib.d: $(BUILDSTEPS)/gcc.d $(SOURCESDIR)/$(UPSTREAM_OSLIB_TARBALL)
	tar -C $(BUILDDIR) -xaf $(SOURCESDIR)/$(UPSTREAM_OSLIB_TARBALL)
	for p in `ls $(RECIPES)/patches/oslib/*.p` ; do patch -d $(BUILDDIR)/oslib-$(UPSTREAM_OSLIB_VERSION) -p0 <$$p ; done
	cd $(BUILDDIR)/oslib-$(UPSTREAM_OSLIB_VERSION) && GCCSDK_INSTALL_ENV=$(PREFIX)/env GCCSDK_INSTALL_CROSSBIN=$(PREFIX)/cross/bin make install
	touch $@

###
# Rules to build and install GCCSDK
###

$(BUILDSTEPS)/gcc.d: $(BUILDSTEPS)/gccsdk-srcdir.d
	cd $(BUILDDIR)/gccsdk/gcc4 && make
	cd $(BUILDDIR)/gccsdk/gcc4 && ./install-env
	@# Newer GCCSDK adds these trampoline binaries that for us
	@# serve no purpose and get in the way.
	$(RM) $(PREFIX)/cross/bin/arm-unknown-riscos-gcc-ar $(PREFIX)/cross/bin/arm-unknown-riscos-gcc-nm $(PREFIX)/cross/bin/arm-unknown-riscos-gcc-ranlib
	touch $@

$(BUILDSTEPS)/gccsdk-srcdir.d: $(BUILDSTEPS)/buildsteps.d $(SOURCESDIR)/$(UPSTREAM_GCCSDK_TARBALL)
	tar -C $(BUILDDIR) -xaf $(SOURCESDIR)/$(UPSTREAM_GCCSDK_TARBALL)
	cp -p $(RECIPES)/files/gcc4/gccsdk-params $(BUILDDIR)/gccsdk/gcc4/gccsdk-params
	sed -i 's#{PREFIX}#$(PREFIX)#' $(BUILDDIR)/gccsdk/gcc4/gccsdk-params
	for p in `ls $(RECIPES)/patches/gcc4/*.p` ; do patch -d $(BUILDDIR)/gccsdk -p0 <$$p ; done
	mkdir -p $(BUILDDIR)/gccsdk/gcc4/srcdir.orig
	$(foreach src,$(GCCSDK_UPSTREAM_SOURCES),cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(src) && mv $(TOP)/../$(src) $(BUILDDIR)/gccsdk/gcc4/srcdir.orig/$(subst _,-,$(src)) &&) :
	touch $@

###
# Rules to fetch upstream sources
###

$(SOURCESDIR)/$(UPSTREAM_GCCSDK_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_OSLIB_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_CCRES_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_MAKERUN_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@
	$(RM) -rf $(SOURCESDIR)/makerun

$(SOURCESDIR)/$(UPSTREAM_SQUEEZE_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@
	$(RM) -rf $(SOURCESDIR)/squeeze

$(SOURCESDIR)/$(UPSTREAM_INFOZIP_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

###
# Rule to create directories for building
###

$(BUILDSTEPS)/buildsteps.d: | $(BUILDSTEPS) $(SOURCESDIR)
	touch $@

$(BUILDSTEPS):
	mkdir -p $@

$(SOURCESDIR):
	mkdir -p $@

###
# Rules to build source archives (via source-archives target)
#
# NB: OSLib/ccres/makerun/squeeze/zip: use arm-riscos-gnueabi Makefile
###

UPSTREAM_GCCSDK_URI := svn://svn.riscos.info/gccsdk/tags/$(UPSTREAM_GCCSDK_VERSION)

$(BUILDSTEPS)/sources.d: $(BUILDSTEPS)/buildsteps.d $(BUILDDIR)/$(UPSTREAM_GCCSDK_TARBALL)
	touch $@

$(BUILDDIR)/$(UPSTREAM_GCCSDK_TARBALL):
	rm -rf $(BUILDDIR)/sources/gccsdk
	svn export $(UPSTREAM_GCCSDK_URI) $(BUILDDIR)/sources/gccsdk/
	tar -C $(BUILDDIR)/sources -caf $@ gccsdk
