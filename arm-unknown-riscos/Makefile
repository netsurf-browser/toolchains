# Use a tested trunk version of GCCSDK
UPSTREAM_GCCSDK_VERSION := release_4_7_4_v2
UPSTREAM_GCCSDK_TARBALL := gcc4$(UPSTREAM_GCCSDK_VERSION).tar.gz
UPSTREAM_GCCSDK_URI := svn://svn.riscos.info/gccsdk/tags/$(UPSTREAM_GCCSDK_VERSION)

# OSLib 7.00
UPSTREAM_OSLIB_VERSION := oslib-700
UPSTREAM_OSLIB_TARBALL := oslib
UPSTREAM_OSLIB_URI := https://svn.code.sf.net/p/ro-oslib/code/tags/$(UPSTREAM_OSLIB_VERSION)/!OSLib

# Known tested version of trunk for CCRes
UPSTREAM_CCRES_VERSION := 96
UPSTREAM_CCRES_TARBALL := ccres
UPSTREAM_CCRES_URI := svn://svn.riscos.info/ccres/trunk@$(UPSTREAM_CCRES_VERSION)

# Makerun
UPSTREAM_MAKERUN_VERSION := unused
UPSTREAM_MAKERUN_TARBALL := makerun
UPSTREAM_MAKERUN_URI := git://git.netsurf-browser.org/makerun.git

# Squeeze
UPSTREAM_SQUEEZE_VERSION := unused
UPSTREAM_SQUEEZE_TARBALL := squeeze
UPSTREAM_SQUEEZE_URI := git://git.netsurf-browser.org/squeeze.git

# Infozip
UPSTREAM_INFOZIP_VERSION := 30
UPSTREAM_INFOZIP_TARBALL := zip$(UPSTREAM_INFOZIP_VERSION).tar.gz
UPSTREAM_INFOZIP_URI := http://downloads.sourceforge.net/project/infozip/Zip%203.x%20%28latest%29/3.0/$(UPSTREAM_INFOZIP_TARBALL)

TOP := $(CURDIR)
RECIPES := $(TOP)/recipes
SOURCESDIR := $(TOP)/sources
BUILDDIR := $(TOP)/builddir
BUILDSTEPS := $(BUILDDIR)/build-steps

TARGET_NAME := arm-unknown-riscos

PREFIX ?= /opt/netsurf/$(TARGET_NAME)

# tools
FETCHSRC:=$(CURDIR)/fetchsrc

.PHONY: all clean distclean
all: $(BUILDSTEPS)/toolchain.d

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
	for p in `ls $(RECIPES)/patches/infozip/*.p` ; do patch -d $(BUILDDIR)/zip$(UPSTREAM_INFOZIP_VERSION) -p0 <$$p ; done
	cd $(BUILDDIR)/zip$(UPSTREAM_INFOZIP_VERSION) && make -f unix/Makefile generic LOCAL_ZIP=-DFORRISCOS
	cp $(BUILDDIR)/zip$(UPSTREAM_INFOZIP_VERSION)/zip $(PREFIX)/cross/bin/zip
	@# Yuck. Build host tooling should not be installed into the target environment
	mkdir -p $(PREFIX)/env/bin
	ln -fs $(PREFIX)/cross/bin/zip $(PREFIX)/env/bin/zip
	touch $@

###
# Rules to build and install Makerun
###

$(BUILDSTEPS)/makerun.d: $(BUILDSTEPS)/gcc.d $(SOURCESDIR)/$(UPSTREAM_MAKERUN_TARBALL)
	cp -r $(SOURCESDIR)/$(UPSTREAM_MAKERUN_TARBALL) $(BUILDDIR)/makerun
	cd $(BUILDDIR)/makerun && GCCSDK_INSTALL_CROSSBIN=$(PREFIX)/cross/bin make install
	touch $@

###
# Rules to build and install Squeeze
###

$(BUILDSTEPS)/squeeze.d: $(BUILDSTEPS)/gcc.d $(SOURCESDIR)/$(UPSTREAM_SQUEEZE_TARBALL)
	cp -r $(SOURCESDIR)/$(UPSTREAM_SQUEEZE_TARBALL) $(BUILDDIR)/squeeze
	cd $(BUILDDIR)/squeeze && GCCSDK_INSTALL_CROSSBIN=$(PREFIX)/cross/bin make install
	touch $@

###
# Rules to build and install CCRes
###

