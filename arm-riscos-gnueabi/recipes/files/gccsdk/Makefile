# To build GCCSDK cross-compiler.
# Written by John Tytgat / BASS
# Copyright (c) 2009-2021 GCCSDK Developers
#
# Build requirements:
#   - apt-get install flex bison libgmp3-dev libmpfr-dev texinfo build-essential
#   - binutils 2.30 and gcc 10.2 needs automake 1.11.1 and autoconf 2.64 and
#     a recent libtool version.
#     Note the required automake/autoconf/libtool sources will be fetched
#     & built automatically.
#   - gcc 10.2 needs at least GMP 6.1.0, ISL 0.18, MPFR 3.1.4 and MPC 1.0.3 (MPFR needs GMP, MPC needs GMP & MPFR)
#     For MPFR/GMP/ISL/MPC version numbers, a good set can be found mentioned at gcc/contrib/download_prerequisites.

# TARGET can have following values: arm-riscos-gnueabi
TARGET=arm-riscos-gnueabi
GCC_LANGUAGES="c,c++"
# Enable shared library support in the cross compiler ?
CROSS_ENABLE_SHARED=yes

# Release ID for GCCSDK RISC OS release (eg 4.7.4 release 6)
GCCSDK_GCC_RISCOS_RELEASE=1

# Versions of binutils/autotools needed to build our tools
AUTOCONF_FOR_BINUTILS_VERSION=2.64
AUTOMAKE_FOR_BINUTILS_VERSION=1.11.1
LIBTOOL_FOR_BINUTILS_VERSION=2.4.2
BINUTILS_VERSION=$(GCCSDK_SUPPORTED_BINUTILS_RELEASE)
AUTOCONF_FOR_GCC_VERSION=2.69
AUTOMAKE_FOR_GCC_VERSION=1.15.1
LIBTOOL_FOR_GCC_VERSION=2.4.2

# Versions of upstream components we need
# XXX: fetch these ourselves, rather than using download_prerequisites
GCC_VERSION=$(GCCSDK_SUPPORTED_GCC_RELEASE)
GMP_VERSION=6.1.0
ISL_VERSION=0.18
MPFR_VERSION=3.1.4
MPC_VERSION=1.1.0

# Whether to enable link-time optimisation
GCC_USE_LTO=no

# If $(AUTOCONF_AUTOMAKE_LIBTOOL_FOR_BINUTILS_VERSIONS) equals
# $(AUTOCONF_AUTOMAKE_LIBTOOL_FOR_GCC_VERSIONS) we can reuse the installed
# buildtools for binutils for building gcc.
AUTOCONF_AUTOMAKE_LIBTOOL_FOR_BINUTILS_VERSIONS := $(AUTOCONF_FOR_BINUTILS_VERSION)-$(AUTOMAKE_FOR_BINUTILS_VERSION)-$(LIBTOOL_FOR_BINUTILS_VERSION)
AUTOCONF_AUTOMAKE_LIBTOOL_FOR_GCC_VERSIONS := $(AUTOCONF_FOR_GCC_VERSION)-$(AUTOMAKE_FOR_GCC_VERSION)-$(LIBTOOL_FOR_GCC_VERSION)

# Notes:
#   1) Configure options are explained at http://gcc.gnu.org/install/configure.html
#      but note that this explanation is for the latest gcc version released and
#      not necessary the gcc version we're building here.
# Specifying --disable-multilib will significantly reduce build times but then
# you only end up with UnixLib/softfloat support.
# Note: --enable-multilib is the default.  Don't specify it unless you want to get hit
# by PR43328/PR45174.
GCC_CONFIG_ARGS := \
	--enable-threads=posix \
	--enable-sjlj-exceptions=no \
	--enable-c99 \
	--enable-cmath \
	--disable-c-mbchar \
	--disable-wchar_t \
	--disable-libstdcxx-pch \
	--disable-tls \
	--enable-__cxa_atexit \
	--disable-libssp \
	--disable-libgomp \
	--disable-libitm \
	--with-abi=aapcs-linux \
	--with-arch=armv4 \
	--with-float=soft
BINUTILS_CONFIG_ARGS :=
GCC_CONFIG_ARGS += --with-pkgversion='GCCSDK GCC $(GCC_VERSION) Release $(GCCSDK_GCC_RISCOS_RELEASE)' \
	--with-bugurl=http://gccsdk.riscos.info/
BINUTILS_CONFIG_ARGS += --with-pkgversion='GCCSDK GCC $(GCC_VERSION) Release $(GCCSDK_GCC_RISCOS_RELEASE)' \
	--with-bugurl=http://gccsdk.riscos.info/
