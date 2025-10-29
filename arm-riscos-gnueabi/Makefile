# NetSurf build script for RISCOS gnueabi cross compilation toolchain.

# Use a tested trunk version of GCCSDK
UPSTREAM_GCCSDK_VERSION := 7698
UPSTREAM_GCCSDK_TARBALL := gccsdk$(UPSTREAM_GCCSDK_VERSION).tar.gz
UPSTREAM_GCCSDK_URI := svn://svn.riscos.info/gccsdk/trunk/autobuilder/develop/gcc@$(UPSTREAM_GCCSDK_VERSION)
UPSTREAM_GCCSDK_SGP_URI := svn://svn.riscos.info/gccsdk/trunk/gcc4/setup-gccsdk-params@$(UPSTREAM_GCCSDK_VERSION)
UPSTREAM_GCCSDK_ROINSTALL_URI := svn://svn.riscos.info/gccsdk/trunk/gcc4/porting-scripts/ro-install@$(UPSTREAM_GCCSDK_VERSION)
UPSTREAM_GCCSDK_AMP_URI := svn://svn.riscos.info/gccsdk/trunk/gcc4/recipe/patches/automake-for-binutils@$(UPSTREAM_GCCSDK_VERSION)
UPSTREAM_GCCSDK_UNIXLIB_URI := svn://svn.riscos.info/gccsdk/trunk/gcc4/recipe/files/gcc/libunixlib@$(UPSTREAM_GCCSDK_VERSION)
UPSTREAM_GCCSDK_LD_URI := svn://svn.riscos.info/gccsdk/trunk/gcc4/riscos/soloader/dynamic/ld.so-1.9.9@$(UPSTREAM_GCCSDK_VERSION)
UPSTREAM_GCCSDK_ASASM_URI := svn://svn.riscos.info/gccsdk/trunk/tools/asasm@$(UPSTREAM_GCCSDK_VERSION)
UPSTREAM_GCCSDK_ELF2AIF_URI := svn://svn.riscos.info/gccsdk/trunk/gcc4/riscos/elf2aif@$(UPSTREAM_GCCSDK_VERSION)
UPSTREAM_GCCSDK_LN_URI := svn://svn.riscos.info/gccsdk/trunk/gcc4/riscos/ln@$(UPSTREAM_GCCSDK_VERSION)
UPSTREAM_GCCSDK_MKRESFS_URI := svn://svn.riscos.info/gccsdk/trunk/gcc4/riscos/mkresfs@$(UPSTREAM_GCCSDK_VERSION)

# OSLib 7.00
UPSTREAM_OSLIB_VERSION := 700
UPSTREAM_OSLIB_TARBALL := oslib$(UPSTREAM_OSLIB_VERSION).tar.gz
UPSTREAM_OSLIB_URI := https://svn.code.sf.net/p/ro-oslib/code/tags/oslib-$(UPSTREAM_OSLIB_VERSION)/!OSLib

# Known tested version of trunk for CCRes
UPSTREAM_CCRES_VERSION := 96
UPSTREAM_CCRES_TARBALL := ccres$(UPSTREAM_CCRES_VERSION).tar.gz
UPSTREAM_CCRES_URI := svn://svn.riscos.info/ccres/trunk@$(UPSTREAM_CCRES_VERSION)

# Makerun
UPSTREAM_MAKERUN_VERSION := fe55b6bf
UPSTREAM_MAKERUN_TARBALL := makerun$(UPSTREAM_MAKERUN_VERSION).tar.gz
UPSTREAM_MAKERUN_URI := git://git.netsurf-browser.org/makerun.git

# Squeeze
UPSTREAM_SQUEEZE_VERSION := d54d441
UPSTREAM_SQUEEZE_TARBALL := squeeze$(UPSTREAM_SQUEEZE_VERSION).tar.gz
UPSTREAM_SQUEEZE_URI := git://git.netsurf-browser.org/squeeze.git

