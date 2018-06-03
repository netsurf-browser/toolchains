#!/usr/bin/make

# Build cross toolchain for m5475 processor with an atari mint ABI

UPSTREAM_GCC_VERSION := 4.6.4
UPSTREAM_GCC_TARBALL := gcc-$(UPSTREAM_GCC_VERSION).tar.bz2
UPSTREAM_GCC_URI := http://ftp.gnu.org/gnu/gcc/gcc-$(UPSTREAM_GCC_VERSION)/$(UPSTREAM_GCC_TARBALL)
UPSTREAM_GCC_PATCH := gcc-4.6.4-mint-20130415.patch.bz2
UPSTREAM_GCC_PATCH_URI := http://vincent.riviere.free.fr/soft/m68k-atari-mint/archives/$(UPSTREAM_GCC_PATCH)

UPSTREAM_BINUTILS_VERSION := 2.28
UPSTREAM_BINUTILS_TARBALL := binutils-$(UPSTREAM_BINUTILS_VERSION).tar.bz2
UPSTREAM_BINUTILS_URI := ftp://ftp.gnu.org/gnu/binutils/$(UPSTREAM_BINUTILS_TARBALL)
UPSTREAM_BINUTILS_PATCH := binutils-2.28-mint-20170531.patch.bz2
UPSTREAM_BINUTILS_PATCH_URI := http://vincent.riviere.free.fr/soft/m68k-atari-mint/archives/$(UPSTREAM_BINUTILS_PATCH)

UPSTREAM_GMP_VERSION := 4.3.2
UPSTREAM_GMP_TARBALL := gmp-$(UPSTREAM_GMP_VERSION).tar.bz2
UPSTREAM_GMP_URI := http://ftp.gnu.org/gnu/gmp/$(UPSTREAM_GMP_TARBALL)

# Would use 3.0.0, but that dislikes in-tree gmp sources
UPSTREAM_MPFR_VERSION := 2.4.2
UPSTREAM_MPFR_TARBALL := mpfr-$(UPSTREAM_MPFR_VERSION).tar.bz2
UPSTREAM_MPFR_URI := http://www.mpfr.org/mpfr-$(UPSTREAM_MPFR_VERSION)/$(UPSTREAM_MPFR_TARBALL)

UPSTREAM_MPC_VERSION := 0.8.2
UPSTREAM_MPC_TARBALL := mpc-$(UPSTREAM_MPC_VERSION).tar.gz
UPSTREAM_MPC_URI := http://www.multiprecision.org/mpc/download/$(UPSTREAM_MPC_TARBALL)

UPSTREAM_MINTBIN_VERSION := CVS-20110527
UPSTREAM_MINTBIN_TARBALL := mintbin-$(UPSTREAM_MINTBIN_VERSION).tar.gz
UPSTREAM_MINTBIN_URI := http://vincent.riviere.free.fr/soft/m68k-atari-mint/archives/$(UPSTREAM_MINTBIN_TARBALL)

UPSTREAM_MINTLIB_VERSION := Git-20170304
UPSTREAM_MINTLIB_TARBALL := mintlib-$(UPSTREAM_MINTLIB_VERSION).tar.gz
UPSTREAM_MINTLIB_URI := http://vincent.riviere.free.fr/soft/m68k-atari-mint/archives/$(UPSTREAM_MINTLIB_TARBALL)

UPSTREAM_PML_VERSION := 2.03
UPSTREAM_PML_TARBALL := pml-$(UPSTREAM_PML_VERSION).tar.bz2
UPSTREAM_PML_URI := http://vincent.riviere.free.fr/soft/m68k-atari-mint/archives/$(UPSTREAM_PML_TARBALL)
UPSTREAM_PML_PATCH := pml-2.03-mint-20110207.patch.bz2
UPSTREAM_PML_PATCH_URI := http://vincent.riviere.free.fr/soft/m68k-atari-mint/archives/$(UPSTREAM_PML_PATCH)

