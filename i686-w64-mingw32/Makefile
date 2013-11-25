#!/usr/bin/make

# Build cross toolchain for i686 processor with a win32 (mingw) ABI

UPSTREAM_GCC_VERSION := 4.6.3
UPSTREAM_GCC_TARBALL := gcc-core-$(UPSTREAM_GCC_VERSION).tar.bz2
UPSTREAM_GCC_URI := http://ftpmirror.gnu.org/gcc/gcc-$(UPSTREAM_GCC_VERSION)/$(UPSTREAM_GCC_TARBALL)

UPSTREAM_BINUTILS_VERSION := 2.22
UPSTREAM_BINUTILS_TARBALL := binutils-$(UPSTREAM_BINUTILS_VERSION).tar.bz2
UPSTREAM_BINUTILS_URI := http://ftpmirror.gnu.org/binutils/$(UPSTREAM_BINUTILS_TARBALL)

UPSTREAM_GMP_VERSION := 5.0.5
UPSTREAM_GMP_TARBALL := gmp-$(UPSTREAM_GMP_VERSION).tar.bz2
UPSTREAM_GMP_URI := http://ftp.gnu.org/gnu/gmp/$(UPSTREAM_GMP_TARBALL)

# Would use 3.0.0, but that dislikes in-tree gmp sources
UPSTREAM_MPFR_VERSION := 2.4.2
UPSTREAM_MPFR_TARBALL := mpfr-$(UPSTREAM_MPFR_VERSION).tar.bz2
UPSTREAM_MPFR_URI := http://www.mpfr.org/mpfr-$(UPSTREAM_MPFR_VERSION)/$(UPSTREAM_MPFR_TARBALL)

UPSTREAM_MPC_VERSION := 0.8.2
UPSTREAM_MPC_TARBALL := mpc-$(UPSTREAM_MPC_VERSION).tar.gz
UPSTREAM_MPC_URI := http://www.multiprecision.org/mpc/download/$(UPSTREAM_MPC_TARBALL)

UPSTREAM_MINGW_VERSION := 2.0.4
UPSTREAM_MINGW_TARBALL := mingw-w64-v$(UPSTREAM_MINGW_VERSION).tar.gz
UPSTREAM_MINGW_URI := "http://downloads.sourceforge.net/project/mingw-w64/mingw-w64/mingw-w64-release/$(UPSTREAM_MINGW_TARBALL)?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fmingw-w64%2F&ts=1342830561&use_mirror=switch"


TOP := $(CURDIR)
RECIPES := $(TOP)/recipes
SOURCESDIR := $(TOP)/sources
BUILDDIR := $(TOP)/builddir
BUILDSTEPS := $(BUILDDIR)/build-steps
SRCDIR := $(BUILDDIR)/srcdir
GCC_SRCDIR := $(SRCDIR)/gcc
BINUTILS_SRCDIR := $(SRCDIR)/binutils
MINGW_SRCDIR := $(SRCDIR)/mingw

TARGET_NAME := i686-w64-mingw32

PREFIX ?= /opt/netsurf/$(TARGET_NAME)/cross

.PHONY: all clean distclean
all: $(BUILDSTEPS)/gcc-final.d

clean:
	rm -fr $(BUILDDIR)

distclean: clean
	rm -fr $(SOURCESDIR)

###
# Rules to build the full compiler
###


$(BUILDSTEPS)/gcc-core.d: $(BUILDSTEPS)/srcdir-step3.d $(BUILDSTEPS)/binutils.d $(BUILDSTEPS)/mingw-headers.d
	${RM} -rf $(BUILDDIR)/gcc
	mkdir -p $(BUILDDIR)/gcc
	cd $(BUILDDIR)/gcc && PATH="$(PREFIX)/bin:$(PATH)" $(GCC_SRCDIR)/configure \
		--prefix=$(PREFIX) --target=$(TARGET_NAME) \
		--enable-languages=c --with-system-zlib --disable-multilib \
		--enable-shared --enable-fully-dynamic-string
	cd $(BUILDDIR)/gcc && PATH="$(PREFIX)/bin:$(PATH)" make all-gcc 
	cd $(BUILDDIR)/gcc && PATH="$(PREFIX)/bin:$(PATH)" make install-gcc
	touch $@

$(BUILDSTEPS)/gcc-final.d: $(BUILDSTEPS)/buildsteps.d $(BUILDSTEPS)/gcc-core.d $(BUILDSTEPS)/mingw-crt.d
	cd $(BUILDDIR)/gcc && PATH="$(PREFIX)/bin:$(PATH)" make 
	cd $(BUILDDIR)/gcc && PATH="$(PREFIX)/bin:$(PATH)" make install
	touch $@


###
# Rules to install mingw
###

$(BUILDSTEPS)/mingw-headers.d: $(BUILDSTEPS)/mingw-srcdir.d
	mkdir -p $(BUILDDIR)/mingw-headers
	cd $(BUILDDIR)/mingw-headers && PATH="$(PREFIX)/bin:$(PATH)" $(MINGW_SRCDIR)/mingw-w64-headers/configure \
	--prefix=$(PREFIX) --host=$(TARGET_NAME) 
	cd $(BUILDDIR)/mingw-headers && PATH="$(PREFIX)/bin:$(PATH)" make
	cd $(BUILDDIR)/mingw-headers && PATH="$(PREFIX)/bin:$(PATH)" make install
	touch $@