# Infozip
UPSTREAM_INFOZIP_VERSION := 30
UPSTREAM_INFOZIP_TARBALL := zip$(UPSTREAM_INFOZIP_VERSION).tar.gz
UPSTREAM_INFOZIP_URI := https://downloads.sourceforge.net/project/infozip/Zip%203.x%20%28latest%29/3.0/$(UPSTREAM_INFOZIP_TARBALL)

Q ?= @
TOP := $(CURDIR)
RECIPES := $(TOP)/recipes
SOURCESDIR := $(TOP)/sources
BUILDDIR := $(TOP)/builddir
BUILDSTEPS := $(BUILDDIR)/build-steps

TARGET_NAME := arm-riscos-gnueabi

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
	for p in `ls $(RECIPES)/patches/oslib/*.p` ; do patch -d $(BUILDDIR)/oslib -p0 <$$p ; done
	cd $(BUILDDIR)/oslib && GCCSDK_INSTALL_ENV=$(PREFIX)/env GCCSDK_INSTALL_CROSSBIN=$(PREFIX)/cross/bin make install-elfeabi
	touch $@

###
# Rules to build and install GCCSDK
###

$(BUILDSTEPS)/gcc.d: $(BUILDSTEPS)/gccsdk-srcdir.d
	cd $(BUILDDIR)/gccsdk && make
	@# Install ro-install
	mkdir -p $(PREFIX)/env
	sed -e s:GCCSDK_ENV:$(PREFIX)/env:g < $(BUILDDIR)/gccsdk/ro-install > $(PREFIX)/env/ro-install
	chmod 755 $(PREFIX)/env/ro-install
	@# Install empty libraries needed to fool configure scripts
	for l in c pthread gcc_s ; do $(PREFIX)/cross/bin/$(TARGET_NAME)-ar mv $(PREFIX)/cross/$(TARGET_NAME)/lib/lib$$l.a ; done
	@# Newer GCCSDK adds these trampoline binaries that for us
	@# serve no purpose and get in the way.
	$(RM) $(PREFIX)/cross/bin/$(TARGET_NAME)-gcc-ar $(PREFIX)/cross/bin/$(TARGET_NAME)-gcc-nm $(PREFIX)/cross/bin/$(TARGET_NAME)-gcc-ranlib
	touch $@