UPSTREAM_GEMLIB_VERSION := CVS-20130415
UPSTREAM_GEMLIB_TARBALL := gemlib-$(UPSTREAM_GEMLIB_VERSION).tar.bz2
UPSTREAM_GEMLIB_URI := http://vincent.riviere.free.fr/soft/m68k-atari-mint/archives/$(UPSTREAM_GEMLIB_TARBALL)

GCC_AUTOCONF := autoconf2.64
GCC_AUTOHEADER := autoheader2.64
GCC_AUTORECONF := autoreconf2.64
GCC_AUTOM4TE := autom4te2.64
GCC_AUTOMAKE := automake-1.11
GCC_ACLOCAL := aclocal-1.11
GCC_MAKEINFO := :

TOP := $(CURDIR)
RECIPES := $(TOP)/recipes
SOURCESDIR := $(TOP)/sources
BUILDDIR := $(TOP)/builddir
BUILDSTEPS := $(BUILDDIR)/build-steps
SRCDIR := $(BUILDDIR)/srcdir
GCC_SRCDIR := $(SRCDIR)/gcc
BINUTILS_SRCDIR := $(SRCDIR)/binutils
MINTBIN_SRCDIR := $(SRCDIR)/mintbin
# optionally pass -jN here
BUILD_SMP :=

TARGET_NAME := m5475-atari-mint

PREFIX ?= /opt/netsurf/$(TARGET_NAME)/cross

.PHONY: all clean distclean
all: $(BUILDSTEPS)/gemlib.d

clean:
	rm -fr $(BUILDDIR)

distclean: clean
	rm -fr $(SOURCESDIR)

###
# Rules to build GemLib
###

$(BUILDSTEPS)/gemlib.d: $(BUILDSTEPS)/stage2.d $(SOURCESDIR)/$(UPSTREAM_GEMLIB_TARBALL)
	tar xjf $(SOURCESDIR)/$(UPSTREAM_GEMLIB_TARBALL)
	mv gemlib-$(UPSTREAM_GEMLIB_VERSION) $(BUILDDIR)/gemlib
	for p in `ls $(RECIPES)/patches/gemlib/*.p` ; do patch -d $(BUILDDIR)/gemlib -p0 <$$p ; done
	cd $(BUILDDIR)/gemlib/gemlib && PATH="$(PREFIX)/bin:$(PATH)" make
	cd $(BUILDDIR)/gemlib/gemlib && PATH="$(PREFIX)/bin:$(PATH)" make install
	touch $@

###
# Rules to build the full compiler
###

$(BUILDSTEPS)/stage2.d: $(BUILDSTEPS)/pml.d
	cd $(BUILDDIR) && PATH="$(PREFIX)/bin:$(PATH)" make all
	cd $(BUILDDIR) && PATH="$(PREFIX)/bin:$(PATH)" make install
	touch $@

###
# Rules to build and install PML
###

$(BUILDSTEPS)/pml.d: $(BUILDSTEPS)/mintlib.d $(SOURCESDIR)/$(UPSTREAM_PML_TARBALL) $(SOURCESDIR)/$(UPSTREAM_PML_PATCH)
	tar xjf $(SOURCESDIR)/$(UPSTREAM_PML_TARBALL)
	mv pml-$(UPSTREAM_PML_VERSION) $(BUILDDIR)/pml
	bzcat $(SOURCESDIR)/$(UPSTREAM_PML_PATCH) | patch -d $(BUILDDIR)/pml -p1
	for p in `ls $(RECIPES)/patches/pml/*.p` ; do patch -d $(BUILDDIR)/pml -p0 <$$p ; done
	cd $(BUILDDIR)/pml/pmlsrc && PATH="$(PREFIX)/bin:$(PATH)" make
	cd $(BUILDDIR)/pml/pmlsrc && PATH="$(PREFIX)/bin:$(PATH)" make install
	cd $(BUILDDIR)/pml/pmlsrc && PATH="$(PREFIX)/bin:$(PATH)" make clean
	cd $(BUILDDIR)/pml/pmlsrc && PATH="$(PREFIX)/bin:$(PATH)" SUBTARGET="m68020-60" make
	cd $(BUILDDIR)/pml/pmlsrc && PATH="$(PREFIX)/bin:$(PATH)" SUBTARGET="m68020-60" make install
	cd $(BUILDDIR)/pml/pmlsrc && PATH="$(PREFIX)/bin:$(PATH)" SUBTARGET="m68020-60" make clean
	cd $(BUILDDIR)/pml/pmlsrc && PATH="$(PREFIX)/bin:$(PATH)" SUBTARGET="m5475" make
	cd $(BUILDDIR)/pml/pmlsrc && PATH="$(PREFIX)/bin:$(PATH)" SUBTARGET="m5475" make install
	cd $(BUILDDIR)/pml/pmlsrc && PATH="$(PREFIX)/bin:$(PATH)" SUBTARGET="m5475" make clean
	touch $@

