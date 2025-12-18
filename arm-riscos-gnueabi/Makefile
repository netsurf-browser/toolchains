# NetSurf build script for RISCOS gnueabi cross compilation toolchain.

# Use a tested trunk version of GCCSDK
UPSTREAM_GCCSDK_VERSION := 7698
UPSTREAM_GCCSDK_TARBALL := gccsdk_svn.trunk.$(UPSTREAM_GCCSDK_VERSION).tar.gz

# Constituent upstream source archives for GCCSDK
GCCSDK_UPSTREAM_SOURCES := \
	autoconf_2.64.tar.xz \
	autoconf_2.69.tar.xz \
	automake_1.11.1.tar.bz2 \
	automake_1.15.1.tar.xz \
	binutils_2.30.tar.bz2 \
	gcc_10.2.0.tar.xz \
	libtool_2.4.2.tar.gz

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
UPSTREAM_INFOZIP_TARBALL := zip_$(UPSTREAM_INFOZIP_VERSION).tar.gz

Q ?= @
TOP := $(CURDIR)
RECIPES := $(TOP)/recipes
SOURCESDIR := $(TOP)/sources
BUILDDIR := $(TOP)/builddir
BUILDSTEPS := $(BUILDDIR)/build-steps

TARGET_NAME := arm-riscos-gnueabi

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
	cd $(BUILDDIR)/oslib-$(UPSTREAM_OSLIB_VERSION) && GCCSDK_INSTALL_ENV=$(PREFIX)/env GCCSDK_INSTALL_CROSSBIN=$(PREFIX)/cross/bin make install-elfeabi
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
	mkdir -p $(BUILDDIR)/gccsdk/srcdir.orig
	$(foreach src,$(GCCSDK_UPSTREAM_SOURCES),cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(src) && mv $(TOP)/../$(src) $(BUILDDIR)/gccsdk/srcdir.orig/$(subst _,-,$(src)) &&) :
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

$(SOURCESDIR)/$(UPSTREAM_SQUEEZE_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

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
###

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
UPSTREAM_OSLIB_URI := https://downloads.sourceforge.net/ro-oslib/OSLib-src-$(UPSTREAM_OSLIB_VERSION).zip
UPSTREAM_CCRES_URI := svn://svn.riscos.info/ccres/trunk@$(UPSTREAM_CCRES_VERSION)
UPSTREAM_MAKERUN_URI := git://git.netsurf-browser.org/makerun.git
UPSTREAM_SQUEEZE_URI := git://git.netsurf-browser.org/squeeze.git
UPSTREAM_INFOZIP_URI := https://downloads.sourceforge.net/project/infozip/Zip%203.x%20%28latest%29/3.0/zip30.tar.gz

$(BUILDSTEPS)/sources.d: $(BUILDSTEPS)/buildsteps.d $(BUILDDIR)/$(UPSTREAM_GCCSDK_TARBALL) $(BUILDDIR)/$(UPSTREAM_OSLIB_TARBALL) $(BUILDDIR)/$(UPSTREAM_CCRES_TARBALL) $(BUILDDIR)/$(UPSTREAM_MAKERUN_TARBALL) $(BUILDDIR)/$(UPSTREAM_SQUEEZE_TARBALL) $(BUILDDIR)/$(UPSTREAM_INFOZIP_TARBALL)
	touch $@

$(BUILDDIR)/$(UPSTREAM_GCCSDK_TARBALL):
	rm -rf $(BUILDDIR)/sources/gccsdk
	mkdir -p $(BUILDDIR)/sources/gccsdk
	svn export $(UPSTREAM_GCCSDK_URI) $(BUILDDIR)/sources/gccsdk/gcc
	svn export $(UPSTREAM_GCCSDK_SGP_URI) $(BUILDDIR)/sources/gccsdk/
	svn export $(UPSTREAM_GCCSDK_ROINSTALL_URI) $(BUILDDIR)/sources/gccsdk/
	svn export $(UPSTREAM_GCCSDK_AMP_URI) $(BUILDDIR)/sources/gccsdk/automake
	svn export $(UPSTREAM_GCCSDK_UNIXLIB_URI) $(BUILDDIR)/sources/gccsdk/libunixlib
	svn export $(UPSTREAM_GCCSDK_LD_URI) $(BUILDDIR)/sources/gccsdk/ld
	svn export $(UPSTREAM_GCCSDK_ASASM_URI) $(BUILDDIR)/sources/gccsdk/asasm
	svn export $(UPSTREAM_GCCSDK_ELF2AIF_URI) $(BUILDDIR)/sources/gccsdk/elf2aif
	svn export $(UPSTREAM_GCCSDK_LN_URI) $(BUILDDIR)/sources/gccsdk/ln
	svn export $(UPSTREAM_GCCSDK_MKRESFS_URI) $(BUILDDIR)/sources/gccsdk/mkresfs
	tar -C $(BUILDDIR)/sources -caf $@ gccsdk

$(BUILDDIR)/$(UPSTREAM_OSLIB_TARBALL):
	$(RM) -rf $(BUILDDIR)/sources/oslib
	mkdir -p $(BUILDDIR)/sources/oslib
	wget -q -O $(BUILDDIR)/sources/oslib/oslib-$(UPSTREAM_OSLIB_VERSION).zip $(UPSTREAM_OSLIB_URI)
	mkdir -p $(BUILDDIR)/sources/oslib/tmp
	@# Unzip, converting RISC OS filetypes into ,xxx suffixes
	unzip -F -d $(BUILDDIR)/sources/oslib/tmp $(BUILDDIR)/sources/oslib/oslib-$(UPSTREAM_OSLIB_VERSION).zip
	@# Strip any ,fff suffixes
	for f in $$(find $(BUILDDIR)/sources/oslib/tmp -type f -a -name '*,fff'); do mv $$f $${f%%,fff}; done
	@# Repack the zipfile now it has unix filenames
	cd $(BUILDDIR)/sources/oslib/tmp && zip -9r $(BUILDDIR)/sources/oslib/repacked-oslib-$(UPSTREAM_OSLIB_VERSION).zip .
	@# Convert the repacked zip file into the required tarball
	$(REPACK) $(BUILDDIR)/sources/oslib/repacked-oslib-$(UPSTREAM_OSLIB_VERSION).zip $@

$(BUILDDIR)/$(UPSTREAM_CCRES_TARBALL):
	$(RM) -rf $(BUILDDIR)/sources/ccres
	svn export $(UPSTREAM_CCRES_URI) $(BUILDDIR)/sources/ccres
	tar -C $(BUILDDIR)/sources -caf $@ ccres

$(BUILDDIR)/$(UPSTREAM_MAKERUN_TARBALL):
	$(RM) -rf $(BUILDDIR)/sources/makerun
	git clone --depth=1 $(UPSTREAM_MAKERUN_URI) $(BUILDDIR)/sources/makerun
	tar -C $(BUILDDIR)/sources --exclude-vcs -caf $@ makerun

$(BUILDDIR)/$(UPSTREAM_SQUEEZE_TARBALL):
	$(RM) -rf $(BUILDDIR)/sources/squeeze
	git clone --depth=1 $(UPSTREAM_SQUEEZE_URI) $(BUILDDIR)/sources/squeeze
	tar -C $(BUILDDIR)/sources --exclude-vcs -caf $@ squeeze

$(BUILDDIR)/$(UPSTREAM_INFOZIP_TARBALL):
	wget -q -O $@ $(UPSTREAM_INFOZIP_URI)