$(BUILDSTEPS)/gccsdk-srcdir.d: $(BUILDSTEPS)/buildsteps.d $(SOURCESDIR)/$(UPSTREAM_GCCSDK_TARBALL)
	tar -C $(BUILDDIR) -xaf $(SOURCESDIR)/$(UPSTREAM_GCCSDK_TARBALL)
	cp -p $(RECIPES)/files/gccsdk/gccsdk-params $(BUILDDIR)/gccsdk/gccsdk-params
	sed -i 's#{PREFIX}#$(PREFIX)#' $(BUILDDIR)/gccsdk/gccsdk-params
	sed -i \
		-e "s#GCCSDK_SUPPORTED_GCC_RELEASE=[0-9.]\+#GCCSDK_SUPPORTED_GCC_RELEASE=$$(grep '^GCC_VERSION=[0-9.]\+' $(BUILDDIR)/gccsdk/gcc/setvars | sed -e 's/^GCC_VERSION=//')#" \
		-e "s#GCCSDK_SUPPORTED_BINUTILS_RELEASE=[0-9.]\+#GCCSDK_SUPPORTED_BINUTILS_RELEASE=$$(grep '^BINUTILS_VERSION=[0-9.]\+' $(BUILDDIR)/gccsdk/gcc/setvars | sed -e 's/^BINUTILS_VERSION=//')#" \
		$(BUILDDIR)/gccsdk/setup-gccsdk-params
	cp -p $(RECIPES)/files/gccsdk/Makefile $(BUILDDIR)/gccsdk/Makefile
	$(Q)for p in $$(ls $(RECIPES)/files/gccsdk/*.p 2>/dev/null) ; do echo " COPY: $$p -> $(BUILDDIR)/gccsdk/gcc/" ; cp -p $$p $(BUILDDIR)/gccsdk/gcc/ ; done
	$(Q)for p in $$(ls $(RECIPES)/files/gccsdk/*.pp 2>/dev/null) ; do echo " COPY: $$p -> $(BUILDDIR)/gccsdk/gcc/" ; cp -p $$p $(BUILDDIR)/gccsdk/gcc/ ; done
	$(Q)for p in $$(ls $(RECIPES)/patches/gccsdk/*.p 2>/dev/null) ; do echo "PATCH: $$p" ; patch -d $(BUILDDIR)/gccsdk -p0 <$$p ; done
	touch $@

###
# Rules to fetch upstream sources
###

$(SOURCESDIR)/$(UPSTREAM_GCCSDK_TARBALL):
	svn export $(UPSTREAM_GCCSDK_URI) $(SOURCESDIR)/gccsdk/gcc
	svn export $(UPSTREAM_GCCSDK_SGP_URI) $(SOURCESDIR)/gccsdk/
	svn export $(UPSTREAM_GCCSDK_ROINSTALL_URI) $(SOURCESDIR)/gccsdk/
	svn export $(UPSTREAM_GCCSDK_AMP_URI) $(SOURCESDIR)/gccsdk/automake
	svn export $(UPSTREAM_GCCSDK_UNIXLIB_URI) $(SOURCESDIR)/gccsdk/libunixlib
	svn export $(UPSTREAM_GCCSDK_LD_URI) $(SOURCESDIR)/gccsdk/ld
	svn export $(UPSTREAM_GCCSDK_ASASM_URI) $(SOURCESDIR)/gccsdk/asasm
	svn export $(UPSTREAM_GCCSDK_ELF2AIF_URI) $(SOURCESDIR)/gccsdk/elf2aif
	svn export $(UPSTREAM_GCCSDK_LN_URI) $(SOURCESDIR)/gccsdk/ln
	svn export $(UPSTREAM_GCCSDK_MKRESFS_URI) $(SOURCESDIR)/gccsdk/mkresfs
	tar -C $(SOURCESDIR) -caf $@ gccsdk

$(SOURCESDIR)/$(UPSTREAM_OSLIB_TARBALL):
	svn co $(UPSTREAM_OSLIB_URI) $(SOURCESDIR)/oslib
	tar -C $(SOURCESDIR) -caf $@ oslib

$(SOURCESDIR)/$(UPSTREAM_CCRES_TARBALL):
	svn co $(UPSTREAM_CCRES_URI) $(SOURCESDIR)/ccres
	tar -C $(SOURCESDIR) -caf $@ ccres

$(SOURCESDIR)/$(UPSTREAM_MAKERUN_TARBALL):
	$(RM) -rf $(SOURCESDIR)/makerun
	git clone $(UPSTREAM_MAKERUN_URI) $(SOURCESDIR)/makerun
	tar -C $(SOURCESDIR) -caf $@ makerun

$(SOURCESDIR)/$(UPSTREAM_SQUEEZE_TARBALL):
	$(RM) -rf $(SOURCESDIR)/squeeze
	git clone $(UPSTREAM_SQUEEZE_URI) $(SOURCESDIR)/squeeze
	tar -C $(SOURCESDIR) -caf $@ squeeze

$(SOURCESDIR)/$(UPSTREAM_INFOZIP_TARBALL):
	$(FETCHSRC) arm-unknown-riscos $(subst $(SOURCESDIR)/,,$@) $(UPSTREAM_INFOZIP_URI) $@

###
# Rule to create directories for building
###

$(BUILDSTEPS)/buildsteps.d: | $(BUILDSTEPS) $(SOURCESDIR)
	touch $@

$(BUILDSTEPS):
	mkdir -p $@

$(SOURCESDIR):
	mkdir -p $@