###
# Rules to build and install MINTLib
###

$(BUILDSTEPS)/mintlib.d: $(BUILDSTEPS)/bootstrap-compiler.d $(SOURCESDIR)/$(UPSTREAM_MINTLIB_TARBALL)
	tar xzf $(SOURCESDIR)/$(UPSTREAM_MINTLIB_TARBALL)
	mv mintlib-$(UPSTREAM_MINTLIB_VERSION) $(BUILDDIR)/mintlib
	for p in `ls $(RECIPES)/patches/mintlib/*.p` ; do patch -d $(BUILDDIR)/mintlib -p0 <$$p ; done
	echo "$(PREFIX)/lib/gcc/$(TARGET_NAME)/$(UPSTREAM_GCC_VERSION)/include -I$(PREFIX)/lib/gcc/$(TARGET_NAME)/$(UPSTREAM_GCC_VERSION)/include-fixed -I$(PREFIX)/include" > $(BUILDDIR)/mintlib/includepath
	cd $(BUILDDIR)/mintlib && PATH="$(PREFIX)/bin:$(PATH)" make
	cd $(BUILDDIR)/mintlib && PATH="$(PREFIX)/bin:$(PATH)" make install
	touch $@

###
# Rules to build and install the bootstrap compiler
###

GCC_ENV_PARAMS := AUTOCONF=$(GCC_AUTOCONF) AUTOHEADER=$(GCC_AUTOHEADER) AUTOM4TE=$(GCC_AUTOM4TE) ACLOCAL=$(GCC_ACLOCAL) AUTOMAKE=$(GCC_AUTOMAKE) MAKEINFO=$(GCC_MAKEINFO) PATH="$(PREFIX)/bin:$(PATH)"

$(BUILDSTEPS)/bootstrap-compiler.d: $(BUILDSTEPS)/srcdir-step3.d $(BUILDSTEPS)/binutils.d $(BUILDSTEPS)/mintbin.d
	cd $(BUILDDIR) && $(GCC_ENV_PARAMS) $(GCC_SRCDIR)/configure \
			--prefix=$(PREFIX) --target=$(TARGET_NAME) \
			--disable-nls --enable-languages=c \
			--enable-checking=no --enable-c99 \
			--enable-maintainer-mode --without-headers \
			--with-arch=cf
	cd $(BUILDDIR) && $(GCC_ENV_PARAMS) make $(BUILD_SMP) all-gcc
	cd $(BUILDDIR) && $(GCC_ENV_PARAMS) make install-gcc
	cd $(BUILDDIR) && $(GCC_ENV_PARAMS) make $(BUILD_SMP) all-target-libgcc
	cd $(BUILDDIR) && $(GCC_ENV_PARAMS) make install-target-libgcc
	touch $@

###
# Rules to create the GCC source tree
###