BINUTILS_CONFIG_ARGS += --disable-werror --with-gcc --enable-interwork --disable-nls #--enable-maintainer-mode
# --disable-werror is added because --enable-maintainer-mode turns all warnings into errors and
# the gcc build is not 100% warning free.
# However, this does not help for libstdc++ builds when newlib is used, cfr http://gcc.gnu.org/PR47058.
GCC_CONFIG_ARGS += --enable-maintainer-mode --disable-werror
GCC_CONFIG_ARGS += --enable-interwork --disable-nls --disable-libquadmath

# When debugging/testing/validating the compiler add "--enable-checking=all",
# otherwise add "--enable-checking=release" or even "--enable-checking=no"
GCC_CONFIG_ARGS += --enable-checking=release
# Configure args shared between different targets:
# For debugging:
# FIXME: add to GCC_BUILD_FLAGS for optimized ARM libraries: CFLAGS_FOR_TARGET="-O3 -march=armv5" CXXFLAGS_FOR_TARGET="-O3 -march=armv5"
# Or perhaps better, at GCC configure time something like --with-arch=armv6 --with-tune=cortex-a8 --with-float=softfp --with-fpu=vfp ?
##GCC_BUILD_FLAGS := CFLAGS="-O0 -g" LIBCXXFLAGS="-O0 -g"
##BINUTILS_BUILD_FLAGS = CFLAGS="-O0 -g"

ROOT := $(CURDIR)
PREFIX_CROSS := $(GCCSDK_CROSS_PREFIX)
PREFIX_RONATIVE := $(GCCSDK_RISCOS_PREFIX)/\!GCC
BUILDDIR := $(GCCSDK_BUILDDIR)
BUILDSTEPSDIR := buildstepsdir
# PREFIX_BUILDTOOL_BINUTILS will contain the built autoconf/automake/libtool versions used during building binutils.
PREFIX_BUILDTOOL_BINUTILS := $(BUILDDIR)/installed-buildtools-for-binutils
# PREFIX_BUILDTOOL_GCC will contain the built autoconf/automake/libtool versions used during building gcc.
# When the autoconf/automake/libtool versions needed to build gcc are the same as the autoconf/automake/libtool versions
# needed to build binutils, PREFIX_BUILDTOOL_GCC will be a symlink to PREFIX_BUILDTOOL_BINUTILS
# (compare $(AUTOCONF_AUTOMAKE_LIBTOOL_FOR_BINUTILS_VERSIONS) with $(AUTOCONF_AUTOMAKE_LIBTOOL_FOR_GCC_VERSIONS)).
PREFIX_BUILDTOOL_GCC := $(BUILDDIR)/installed-buildtools-for-gcc
PREFIX_CROSSGCC_LIBS := $(BUILDDIR)/installed-libs-for-cross-gcc
SRCDIR := $(GCCSDK_SRCDIR)
SRCORIGDIR := $(GCCSDK_SRCDIR).orig

ifeq ($(CROSS_ENABLE_SHARED),yes)
CROSS_GCC_CONFIG_ARGS += target_configargs=--enable-shared=libunixlib,libgcc,libstdc++,libbacktrace
else
CROSS_GCC_CONFIG_ARGS += target_configargs=--disable-shared
endif
ifeq ($(GCC_USE_LTO),yes)
CROSS_GCC_CONFIG_ARGS += --enable-lto
else
CROSS_GCC_CONFIG_ARGS += --disable-lto
endif

# Configure arguments Binutils & GCC:
CROSS_CONFIG_ARGS := --target=$(TARGET) --prefix=$(PREFIX_CROSS)

# To respawn Makefile with setup-gccsdk-param environment loaded.
GCCSDK_INTERNAL_GETENV=getenv
ifeq ($(MAKECMDGOALS),)
MAKECMDGOALS=all
endif

.NOTPARALLEL:
.PHONY: all all-done cross clean clean-done clean-cross clean-cross-done distclean distclean-done getenv
VPATH = $(BUILDSTEPSDIR)

# Default target is to build the cross-compiler (including the RISC OS tools):
all: $(GCCSDK_INTERNAL_GETENV)
all-done: cross-done

# Builds the cross compiler:
cross: $(GCCSDK_INTERNAL_GETENV)
cross-done: cross-gcc-built cross-riscostools-built
	touch $(BUILDSTEPSDIR)/$@

clean: $(GCCSDK_INTERNAL_GETENV)
clean-done:
	-rm -rf $(BUILDDIR) $(BUILDSTEPSDIR) $(SRCDIR) $(GCCSDK_CROSS_PREFIX)

# Return to a state for doing a full fresh cross build (using the current binutils/gcc sources).
clean-cross: $(GCCSDK_INTERNAL_GETENV)
clean-cross-done:
	-rm -rf $(BUILDSTEPSDIR)/cross-* $(BUILDDIR)/cross* $(GCCSDK_CROSS_PREFIX)