$(BUILDSTEPS)/ccres.d: $(BUILDSTEPS)/oslib.d $(SOURCESDIR)/$(UPSTREAM_CCRES_TARBALL)
	cp -r $(SOURCESDIR)/$(UPSTREAM_CCRES_TARBALL) $(BUILDDIR)/ccres
	cd $(BUILDDIR)/ccres && GCCSDK_INSTALL_ENV=$(PREFIX)/env GCCSDK_INSTALL_CROSSBIN=$(PREFIX)/cross/bin make
	cd $(BUILDDIR)/ccres && GCCSDK_INSTALL_ENV=$(PREFIX)/env GCCSDK_INSTALL_CROSSBIN=$(PREFIX)/cross/bin make install
	touch $@

###
# Rules to build and install OSLib
###

$(BUILDSTEPS)/oslib.d: $(BUILDSTEPS)/gcc.d $(SOURCESDIR)/$(UPSTREAM_OSLIB_TARBALL)
	cp -r $(SOURCESDIR)/$(UPSTREAM_OSLIB_TARBALL) $(BUILDDIR)/oslib
	for p in `ls $(RECIPES)/patches/oslib/*.p` ; do patch -d $(BUILDDIR)/oslib -p0 <$$p ; done
	cd $(BUILDDIR)/oslib && GCCSDK_INSTALL_ENV=$(PREFIX)/env GCCSDK_INSTALL_CROSSBIN=$(PREFIX)/cross/bin make install
	touch $@

###
# Rules to build and install GCCSDK
###

$(BUILDSTEPS)/gcc.d: $(BUILDSTEPS)/gccsdk-srcdir.d
	cd $(BUILDDIR)/gcc4/gcc4 && make
	cd $(BUILDDIR)/gcc4/gcc4 && ./install-env
	@# Newer GCCSDK adds these trampoline binaries that for us
	@# serve no purpose and get in the way.
	$(RM) $(PREFIX)/cross/bin/arm-unknown-riscos-gcc-ar $(PREFIX)/cross/bin/arm-unknown-riscos-gcc-nm $(PREFIX)/cross/bin/arm-unknown-riscos-gcc-ranlib
	touch $@

$(BUILDSTEPS)/gccsdk-srcdir.d: $(BUILDSTEPS)/buildsteps.d $(SOURCESDIR)/$(UPSTREAM_GCCSDK_TARBALL)
	tar -C $(BUILDDIR) -xaf $(SOURCESDIR)/$(UPSTREAM_GCCSDK_TARBALL)
	cp -p $(RECIPES)/files/gcc4/gccsdk-params $(BUILDDIR)/gcc4/gcc4/gccsdk-params
	cp -p $(RECIPES)/files/gcc4/*.p $(BUILDDIR)/gcc4/gcc4/recipe/patches/gcc/
	sed -i 's#{PREFIX}#$(PREFIX)#' $(BUILDDIR)/gcc4/gcc4/gccsdk-params
	for p in `ls $(RECIPES)/patches/gcc4/*.p` ; do patch -d $(BUILDDIR)/gcc4 -p0 <$$p ; done
	touch $@

###
# Rules to fetch upstream sources
###

$(SOURCESDIR)/$(UPSTREAM_GCCSDK_TARBALL):
	svn co $(UPSTREAM_GCCSDK_URI) $(SOURCESDIR)/gcc4
	tar -C $(SOURCESDIR) -caf $@ gcc4

$(SOURCESDIR)/$(UPSTREAM_OSLIB_TARBALL):
	svn co $(UPSTREAM_OSLIB_URI) $@

$(SOURCESDIR)/$(UPSTREAM_CCRES_TARBALL):
	svn co $(UPSTREAM_CCRES_URI) $@

$(SOURCESDIR)/$(UPSTREAM_MAKERUN_TARBALL):
	git clone $(UPSTREAM_MAKERUN_URI) $@

$(SOURCESDIR)/$(UPSTREAM_SQUEEZE_TARBALL):
	git clone $(UPSTREAM_SQUEEZE_URI) $@

$(SOURCEDIR)/$(UPSTREAM_INFOZIP_TARBALL):
        $(FETCHSRC) arm-unknown-riscos $(subst $(SOURCEDIR)/,,$@) $(UPSTREAM_INFOZIP_URI) $@

###
# Rule to create buildsteps dir
###

$(BUILDSTEPS)/buildsteps.d: $(SOURCESDIR)
	mkdir -p $(BUILDSTEPS)
	touch $@

$(SOURCESDIR):
	mkdir -p $@