$(BUILDSTEPS)/srcdir-step3.d: $(BUILDSTEPS)/srcdir-step2.d $(SOURCESDIR)/$(UPSTREAM_GCC_PATCH)
	bzcat $(SOURCESDIR)/$(UPSTREAM_GCC_PATCH) | patch -d $(GCC_SRCDIR) -p1
	cd $(GCC_SRCDIR) && ./contrib/gcc_update --touch
	$(GCC_ENV_PARAMS) $(GCC_AUTORECONF) -f $(GCC_SRCDIR)/libmudflap
	$(GCC_ENV_PARAMS) $(GCC_AUTORECONF) -f $(GCC_SRCDIR)/libssp
	$(GCC_ENV_PARAMS) $(GCC_AUTORECONF) -f $(GCC_SRCDIR)/libquadmath
	for p in `ls $(RECIPES)/patches/gcc/*.p` ; do patch -d $(GCC_SRCDIR) -p0 <$$p ; done
	touch $@

$(BUILDSTEPS)/srcdir-step2.d: $(BUILDSTEPS)/srcdir-step1.d $(SOURCESDIR)/$(UPSTREAM_GMP_TARBALL) $(SOURCESDIR)/$(UPSTREAM_MPFR_TARBALL) $(SOURCESDIR)/$(UPSTREAM_MPC_TARBALL)
	tar xjf $(SOURCESDIR)/$(UPSTREAM_GMP_TARBALL)
	mv gmp-$(UPSTREAM_GMP_VERSION) $(GCC_SRCDIR)/gmp
	tar xjf $(SOURCESDIR)/$(UPSTREAM_MPFR_TARBALL)
	mv mpfr-$(UPSTREAM_MPFR_VERSION) $(GCC_SRCDIR)/mpfr
	tar xzf $(SOURCESDIR)/$(UPSTREAM_MPC_TARBALL)
	mv mpc-$(UPSTREAM_MPC_VERSION) $(GCC_SRCDIR)/mpc
	touch $@

$(BUILDSTEPS)/srcdir-step1.d: $(BUILDSTEPS)/$(UPSTREAM_GCC_TARBALL).d
	tar xjf $(SOURCESDIR)/$(UPSTREAM_GCC_TARBALL)
	mv gcc-$(UPSTREAM_GCC_VERSION) $(GCC_SRCDIR)
	touch $@

$(BUILDSTEPS)/$(UPSTREAM_GCC_TARBALL).d: $(BUILDSTEPS)/buildsteps.d $(SOURCESDIR)/$(UPSTREAM_GCC_TARBALL)
	touch $@

###
# Rules to build and install mintbin
###

$(BUILDSTEPS)/mintbin.d: $(BUILDSTEPS)/mintbin-srcdir.d
	mkdir -p $(BUILDDIR)/mintbin
	cd $(BUILDDIR)/mintbin && $(BINUTILS_SRCDIR)/configure --prefix=$(PREFIX) --target=$(TARGET_NAME) --disable-nls
	cd $(BUILDDIR)/mintbin && make
	cd $(BUILDDIR)/mintbin && make install
	touch $@

$(BUILDSTEPS)/mintbin-srcdir.d: $(SOURCESDIR)/$(UPSTREAM_MINTBIN_TARBALL) 
	tar xzf $(SOURCESDIR)/$(UPSTREAM_MINTBIN_TARBALL)
	mv mintbin-$(UPSTREAM_MINTBIN_VERSION) $(MINTBIN_SRCDIR)
	touch $@

###
# Rules to build and install binutils
###

$(BUILDSTEPS)/binutils.d: $(BUILDSTEPS)/binutils-srcdir.d
	mkdir -p $(BUILDDIR)/binutils
	cd $(BUILDDIR)/binutils && $(BINUTILS_SRCDIR)/configure --prefix=$(PREFIX) --target=$(TARGET_NAME) --disable-nls
	cd $(BUILDDIR)/binutils && make $(BUILD_SMP)
	cd $(BUILDDIR)/binutils && make install
	touch $@

$(BUILDSTEPS)/binutils-srcdir.d: $(SOURCESDIR)/$(UPSTREAM_BINUTILS_TARBALL) $(SOURCESDIR)/$(UPSTREAM_BINUTILS_PATCH)
	tar xjf $(SOURCESDIR)/$(UPSTREAM_BINUTILS_TARBALL)
	mv binutils-$(UPSTREAM_BINUTILS_VERSION) $(BINUTILS_SRCDIR)
	bzcat $(SOURCESDIR)/$(UPSTREAM_BINUTILS_PATCH) | patch -d $(BINUTILS_SRCDIR) -p1
	touch $@