distclean: $(GCCSDK_INTERNAL_GETENV)
distclean-done: clean-done
	-rm -rf release-area $(SRCORIGDIR)

# Respawn Makefile again after having loaded all our GCCSDK environment variables.
ifeq ($(GCCSDK_INTERNAL_GETENV),getenv)
getenv:
	test -d $(BUILDSTEPSDIR) || mkdir $(BUILDSTEPSDIR)
	@bash -c ". ./setup-gccsdk-params && $(MAKE) $(patsubst %,%-done,$(MAKECMDGOALS)) GCCSDK_INTERNAL_GETENV="
endif

# -- Configure & building:

# Configure & build autoconf-for-binutils tool:
buildtool-autoconf-for-binutils-built: src-autoconf-for-binutils-copied
	-rm -rf $(BUILDDIR)/buildtool-autoconf-for-binutils
	mkdir -p $(BUILDDIR)/buildtool-autoconf-for-binutils
	cd $(BUILDDIR)/buildtool-autoconf-for-binutils && $(SRCDIR)/autoconf-for-binutils/configure --prefix=$(PREFIX_BUILDTOOL_BINUTILS) && $(MAKE) && $(MAKE) install
	touch $(BUILDSTEPSDIR)/$@

ifneq ($(AUTOCONF_AUTOMAKE_LIBTOOL_FOR_BINUTILS_VERSIONS),$(AUTOCONF_AUTOMAKE_LIBTOOL_FOR_GCC_VERSIONS))
# Configure & build autoconf-for-gcc tool:
buildtool-autoconf-for-gcc-built: src-autoconf-for-gcc-copied
	-rm -rf $(BUILDDIR)/buildtool-autoconf-for-gcc
	mkdir -p $(BUILDDIR)/buildtool-autoconf-for-gcc
	cd $(BUILDDIR)/buildtool-autoconf-for-gcc && $(SRCDIR)/autoconf-for-gcc/configure --prefix=$(PREFIX_BUILDTOOL_GCC) && $(MAKE) && $(MAKE) install
else
# autoconf-for-gcc, automake-for-gcc and libtool-for-gcc are the same as autoconf-for-binutils, automake-for-binutils and libtool-for-binutils so we can use PREFIX_BUILDTOOL_GCC as PREFIX_BUILDTOOL_BINUTILS
buildtool-autoconf-for-gcc-built: buildtool-autoconf-for-binutils-built
	ln -f -s $(PREFIX_BUILDTOOL_BINUTILS) $(PREFIX_BUILDTOOL_GCC)
endif
	touch $(BUILDSTEPSDIR)/$@

# Configure & build automake-for-binutils tool:
buildtool-automake-for-binutils-built: src-automake-for-binutils-copied buildtool-autoconf-for-binutils-built
	-rm -rf $(BUILDDIR)/buildtool-automake-for-binutils
	mkdir -p $(BUILDDIR)/buildtool-automake-for-binutils
	cd $(BUILDDIR)/buildtool-automake-for-binutils && PATH="$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" && $(SRCDIR)/automake-for-binutils/configure --prefix=$(PREFIX_BUILDTOOL_BINUTILS) && $(MAKE) && $(MAKE) install
	touch $(BUILDSTEPSDIR)/$@

ifneq ($(AUTOCONF_AUTOMAKE_LIBTOOL_FOR_BINUTILS_VERSIONS),$(AUTOCONF_AUTOMAKE_LIBTOOL_FOR_GCC_VERSIONS))
# Configure & build automake-for-gcc tool:
buildtool-automake-for-gcc-built: src-automake-for-gcc-copied buildtool-autoconf-for-gcc-built
	-rm -rf $(BUILDDIR)/buildtool-automake-for-gcc
	mkdir -p $(BUILDDIR)/buildtool-automake-for-gcc
	cd $(BUILDDIR)/buildtool-automake-for-gcc && PATH="$(PREFIX_BUILDTOOL_GCC)/bin:$(PATH)" && $(SRCDIR)/automake-for-gcc/configure --prefix=$(PREFIX_BUILDTOOL_GCC) && $(MAKE) && $(MAKE) install
else
# autoconf-for-gcc, automake-for-gcc and libtool-for-gcc are the same as autoconf-for-binutils, automake-for-binutils and libtool-for-binutils so we can use PREFIX_BUILDTOOL_GCC as PREFIX_BUILDTOOL_BINUTILS
buildtool-automake-for-gcc-built: buildtool-automake-for-binutils-built
endif
	touch $(BUILDSTEPSDIR)/$@

