UPSTREAM_GCC_VERSION := 6.1.0
UPSTREAM_GCC_TARBALL := gcc-$(UPSTREAM_GCC_VERSION).tar.bz2
UPSTREAM_GCC_URI := http://ftp.gnu.org/gnu/gcc/gcc-$(UPSTREAM_GCC_VERSION)/$(UPSTREAM_GCC_TARBALL)

UPSTREAM_BINUTILS_VERSION := 2.23.2
UPSTREAM_BINUTILS_TARBALL := binutils-$(UPSTREAM_BINUTILS_VERSION).tar.bz2
UPSTREAM_BINUTILS_URI := ftp://ftp.gnu.org/gnu/binutils/$(UPSTREAM_BINUTILS_TARBALL)

UPSTREAM_GMP_VERSION := 6.1.2
UPSTREAM_GMP_TARBALL := gmp-$(UPSTREAM_GMP_VERSION).tar.bz2
UPSTREAM_GMP_URI := http://ftp.gnu.org/gnu/gmp/$(UPSTREAM_GMP_TARBALL)

UPSTREAM_MPFR_VERSION := 3.1.6
UPSTREAM_MPFR_TARBALL := mpfr-$(UPSTREAM_MPFR_VERSION).tar.bz2
UPSTREAM_MPFR_URI := http://www.mpfr.org/mpfr-$(UPSTREAM_MPFR_VERSION)/$(UPSTREAM_MPFR_TARBALL)

UPSTREAM_MPC_VERSION := 1.0.2
UPSTREAM_MPC_TARBALL := mpc-$(UPSTREAM_MPC_VERSION).tar.gz
UPSTREAM_MPC_URI := http://ftp.gnu.org/gnu/mpc/$(UPSTREAM_MPC_TARBALL)

UPSTREAM_NDK_TARBALL := SDK_53.29.lha
UPSTREAM_NDK_URI := "http://hyperion-entertainment.biz/index.php/downloads?view=download&format=raw&file=82"

UPSTREAM_OPENURL_VERSION := 7.16
UPSTREAM_OPENURL_TARBALL := openurl-$(UPSTREAM_OPENURL_VERSION).tar.gz
UPSTREAM_OPENURL_URI := https://github.com/jens-maus/libopenurl/archive/$(UPSTREAM_OPENURL_VERSION).tar.gz

UPSTREAM_ONCHIPMEM_VERSION := 53.1
UPSTREAM_ONCHIPMEM_TARBALL := onchipmem_res-$(UPSTREAM_ONCHIPMEM_VERSION).lha
UPSTREAM_ONCHIPMEM_URI := http://www.acube-systems.biz/download/onchipmem_res-$(UPSTREAM_ONCHIPMEM_VERSION).lha

UPSTREAM_GUIGFX_TARBALL := guigfxlib.lha
UPSTREAM_GUIGFX_URI := http://neoscientists.org/~bifat/binarydistillery/$(UPSTREAM_GUIGFX_TARBALL)

UPSTREAM_RENDER_TARBALL := renderlib.lha
UPSTREAM_RENDER_URI := http://neoscientists.org/~bifat/binarydistillery/$(UPSTREAM_RENDER_TARBALL)

UPSTREAM_CODESETS_VERSION := 6.20
UPSTREAM_CODESETS_TARBALL := codesets-$(UPSTREAM_CODESETS_VERSION).lha
UPSTREAM_CODESETS_URI := https://github.com/jens-maus/libcodesets/releases/download/$(UPSTREAM_CODESETS_VERSION)/$(UPSTREAM_CODESETS_TARBALL)

# need to force the auto* version in use
GCC_AUTOCONF := autoconf2.64
GCC_AUTOHEADER := autoheader2.64
GCC_AUTORECONF := autoreconf2.64
GCC_AUTOM4TE := autom4te2.64

TOP := $(CURDIR)
RECIPES := $(TOP)/recipes
SOURCESDIR := $(TOP)/sources
BUILDDIR := $(TOP)/builddir
BUILDSTEPS := $(BUILDDIR)/build-steps
SRCDIR := $(BUILDDIR)/srcdir
GCC_SRCDIR := $(SRCDIR)/gcc
BINUTILS_SRCDIR := $(SRCDIR)/binutils

TARGET_NAME := ppc-amigaos

PREFIX ?= /opt/netsurf/$(TARGET_NAME)/cross

.PHONY: all clean distclean
all: $(BUILDSTEPS)/stage2.d

clean:
	rm -fr $(BUILDDIR)

distclean: clean
	rm -fr $(SOURCESDIR)