###
# Rules to fetch upstream sources
###

$(SOURCESDIR)/$(UPSTREAM_GCC_TARBALL):
	$(TOP)/fetchsrc $(TARGET_NAME) $(UPSTREAM_GCC_TARBALL) $(UPSTREAM_GCC_URI) $@

$(SOURCESDIR)/$(UPSTREAM_GCC_PATCH):
	$(TOP)/fetchsrc $(TARGET_NAME) $(UPSTREAM_GCC_PATCH) $(UPSTREAM_GCC_PATCH_URI) $@

$(SOURCESDIR)/$(UPSTREAM_GMP_TARBALL):
	$(TOP)/fetchsrc $(TARGET_NAME) $(UPSTREAM_GMP_TARBALL) $(UPSTREAM_GMP_URI) $@

$(SOURCESDIR)/$(UPSTREAM_MPFR_TARBALL):
	$(TOP)/fetchsrc $(TARGET_NAME) $(UPSTREAM_MPFR_TARBALL) $(UPSTREAM_MPFR_URI) $@

$(SOURCESDIR)/$(UPSTREAM_MPC_TARBALL):
	$(TOP)/fetchsrc $(TARGET_NAME) $(UPSTREAM_MPC_TARBALL) $(UPSTREAM_MPC_URI)  $@

$(SOURCESDIR)/$(UPSTREAM_BINUTILS_TARBALL):
	$(TOP)/fetchsrc $(TARGET_NAME) $(UPSTREAM_BINUTILS_TARBALL) $(UPSTREAM_BINUTILS_URI) $@

$(SOURCESDIR)/$(UPSTREAM_BINUTILS_PATCH):
	$(TOP)/fetchsrc $(TARGET_NAME) $(UPSTREAM_BINUTILS_PATCH) $(UPSTREAM_BINUTILS_PATCH_URI) $@

$(SOURCESDIR)/$(UPSTREAM_MINTBIN_TARBALL):
	$(TOP)/fetchsrc $(TARGET_NAME) $(UPSTREAM_MINTBIN_TARBALL) $(UPSTREAM_MINTBIN_URI) $@

$(SOURCESDIR)/$(UPSTREAM_MINTBIN_PATCH):
	$(TOP)/fetchsrc $(TARGET_NAME) $(UPSTREAM_MINTBIN_PATCH) $(UPSTREAM_MINTBIN_PATCH_URI) $@

$(SOURCESDIR)/$(UPSTREAM_MINTLIB_TARBALL):
	$(TOP)/fetchsrc $(TARGET_NAME) $(UPSTREAM_MINTLIB_TARBALL) $(UPSTREAM_MINTLIB_URI) $@

$(SOURCESDIR)/$(UPSTREAM_PML_TARBALL):
	$(TOP)/fetchsrc $(TARGET_NAME) $(UPSTREAM_PML_TARBALL) $(UPSTREAM_PML_URI) $@

$(SOURCESDIR)/$(UPSTREAM_PML_PATCH):
	$(TOP)/fetchsrc $(TARGET_NAME) $(UPSTREAM_PML_PATCH) $(UPSTREAM_PML_PATCH_URI) $@

$(SOURCESDIR)/$(UPSTREAM_GEMLIB_TARBALL):
	$(TOP)/fetchsrc $(TARGET_NAME) $(UPSTREAM_GEMLIB_TARBALL) $(UPSTREAM_GEMLIB_URI) $@

###
# Rule to create buildsteps dir
###

$(BUILDSTEPS)/buildsteps.d: $(SOURCESDIR) $(SRCDIR)
	mkdir -p $(BUILDSTEPS)
	touch $@

$(SOURCESDIR):
	mkdir -p $@

$(SRCDIR):
	mkdir -p $@