# Configure & build libtool-for-binutils tool:
buildtool-libtool-for-binutils-built: src-libtool-for-binutils-copied buildtool-autoconf-for-binutils-built buildtool-automake-for-binutils-built
	-rm -rf $(BUILDDIR)/buildtool-libtool-for-binutils
	mkdir -p $(BUILDDIR)/buildtool-libtool-for-binutils
	cd $(BUILDDIR)/buildtool-libtool-for-binutils && PATH="$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" && $(SRCDIR)/libtool-for-binutils/configure --prefix=$(PREFIX_BUILDTOOL_BINUTILS) && $(MAKE) && $(MAKE) install
	touch $(BUILDSTEPSDIR)/$@

ifneq ($(AUTOCONF_AUTOMAKE_LIBTOOL_FOR_BINUTILS_VERSIONS),$(AUTOCONF_AUTOMAKE_LIBTOOL_FOR_GCC_VERSIONS))
# Configure & build libtool-for-gcc tool:
buildtool-libtool-for-gcc-built: src-libtool-for-gcc-copied buildtool-autoconf-for-gcc-built buildtool-automake-for-gcc-built
	-rm -rf $(BUILDDIR)/buildtool-libtool-for-gcc
	mkdir -p $(BUILDDIR)/buildtool-libtool-for-gcc
	cd $(BUILDDIR)/buildtool-libtool-for-gcc && PATH="$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" && $(SRCDIR)/libtool-for-gcc/configure --prefix=$(PREFIX_BUILDTOOL_GCC) && $(MAKE) && $(MAKE) install
else
# autoconf-for-gcc, automake-for-gcc and libtool-for-gcc are the same as autoconf-for-binutils, automake-for-binutils and libtool-for-binutils so we can use PREFIX_BUILDTOOL_GCC as PREFIX_BUILDTOOL_BINUTILS
buildtool-libtool-for-gcc-built: buildtool-libtool-for-binutils-built
endif
	touch $(BUILDSTEPSDIR)/$@

# Configure binutils cross:
cross-binutils-configured: src-binutils-copied
	-rm -rf $(BUILDDIR)/cross-binutils
	mkdir -p $(BUILDDIR)/cross-binutils
	cd $(BUILDDIR)/cross-binutils && PATH="$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" && $(SRCDIR)/binutils/configure $(CROSS_CONFIG_ARGS) $(BINUTILS_CONFIG_ARGS)
	touch $(BUILDSTEPSDIR)/$@

# Build binutils cross:
cross-binutils-built: cross-binutils-configured
	cd $(BUILDDIR)/cross-binutils && PATH="$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" && $(SRCDIR)/binutils/configure $(CROSS_CONFIG_ARGS) $(BINUTILS_CONFIG_ARGS) && $(MAKE) $(BINUTILS_BUILD_FLAGS) && $(MAKE) install
	touch $(BUILDSTEPSDIR)/$@

# Configure gcc cross:
cross-gcc-configured: src-gcc-copied cross-binutils-built
	-rm -rf $(BUILDDIR)/cross-gcc
	mkdir -p $(BUILDDIR)/cross-gcc
	mkdir -p $(PREFIX_CROSS)/$(TARGET)/lib
	for l in c pthread gcc_s; do $(PREFIX_CROSS)/bin/$(TARGET)-ar mv $(PREFIX_CROSS)/$(TARGET)/lib/lib$${l}.a; done
	cd $(BUILDDIR)/cross-gcc && PATH="$(PREFIX_BUILDTOOL_GCC)/bin:$(PREFIX_CROSS)/bin:$(PATH)" && $(SRCDIR)/gcc/configure $(CROSS_CONFIG_ARGS) $(CROSS_GCC_CONFIG_ARGS) $(GCC_CONFIG_ARGS) --enable-languages=$(GCC_LANGUAGES)
	touch $(BUILDSTEPSDIR)/$@

# Build gcc cross:
cross-gcc-built: cross-gcc-configured
	cd $(BUILDDIR)/cross-gcc && PATH="$(PREFIX_BUILDTOOL_GCC)/bin:$(PREFIX_CROSS)/bin:$(PATH)" && $(MAKE) $(GCC_BUILD_FLAGS) && $(MAKE) install
	touch $(BUILDSTEPSDIR)/$@