###
# Rules to build the full compiler
###

GCC_ENV_PARAMS := AUTOCONF=$(GCC_AUTOCONF) AUTOHEADER=$(GCC_AUTOHEADER) AUTOM4TE=$(GCC_AUTOM4TE) PATH="$(PREFIX)/bin:$(PATH)"

$(BUILDSTEPS)/stage2.d: $(BUILDSTEPS)/srcdir-step3.d $(BUILDSTEPS)/binutils.d $(BUILDSTEPS)/ndk.d
	cd $(BUILDDIR) && $(GCC_ENV_PARAMS) $(GCC_SRCDIR)/configure \
			--prefix=$(PREFIX) --target=$(TARGET_NAME) \
			--disable-nls --disable-c-mbchar --enable-languages=c \
			--enable-checking=no --enable-c99 --with-cross-host \
			--without-x --enable-maintainer-mode --enable-haifa \
			--enable-sjlj-exceptions --disable-libstdcxx-pch \
			--disable-tls --disable-libssp
	cd $(BUILDDIR) && $(GCC_ENV_PARAMS) make all
	cd $(BUILDDIR) && $(GCC_ENV_PARAMS) make install
	touch $@

###
# Rules to install the NDK
###

$(BUILDSTEPS)/ndk.d: $(SOURCESDIR)/$(UPSTREAM_NDK_TARBALL) $(SOURCESDIR)/$(UPSTREAM_OPENURL_TARBALL) $(SOURCESDIR)/$(UPSTREAM_ONCHIPMEM_TARBALL) $(SOURCESDIR)/$(UPSTREAM_GUIGFX_TARBALL) $(SOURCESDIR)/$(UPSTREAM_RENDER_TARBALL) $(SOURCESDIR)/$(UPSTREAM_CODESETS_TARBALL)
	mkdir -p $(BUILDDIR)/ndk/tmp/
	lha xw=$(BUILDDIR)/ndk $(SOURCESDIR)/$(UPSTREAM_NDK_TARBALL)
	lha xw=$(BUILDDIR)/ndk/tmp $(BUILDDIR)/ndk/SDK_Install/base.lha
	lha xw=$(BUILDDIR)/ndk/tmp $(BUILDDIR)/ndk/SDK_Install/newlib-53.30.lha
	lha xw=$(BUILDDIR)/ndk/tmp $(BUILDDIR)/ndk/SDK_Install/clib2-1.206.lha
	lha xw=$(BUILDDIR)/ndk/tmp $(BUILDDIR)/ndk/SDK_Install/pthreads-53.11.lha
	lha xw=$(BUILDDIR)/ndk/tmp $(SOURCESDIR)/$(UPSTREAM_ONCHIPMEM_TARBALL)
	mkdir -p $(PREFIX)/$(TARGET_NAME)/SDK
	cp -r $(BUILDDIR)/ndk/tmp/Include $(PREFIX)/$(TARGET_NAME)/SDK/include
	cp -r $(BUILDDIR)/ndk/tmp/newlib $(PREFIX)/$(TARGET_NAME)/SDK/newlib
	cp -r $(BUILDDIR)/ndk/tmp/clib2 $(PREFIX)/$(TARGET_NAME)/SDK/clib2
	cp -r $(BUILDDIR)/ndk/tmp/Local/common/include/* $(PREFIX)/$(TARGET_NAME)/SDK/newlib/include/
	cp -r $(BUILDDIR)/ndk/tmp/Local/newlib/lib/* $(PREFIX)/$(TARGET_NAME)/SDK/newlib/lib/
	cp -r $(BUILDDIR)/ndk/tmp/SDK/Include/* $(PREFIX)/$(TARGET_NAME)/SDK/include
	mkdir -p $(BUILDDIR)/openurl
	cd $(BUILDDIR)/openurl && tar xaf $(SOURCESDIR)/$(UPSTREAM_OPENURL_TARBALL)
	cp -r $(BUILDDIR)/openurl/libopenurl-$(UPSTREAM_OPENURL_VERSION)/include/* $(PREFIX)/$(TARGET_NAME)/SDK/include/include_h/
	mkdir -p $(BUILDDIR)/guigfxlib
	lha xw=$(BUILDDIR)/guigfxlib $(SOURCESDIR)/$(UPSTREAM_GUIGFX_TARBALL)
	cp -r $(BUILDDIR)/guigfxlib/include_os4/* $(PREFIX)/$(TARGET_NAME)/SDK/include/include_h/
	mkdir -p $(BUILDDIR)/renderlib
	lha xw=$(BUILDDIR)/renderlib $(SOURCESDIR)/$(UPSTREAM_RENDER_TARBALL)
	cp -r $(BUILDDIR)/renderlib/renderlib/include_os4/* $(PREFIX)/$(TARGET_NAME)/SDK/include/include_h/
	mkdir -p $(BUILDDIR)/codesets
	lha xw=$(BUILDDIR)/codesets $(SOURCESDIR)/$(UPSTREAM_CODESETS_TARBALL)
	cp -r $(BUILDDIR)/codesets/codesets/Developer/include/* $(PREFIX)/$(TARGET_NAME)/SDK/include/include_h/
	touch $@

###
# Rules to create the GCC source tree
###

$(BUILDSTEPS)/srcdir-step3.d: $(BUILDSTEPS)/srcdir-step2.d
	for p in `ls $(RECIPES)/patches/gcc/*.p` ; do patch -d $(GCC_SRCDIR) -p0 <$$p ; done
	touch $(GCC_SRCDIR)/lto-plugin/aclocal.m4 $(GCC_SRCDIR)/lto-plugin/Makefile.in
	touch $(GCC_SRCDIR)/zlib/aclocal.m4 $(GCC_SRCDIR)/zlib/Makefile.in
	touch $(GCC_SRCDIR)/libbacktrace/aclocal.m4 $(GCC_SRCDIR)/libbacktrace/Makefile.in
	touch $(GCC_SRCDIR)/libcc1/aclocal.m4 $(GCC_SRCDIR)/libcc1/Makefile.in
	touch $(GCC_SRCDIR)/libquadmath/aclocal.m4 $(GCC_SRCDIR)/libquadmath/Makefile.in
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
# Rules to build and install binutils
###

$(BUILDSTEPS)/binutils.d: $(BUILDSTEPS)/binutils-srcdir.d
	mkdir -p $(BUILDDIR)/binutils
	cd $(BUILDDIR)/binutils && $(BINUTILS_SRCDIR)/configure --prefix=$(PREFIX) --target=$(TARGET_NAME) --disable-nls --enable-plugins --disable-werror
	cd $(BUILDDIR)/binutils && make
	cd $(BUILDDIR)/binutils && make install
	touch $@

$(BUILDSTEPS)/binutils-srcdir.d: $(SOURCESDIR)/$(UPSTREAM_BINUTILS_TARBALL)
	tar xjf $(SOURCESDIR)/$(UPSTREAM_BINUTILS_TARBALL)
	mv binutils-$(UPSTREAM_BINUTILS_VERSION) $(BINUTILS_SRCDIR)
	for p in `ls $(RECIPES)/patches/binutils/*.p` ; do patch -d $(BINUTILS_SRCDIR) -p0 <$$p ; done
	touch $@

###
# Rules to fetch upstream sources
###

$(SOURCESDIR)/$(UPSTREAM_GCC_TARBALL):
	wget -q -O $@ $(UPSTREAM_GCC_URI)

$(SOURCESDIR)/$(UPSTREAM_GMP_TARBALL):
	wget -q -O $@ $(UPSTREAM_GMP_URI)

$(SOURCESDIR)/$(UPSTREAM_MPFR_TARBALL):
	wget -q -O $@ $(UPSTREAM_MPFR_URI)

$(SOURCESDIR)/$(UPSTREAM_MPC_TARBALL):
	wget -q -O $@ $(UPSTREAM_MPC_URI)

$(SOURCESDIR)/$(UPSTREAM_BINUTILS_TARBALL):
	wget -q -O $@ $(UPSTREAM_BINUTILS_URI)

$(SOURCESDIR)/$(UPSTREAM_NDK_TARBALL):
	wget -q -O $@ $(UPSTREAM_NDK_URI)

$(SOURCESDIR)/$(UPSTREAM_OPENURL_TARBALL):
	wget -q -O $@ $(UPSTREAM_OPENURL_URI)

$(SOURCESDIR)/$(UPSTREAM_ONCHIPMEM_TARBALL):
	wget -q -O $@ $(UPSTREAM_ONCHIPMEM_URI)

$(SOURCESDIR)/$(UPSTREAM_GUIGFX_TARBALL):
	wget -q -O $@ $(UPSTREAM_GUIGFX_URI)

$(SOURCESDIR)/$(UPSTREAM_RENDER_TARBALL):
	wget -q -O $@ $(UPSTREAM_RENDER_URI)

$(SOURCESDIR)/$(UPSTREAM_CODESETS_TARBALL):
	wget -q -O $@ $(UPSTREAM_CODESETS_URI)

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