$(BUILDSTEPS)/mingw-crt.d: $(BUILDSTEPS)/mingw-srcdir.d
	mkdir -p $(BUILDDIR)/mingw-crt
	cd $(BUILDDIR)/mingw-crt && PATH="$(PREFIX)/bin:$(PATH)" $(MINGW_SRCDIR)/configure \
	--prefix=$(PREFIX) --host=$(TARGET_NAME) 
	cd $(BUILDDIR)/mingw-crt && PATH="$(PREFIX)/bin:$(PATH)" make
	cd $(BUILDDIR)/mingw-crt && PATH="$(PREFIX)/bin:$(PATH)" make install
	touch $@

$(BUILDSTEPS)/mingw-srcdir.d: $(SOURCESDIR)/$(UPSTREAM_MINGW_TARBALL)
	tar xzf $(SOURCESDIR)/$(UPSTREAM_MINGW_TARBALL)
	mv mingw-w64-v$(UPSTREAM_MINGW_VERSION) $(MINGW_SRCDIR)
	touch $@

###
# Rules to create the GCC source tree
###

$(BUILDSTEPS)/srcdir-step3.d: $(BUILDSTEPS)/srcdir-step2.d
	touch $@

$(BUILDSTEPS)/srcdir-step2.d: $(BUILDSTEPS)/srcdir-step1.d $(SOURCESDIR)/$(UPSTREAM_GMP_TARBALL) $(SOURCESDIR)/$(UPSTREAM_MPFR_TARBALL) $(SOURCESDIR)/$(UPSTREAM_MPC_TARBALL)
	tar xjf $(SOURCESDIR)/$(UPSTREAM_GMP_TARBALL)
	mv gmp-$(UPSTREAM_GMP_VERSION) $(GCC_SRCDIR)/gmp
	tar xjf $(SOURCESDIR)/$(UPSTREAM_MPFR_TARBALL)
	mv mpfr-$(UPSTREAM_MPFR_VERSION) $(GCC_SRCDIR)/mpfr
	tar xzf $(SOURCESDIR)/$(UPSTREAM_MPC_TARBALL)
	mv mpc-$(UPSTREAM_MPC_VERSION) $(GCC_SRCDIR)/mpc
	touch $@

$(BUILDSTEPS)/srcdir-step1.d: $(SOURCESDIR)/$(UPSTREAM_GCC_TARBALL)
	tar xjf $(SOURCESDIR)/$(UPSTREAM_GCC_TARBALL)
	mv gcc-$(UPSTREAM_GCC_VERSION) $(GCC_SRCDIR)
	touch $@


###
# Rules to build and install binutils
###

$(BUILDSTEPS)/binutils.d: $(BUILDSTEPS)/binutils-srcdir.d
	mkdir -p $(BUILDDIR)/binutils
	cd $(BUILDDIR)/binutils && $(BINUTILS_SRCDIR)/configure \
	--prefix=$(PREFIX) --target=$(TARGET_NAME) --disable-multilib
	cd $(BUILDDIR)/binutils && make
	cd $(BUILDDIR)/binutils && make install
	touch $@

$(BUILDSTEPS)/binutils-srcdir.d: $(SOURCESDIR)/$(UPSTREAM_BINUTILS_TARBALL)
	tar xjf $(SOURCESDIR)/$(UPSTREAM_BINUTILS_TARBALL)
	mv binutils-$(UPSTREAM_BINUTILS_VERSION) $(BINUTILS_SRCDIR)
	touch $@

###
# Rules to fetch upstream sources
###

$(SOURCESDIR)/$(UPSTREAM_GCC_TARBALL): $(SOURCESDIR)/sourcesdir.d
	wget -q -O $@ $(UPSTREAM_GCC_URI)
	touch $@

$(SOURCESDIR)/$(UPSTREAM_GMP_TARBALL): $(SOURCESDIR)/sourcesdir.d
	wget -q -O $@ $(UPSTREAM_GMP_URI)
	touch $@

$(SOURCESDIR)/$(UPSTREAM_MPFR_TARBALL): $(SOURCESDIR)/sourcesdir.d
	wget -q -O $@ $(UPSTREAM_MPFR_URI)
	touch $@

$(SOURCESDIR)/$(UPSTREAM_MPC_TARBALL): $(SOURCESDIR)/sourcesdir.d
	wget -q -O $@ $(UPSTREAM_MPC_URI)
	touch $@

$(SOURCESDIR)/$(UPSTREAM_BINUTILS_TARBALL): $(SOURCESDIR)/sourcesdir.d
	wget -q -O $@ $(UPSTREAM_BINUTILS_URI) 
	touch $@

$(SOURCESDIR)/$(UPSTREAM_MINGW_TARBALL): $(SOURCESDIR)/sourcesdir.d
	wget -q -O $@ $(UPSTREAM_MINGW_URI)
	touch $@

###
# Rule to create buildsteps dir
###

$(BUILDSTEPS)/buildsteps.d: 
	mkdir -p $(BUILDSTEPS)
	mkdir -p $(SRCDIR)
	touch $@

$(SOURCESDIR)/sourcesdir.d:
	mkdir -p $(SOURCESDIR)
	${RM} $@
	touch $@