# Build the RISC OS related tools (elf2aif, asasm, etc) cross:
cross-riscostools-built: cross-gcc-built
	@# LD
	-rm -rf $(BUILDDIR)/cross-ld
	mkdir -p $(BUILDDIR)/cross-ld
	cd $(ROOT)/ld && PATH="$(PREFIX_CROSS)/bin:$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" autoreconf -f --install
	cd $(BUILDDIR)/cross-ld && PATH="$(PREFIX_CROSS)/bin:$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" CC=$(GCCSDK_INSTALL_CROSSBIN)/$(TARGET)-gcc $(ROOT)/ld/configure --prefix=$(PREFIX_CROSS)/$(TARGET) --host=$(TARGET) --target=$(TARGET)
	cd $(BUILDDIR)/cross-ld && PATH="$(PREFIX_CROSS)/bin:$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" make install
	@# asasm
	-rm -rf $(BUILDDIR)/cross-asasm
	mkdir -p $(BUILDDIR)/cross-asasm
	cd $(ROOT)/asasm && PATH="$(PREFIX_CROSS)/bin:$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" ACLOCAL='aclocal -I $(ROOT)/asasm/m4' autoreconf -f --install
	cd $(BUILDDIR)/cross-asasm && PATH="$(PREFIX_CROSS)/bin:$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" $(ROOT)/asasm/configure --prefix=$(PREFIX_CROSS) --target=$(TARGET) --disable-building-tools
	cd $(BUILDDIR)/cross-asasm && PATH="$(PREFIX_CROSS)/bin:$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" make install-minimum
	@# elf2aif
	-rm -rf $(BUILDDIR)/cross-elf2aif
	mkdir -p $(BUILDDIR)/cross-elf2aif
	cd $(ROOT)/elf2aif && PATH="$(PREFIX_CROSS)/bin:$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" autoreconf -f --install
	cd $(BUILDDIR)/cross-elf2aif && PATH="$(PREFIX_CROSS)/bin:$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" $(ROOT)/elf2aif/configure --prefix=$(PREFIX_CROSS) --target=$(TARGET)
	cd $(BUILDDIR)/cross-elf2aif && PATH="$(PREFIX_CROSS)/bin:$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" make install
	@# ln
	-rm -rf $(BUILDDIR)/cross-ln
	mkdir -p $(BUILDDIR)/cross-ln
	cd $(ROOT)/ln && PATH="$(PREFIX_CROSS)/bin:$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" autoreconf -f --install
	cd $(BUILDDIR)/cross-ln && PATH="$(PREFIX_CROSS)/bin:$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" $(ROOT)/ln/configure --prefix=$(PREFIX_CROSS) --target=$(TARGET)
	cd $(BUILDDIR)/cross-ln && PATH="$(PREFIX_CROSS)/bin:$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" make install
	@# mkresfs
	-rm -rf $(BUILDDIR)/cross-mkresfs
	mkdir -p $(BUILDDIR)/cross-mkresfs
	cd $(ROOT)/mkresfs && PATH="$(PREFIX_CROSS)/bin:$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" autoreconf -f --install
	cd $(BUILDDIR)/cross-mkresfs && PATH="$(PREFIX_CROSS)/bin:$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" $(ROOT)/mkresfs/configure --prefix=$(PREFIX_CROSS) --target=$(TARGET)
	cd $(BUILDDIR)/cross-mkresfs && PATH="$(PREFIX_CROSS)/bin:$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" make install
	touch $(BUILDSTEPSDIR)/$@

# -- Source unpacking.

# Unpack & copy autoconf-for-binutils source:
src-autoconf-for-binutils-copied: $(SRCORIGDIR)/autoconf-$(AUTOCONF_FOR_BINUTILS_VERSION).tar.xz
	-rm -rf $(SRCORIGDIR)/autoconf-$(AUTOCONF_FOR_BINUTILS_VERSION) $(SRCDIR)/autoconf-for-binutils
	cd $(SRCORIGDIR) && tar xf $<
	-mkdir -p $(SRCDIR)/autoconf-for-binutils
	cp -T -p -r $(SRCORIGDIR)/autoconf-$(AUTOCONF_FOR_BINUTILS_VERSION) $(SRCDIR)/autoconf-for-binutils
	touch $(BUILDSTEPSDIR)/$@

ifneq ($(AUTOCONF_AUTOMAKE_LIBTOOL_FOR_BINUTILS_VERSIONS),$(AUTOCONF_AUTOMAKE_LIBTOOL_FOR_GCC_VERSIONS))
# Unpack & copy autoconf-for-gcc source:
src-autoconf-for-gcc-copied: $(SRCORIGDIR)/autoconf-$(AUTOCONF_FOR_GCC_VERSION).tar.xz
	-rm -rf $(SRCORIGDIR)/autoconf-$(AUTOCONF_FOR_GCC_VERSION) $(SRCDIR)/autoconf-for-gcc
	cd $(SRCORIGDIR) && tar xf $<
	-mkdir -p $(SRCDIR)/autoconf-for-gcc
	cp -T -p -r $(SRCORIGDIR)/autoconf-$(AUTOCONF_FOR_GCC_VERSION) $(SRCDIR)/autoconf-for-gcc
	touch $(BUILDSTEPSDIR)/$@
endif

# Unpack & copy automake-for-binutils source:
src-automake-for-binutils-copied: $(SRCORIGDIR)/automake-$(AUTOMAKE_FOR_BINUTILS_VERSION).tar.bz2
	-rm -rf $(SRCORIGDIR)/automake-$(AUTOMAKE_FOR_BINUTILS_VERSION) $(SRCDIR)/automake-for-binutils
	cd $(SRCORIGDIR) && tar xf $<
	-mkdir -p $(SRCDIR)/automake-for-binutils
	cp -T -p -r $(SRCORIGDIR)/automake-$(AUTOMAKE_FOR_BINUTILS_VERSION) $(SRCDIR)/automake-for-binutils
	for p in $$(ls $(ROOT)/automake/*.p); do patch -d $(SRCDIR)/automake-for-binutils -p0 -l < $$p; done
	touch $(BUILDSTEPSDIR)/$@

ifneq ($(AUTOCONF_AUTOMAKE_LIBTOOL_FOR_BINUTILS_VERSIONS),$(AUTOCONF_AUTOMAKE_LIBTOOL_FOR_GCC_VERSIONS))
# Unpack & copy automake-for-gcc source:
src-automake-for-gcc-copied: $(SRCORIGDIR)/automake-$(AUTOMAKE_FOR_GCC_VERSION).tar.xz
	-rm -rf $(SRCORIGDIR)/automake-$(AUTOMAKE_FOR_GCC_VERSION) $(SRCDIR)/automake-for-gcc
	cd $(SRCORIGDIR) && tar xf $<
	-mkdir -p $(SRCDIR)/automake-for-gcc
	cp -T -p -r $(SRCORIGDIR)/automake-$(AUTOMAKE_FOR_GCC_VERSION) $(SRCDIR)/automake-for-gcc
	touch $(BUILDSTEPSDIR)/$@
endif

# Unpack & copy libtool-for-binutils source:
src-libtool-for-binutils-copied: $(SRCORIGDIR)/libtool-$(LIBTOOL_FOR_BINUTILS_VERSION).tar.gz
	-rm -rf $(SRCORIGDIR)/libtool-$(LIBTOOL_FOR_BINUTILS_VERSION) $(SRCDIR)/libtool-for-binutils
	cd $(SRCORIGDIR) && tar xf $<
	-mkdir -p $(SRCDIR)/libtool-for-binutils
	cp -T -p -r $(SRCORIGDIR)/libtool-$(LIBTOOL_FOR_BINUTILS_VERSION) $(SRCDIR)/libtool-for-binutils
	touch $(BUILDSTEPSDIR)/$@

ifneq ($(AUTOCONF_AUTOMAKE_LIBTOOL_FOR_BINUTILS_VERSIONS),$(AUTOCONF_AUTOMAKE_LIBTOOL_FOR_GCC_VERSIONS))
# Unpack & copy libtool-for-gcc source:
src-libtool-for-gcc-copied: $(SRCORIGDIR)/libtool-$(LIBTOOL_FOR_GCC_VERSION).tar.gz
	-rm -rf $(SRCORIGDIR)/libtool-$(LIBTOOL_FOR_GCC_VERSION) $(SRCDIR)/libtool-for-gcc
	cd $(SRCORIGDIR) && tar xf $<
	-mkdir -p $(SRCDIR)/libtool-for-gcc
	cp -T -p -r $(SRCORIGDIR)/libtool-$(LIBTOOL_FOR_GCC_VERSION) $(SRCDIR)/libtool-for-gcc
	touch $(BUILDSTEPSDIR)/$@
endif

# Unpack & copy (and optionally patch) binutils source:
# As we're patching and (re)configuring, we need built & installed versions of
# autoconf, automake and libtool.
src-binutils-copied: $(SRCORIGDIR)/binutils-$(BINUTILS_VERSION).tar.bz2 buildtool-autoconf-for-binutils-built buildtool-automake-for-binutils-built buildtool-libtool-for-binutils-built
	-rm -rf $(SRCORIGDIR)/binutils-$(BINUTILS_VERSION) $(SRCDIR)/binutils
	cd $(SRCORIGDIR) && tar xf $<
	-mkdir -p $(SRCDIR)/binutils
	cp -T -p -r $(SRCORIGDIR)/binutils-$(BINUTILS_VERSION) $(SRCDIR)/binutils
	cp $(ROOT)/gcc/ld.emulparams.armelf_riscos_eabi.sh $(SRCDIR)/binutils/ld/emulparams/armelf_riscos_eabi.sh
	cp $(ROOT)/gcc/gas.config.te-riscos.h $(SRCDIR)/binutils/gas/config/te-riscos.h
	for p in $$(ls $(ROOT)/gcc/*.pp); do patch -d $(SRCDIR)/binutils -p0 -l < $$p ; done
	cd $(SRCDIR)/binutils/ld && PATH="$(PREFIX_BUILDTOOL_BINUTILS)/bin:$(PATH)" autoreconf
	touch $(BUILDSTEPSDIR)/$@

# Unpack & copy or checkout the gcc source:
# As we're patching and (re)configuring, we need built & installed versions of
# autoconf, automake and libtool.
src-gcc-copied: buildtool-autoconf-for-gcc-built buildtool-automake-for-gcc-built buildtool-libtool-for-gcc-built
src-gcc-copied: $(SRCORIGDIR)/gcc-$(GCC_VERSION).tar.xz
	-rm -rf $(SRCORIGDIR)/gcc-$(GCC_VERSION) $(SRCDIR)/gcc
	cd $(SRCORIGDIR) && tar xf $<
	-mkdir -p $(SRCDIR)/gcc
	cp -T -p -r $(SRCORIGDIR)/gcc-$(GCC_VERSION) $(SRCDIR)/gcc
	mkdir -p $(SRCDIR)/gcc/libstdc++-v3/config/os/riscos
	cp -T -p $(ROOT)/gcc//gcc.config.arm.riscos-elf.h $(SRCDIR)/gcc/gcc/config/arm/riscos-elf.h
	cp -T -p $(ROOT)/gcc/gcc.config.arm.riscos-gnueabihf.h $(SRCDIR)/gcc/gcc/config/arm/riscos-gnueabihf.h
	cp -T -p $(ROOT)/gcc/gcc.config.arm.riscos-gcc.c $(SRCDIR)/gcc/gcc/config/arm/riscos-gcc.c
	cp -T -p $(ROOT)/gcc/gcc.config.arm.riscos.c $(SRCDIR)/gcc/gcc/config/arm/riscos.c
	cp -T -p $(ROOT)/gcc/gcc.config.arm.riscos.md $(SRCDIR)/gcc/gcc/config/arm/riscos.md
	cp -T -p $(ROOT)/gcc/gcc.config.arm.riscos.opt $(SRCDIR)/gcc/gcc/config/arm/riscos.opt
	cp -T -p $(ROOT)/gcc/gcc.config.arm.t-arm-riscos-elf $(SRCDIR)/gcc/gcc/config/arm/t-arm-riscos-elf
	cp -T -p $(ROOT)/gcc/gcc.config.arm.t-riscos-gnueabihf $(SRCDIR)/gcc/gcc/config/arm/t-riscos-gnueabihf
	cp -T -p $(ROOT)/gcc/gcc.config.arm.xm-riscos.h $(SRCDIR)/gcc/gcc/config/arm/xm-riscos.h
	cp -T -p $(ROOT)/gcc/libgcc.config.arm.t-arm-riscos-elf $(SRCDIR)/gcc/libgcc/config/arm/t-arm-riscos-elf
	cp -T -p $(ROOT)/gcc/libgcc.config.arm.t-riscos-gnueabihf $(SRCDIR)/gcc/libgcc/config/arm/t-riscos-gnueabihf
	cp -T -p $(ROOT)/gcc/libstdc++-v3.config.os.riscos.ctype_base.h $(SRCDIR)/gcc/libstdc++-v3/config/os/riscos/ctype_base.h
	cp -T -p $(ROOT)/gcc/libstdc++-v3.config.os.riscos.ctype_configure_char.cc $(SRCDIR)/gcc/libstdc++-v3/config/os/riscos/ctype_configure_char.cc
	cp -T -p $(ROOT)/gcc/libstdc++-v3.config.os.riscos.ctype_inline.h $(SRCDIR)/gcc/libstdc++-v3/config/os/riscos/ctype_inline.h
	cp -T -p $(ROOT)/gcc/libstdc++-v3.config.os.riscos.error_constants.h $(SRCDIR)/gcc/libstdc++-v3/config/os/riscos/error_constants.h
	cp -T -p $(ROOT)/gcc/libstdc++-v3.config.os.riscos.os_defines.h $(SRCDIR)/gcc/libstdc++-v3/config/os/riscos/os_defines.h
	for p in $$(ls $(ROOT)/gcc/*.p); do patch -d $(SRCDIR)/gcc -p0 -l < $$p ; done
	cp -T -p -r $(ROOT)/libunixlib $(SRCDIR)/gcc/libunixlib
	cd $(SRCDIR)/gcc && $(SRCDIR)/gcc/contrib/download_prerequisites
	cd $(SRCDIR)/gcc && PATH="$(PREFIX_BUILDTOOL_GCC)/bin:$(PATH)" autogen Makefile.def
	cd $(SRCDIR)/gcc && PATH="$(PREFIX_BUILDTOOL_GCC)/bin:$(PATH)" autoconf
	cd $(SRCDIR)/gcc/libcpp && PATH="$(PREFIX_BUILDTOOL_GCC)/bin:$(PATH)" ACLOCAL="aclocal -I .. -I ../config" autoreconf -v
	cd $(SRCDIR)/gcc/libiberty && PATH="$(PREFIX_BUILDTOOL_GCC)/bin:$(PATH)" ACLOCAL="aclocal -I .. -I ../config" autoreconf -v
	cd $(SRCDIR)/gcc/libstdc++-v3 && PATH="$(PREFIX_BUILDTOOL_GCC)/bin:$(PATH)" ACLOCAL="aclocal -I .. -I ../config" autoreconf -v
	cd $(SRCDIR)/gcc && PATH="$(PREFIX_BUILDTOOL_GCC)/bin:$(PATH)" $(SRCDIR)/gcc/libunixlib/gen-auto.pl
	cd $(SRCDIR)/gcc/libunixlib && PATH="$(PREFIX_BUILDTOOL_GCC)/bin:$(PATH)" ACLOCAL="aclocal -I .. -I ../config" autoreconf -v
	touch $(BUILDSTEPSDIR)/$@

# -- Source downloading.

# Download autoconf source to be used to build binutils:
$(SRCORIGDIR)/autoconf-$(AUTOCONF_FOR_BINUTILS_VERSION).tar.xz:
	-mkdir -p $(SRCORIGDIR)
	cd $(SRCORIGDIR) && wget -c https://ftpmirror.gnu.org/autoconf/autoconf-$(AUTOCONF_FOR_BINUTILS_VERSION).tar.xz
	touch $@

# Download automake source to be used to build binutils/autoconf-for-binutils:
$(SRCORIGDIR)/automake-$(AUTOMAKE_FOR_BINUTILS_VERSION).tar.bz2:
	-mkdir -p $(SRCORIGDIR)
	cd $(SRCORIGDIR) && wget -c https://ftpmirror.gnu.org/automake/automake-$(AUTOMAKE_FOR_BINUTILS_VERSION).tar.bz2
	touch $@

# Download libtool source to be used to build binutils/autoconf-for-binutils:
$(SRCORIGDIR)/libtool-$(LIBTOOL_FOR_BINUTILS_VERSION).tar.gz:
	-mkdir -p $(SRCORIGDIR)
	cd $(SRCORIGDIR) && wget -c https://ftpmirror.gnu.org/libtool/libtool-$(LIBTOOL_FOR_BINUTILS_VERSION).tar.gz
	touch $@

ifneq ($(AUTOCONF_FOR_BINUTILS_VERSION),$(AUTOCONF_FOR_GCC_VERSION))
# Download autoconf source to be used to build gcc:
$(SRCORIGDIR)/autoconf-$(AUTOCONF_FOR_GCC_VERSION).tar.xz:
	-mkdir -p $(SRCORIGDIR)
	cd $(SRCORIGDIR) && wget -c https://ftpmirror.gnu.org/autoconf/autoconf-$(AUTOCONF_FOR_GCC_VERSION).tar.xz
	touch $@
endif

ifneq ($(AUTOMAKE_FOR_BINUTILS_VERSION),$(AUTOMAKE_FOR_GCC_VERSION))
# Download automake source to be used to build gcc:
$(SRCORIGDIR)/automake-$(AUTOMAKE_FOR_GCC_VERSION).tar.xz:
	-mkdir -p $(SRCORIGDIR)
	cd $(SRCORIGDIR) && wget -c https://ftpmirror.gnu.org/automake/automake-$(AUTOMAKE_FOR_GCC_VERSION).tar.xz
	touch $@
endif

ifneq ($(LIBTOOL_FOR_BINUTILS_VERSION),$(LIBTOOL_FOR_GCC_VERSION))
# Download libtool source to be used to build gcc:
$(SRCORIGDIR)/libtool-$(LIBTOOL_FOR_GCC_VERSION).tar.gz:
	-mkdir -p $(SRCORIGDIR)
	cd $(SRCORIGDIR) && wget -c https://ftpmirror.gnu.org/libtool/libtool-$(LIBTOOL_FOR_GCC_VERSION).tar.gz
	touch $@
endif

# Download binutils source:
$(SRCORIGDIR)/binutils-$(BINUTILS_VERSION).tar.bz2:
	-mkdir -p $(SRCORIGDIR)
	cd $(SRCORIGDIR) && wget -c https://ftpmirror.gnu.org/binutils/binutils-$(BINUTILS_VERSION).tar.bz2
	touch $@

# Download gcc source:
$(SRCORIGDIR)/gcc-$(GCC_VERSION).tar.xz:
	-mkdir -p $(SRCORIGDIR)
	cd $(SRCORIGDIR) && wget -c https://ftpmirror.gnu.org/gcc/gcc-$(GCC_VERSION)/gcc-$(GCC_VERSION).tar.xz
	touch $@
