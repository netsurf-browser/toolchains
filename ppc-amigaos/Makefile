# NetSurf build script for Amigaos 4 PPC cross compliation toolchain.

UPSTREAM_GCC_VERSION := 11.3.0
UPSTREAM_GCC_TARBALL := gcc-$(UPSTREAM_GCC_VERSION).tar.xz
UPSTREAM_GCC_URI := http://ftp.gnu.org/gnu/gcc/gcc-$(UPSTREAM_GCC_VERSION)/$(UPSTREAM_GCC_TARBALL)

UPSTREAM_BINUTILS_VERSION := 2.40
UPSTREAM_BINUTILS_TARBALL := binutils-$(UPSTREAM_BINUTILS_VERSION).tar.bz2
UPSTREAM_BINUTILS_URI := ftp://ftp.gnu.org/gnu/binutils/$(UPSTREAM_BINUTILS_TARBALL)

UPSTREAM_GMP_VERSION := 6.1.2
UPSTREAM_GMP_TARBALL := gmp-$(UPSTREAM_GMP_VERSION).tar.bz2
UPSTREAM_GMP_URI := http://ftp.gnu.org/gnu/gmp/$(UPSTREAM_GMP_TARBALL)

UPSTREAM_MPFR_VERSION := 3.1.6
UPSTREAM_MPFR_TARBALL := mpfr-$(UPSTREAM_MPFR_VERSION).tar.bz2
UPSTREAM_MPFR_URI := http://www.mpfr.org/mpfr-$(UPSTREAM_MPFR_VERSION)/$(UPSTREAM_MPFR_TARBALL)

UPSTREAM_MPC_VERSION := 1.0.3
UPSTREAM_MPC_TARBALL := mpc-$(UPSTREAM_MPC_VERSION).tar.gz
UPSTREAM_MPC_URI := http://ftp.gnu.org/gnu/mpc/$(UPSTREAM_MPC_TARBALL)

UPSTREAM_LHASA_VERSION := 887d68ebca0cbad392af3709203745d202a2020b
UPSTREAM_LHASA_TARBALL := lhasa-$(UPSTREAM_LHASA_VERSION).zip
UPSTREAM_LHASA_URI := https://github.com/fragglet/lhasa/archive/$(UPSTREAM_LHASA_VERSION).zip

UPSTREAM_NDK_TARBALL := SDK_54.16.lha
UPSTREAM_NDK_URI := "http://hyperion-entertainment.biz/index.php/downloads?view=download&format=raw&file=127"

UPSTREAM_NDK_UPDATE_TARBALL := SDK_addon_final_edition_update2.zip
UPSTREAM_NDK_UPDATE_URI := http://kas1e.mikendezign.com/aos4/SDK_addon_final_edition_update2.zip

UPSTREAM_CLIB4_VERSION := 1.6.0
UPSTREAM_CLIB4_TARBALL := clib4-v$(UPSTREAM_CLIB4_VERSION).lha
UPSTREAM_CLIB4_URI := https://github.com/AmigaLabs/clib4/releases/download/v$(UPSTREAM_CLIB4_VERSION)/$(UPSTREAM_CLIB4_TARBALL)

UPSTREAM_OPENURL_VERSION := 7.16
UPSTREAM_OPENURL_TARBALL := openurl-$(UPSTREAM_OPENURL_VERSION).tar.gz
UPSTREAM_OPENURL_URI := https://github.com/jens-maus/libopenurl/archive/$(UPSTREAM_OPENURL_VERSION).tar.gz

UPSTREAM_AMISSL_VERSION := 5.17
UPSTREAM_AMISSL_TARBALL := AmiSSL-$(UPSTREAM_AMISSL_VERSION)-SDK.lha
UPSTREAM_AMISSL_URI := https://github.com/jens-maus/amissl/releases/download/$(UPSTREAM_AMISSL_VERSION)/$(UPSTREAM_AMISSL_TARBALL)

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
GCC_AUTOCONF := autoconf2.69
GCC_AUTOHEADER := autoheader2.69
GCC_AUTORECONF := autoreconf2.69
GCC_AUTOM4TE := autom4te2.69

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

GCC_ENV_PARAMS := AUTOCONF=$(GCC_AUTOCONF) AUTOHEADER=$(GCC_AUTOHEADER) AUTOM4TE=$(GCC_AUTOM4TE) PATH="$(PREFIX)/bin:$(PATH)" CXXFLAGS="-fpermissive"

$(BUILDSTEPS)/stage2.d: $(BUILDSTEPS)/srcdir-step3.d $(BUILDSTEPS)/binutils.d $(BUILDSTEPS)/ndk.d
	cd $(BUILDDIR) && $(GCC_ENV_PARAMS) $(GCC_SRCDIR)/configure \
			--prefix=$(PREFIX) --target=$(TARGET_NAME) \
			--disable-nls --disable-c-mbchar --enable-languages=c \
			--enable-checking=no --enable-c99 --with-cross-host \
			--without-x --enable-maintainer-mode --enable-haifa \
			--enable-sjlj-exceptions --disable-libstdcxx-pch \
			--disable-tls --disable-libssp --enable-lto
	cd $(BUILDDIR) && $(GCC_ENV_PARAMS) make all
	cd $(BUILDDIR) && $(GCC_ENV_PARAMS) make install
	touch $@

###
# Rules to install the NDK
###

$(BUILDSTEPS)/ndk.d: $(BUILDSTEPS)/lhasa.d $(SOURCESDIR)/$(UPSTREAM_NDK_TARBALL) $(SOURCESDIR)/$(UPSTREAM_NDK_UPDATE_TARBALL) $(SOURCESDIR)/$(UPSTREAM_OPENURL_TARBALL) $(SOURCESDIR)/$(UPSTREAM_ONCHIPMEM_TARBALL) $(SOURCESDIR)/$(UPSTREAM_GUIGFX_TARBALL) $(SOURCESDIR)/$(UPSTREAM_RENDER_TARBALL) $(SOURCESDIR)/$(UPSTREAM_CODESETS_TARBALL) $(SOURCESDIR)/$(UPSTREAM_AMISSL_TARBALL) $(SOURCESDIR)/$(UPSTREAM_CLIB4_TARBALL)
	mkdir -p $(BUILDDIR)/ndk/tmp/
	$(BUILDDIR)/prefix/bin/lha xw=$(BUILDDIR)/ndk $(SOURCESDIR)/$(UPSTREAM_NDK_TARBALL)
	$(BUILDDIR)/prefix/bin/lha xw=$(BUILDDIR)/ndk/tmp $(BUILDDIR)/ndk/SDK_Install/base.lha
	$(BUILDDIR)/prefix/bin/lha xw=$(BUILDDIR)/ndk/tmp $(BUILDDIR)/ndk/SDK_Install/execsg_sdk-54.31.lha
	$(BUILDDIR)/prefix/bin/lha xw=$(BUILDDIR)/ndk/tmp $(BUILDDIR)/ndk/SDK_Install/newlib-53.80.lha
	$(BUILDDIR)/prefix/bin/lha xw=$(BUILDDIR)/ndk/tmp $(BUILDDIR)/ndk/SDK_Install/clib2.lha
	$(BUILDDIR)/prefix/bin/lha xw=$(BUILDDIR)/ndk/tmp $(BUILDDIR)/ndk/SDK_Install/pthreads-53.12.lha
	$(BUILDDIR)/prefix/bin/lha xw=$(BUILDDIR)/ndk/tmp $(SOURCESDIR)/$(UPSTREAM_ONCHIPMEM_TARBALL)
	mkdir -p $(PREFIX)/$(TARGET_NAME)/SDK
	unzip -o $(SOURCESDIR)/$(UPSTREAM_NDK_UPDATE_TARBALL) -d $(BUILDDIR)/ndk/tmp
	cp -r $(BUILDDIR)/ndk/tmp/Include $(PREFIX)/$(TARGET_NAME)/SDK/include
	cp -r $(BUILDDIR)/ndk/tmp/newlib $(PREFIX)/$(TARGET_NAME)/SDK/newlib
	cp -r $(BUILDDIR)/ndk/tmp/clib2 $(PREFIX)/$(TARGET_NAME)/SDK/clib2
	cp -r $(BUILDDIR)/ndk/tmp/Local/common/include/* $(PREFIX)/$(TARGET_NAME)/SDK/newlib/include/
	cp -r $(BUILDDIR)/ndk/tmp/Local/newlib/lib/* $(PREFIX)/$(TARGET_NAME)/SDK/newlib/lib/
	cp -r $(BUILDDIR)/ndk/tmp/SDK/Include/* $(PREFIX)/$(TARGET_NAME)/SDK/include
	cp -r $(BUILDDIR)/ndk/tmp/SDK/newlib $(PREFIX)/$(TARGET_NAME)/SDK/newlib
	cp -r $(BUILDDIR)/ndk/tmp/SDK/local/newlib/lib/* $(PREFIX)/$(TARGET_NAME)/SDK/newlib/lib/
	cp -r $(BUILDDIR)/ndk/tmp/SDK/local/common/include/* $(PREFIX)/$(TARGET_NAME)/SDK/newlib/include/
	unzip -o $(SOURCESDIR)/$(UPSTREAM_NDK_UPDATE_TARBALL) -d $(BUILDDIR)/ndk/tmp
	mkdir -p $(BUILDDIR)/openurl
	cd $(BUILDDIR)/openurl && tar xaf $(SOURCESDIR)/$(UPSTREAM_OPENURL_TARBALL)
	cp -r $(BUILDDIR)/openurl/libopenurl-$(UPSTREAM_OPENURL_VERSION)/include/* $(PREFIX)/$(TARGET_NAME)/SDK/include/include_h/
	mkdir -p $(BUILDDIR)/guigfxlib
	$(BUILDDIR)/prefix/bin/lha xw=$(BUILDDIR)/guigfxlib $(SOURCESDIR)/$(UPSTREAM_GUIGFX_TARBALL)
	cp -r $(BUILDDIR)/guigfxlib/include_os4/* $(PREFIX)/$(TARGET_NAME)/SDK/include/include_h/
	mkdir -p $(BUILDDIR)/renderlib
	$(BUILDDIR)/prefix/bin/lha xw=$(BUILDDIR)/renderlib $(SOURCESDIR)/$(UPSTREAM_RENDER_TARBALL)
	cp -r $(BUILDDIR)/renderlib/renderlib/include_os4/* $(PREFIX)/$(TARGET_NAME)/SDK/include/include_h/
	mkdir -p $(BUILDDIR)/codesets
	$(BUILDDIR)/prefix/bin/lha xw=$(BUILDDIR)/codesets $(SOURCESDIR)/$(UPSTREAM_CODESETS_TARBALL)
	cp -r $(BUILDDIR)/codesets/codesets/Developer/include/* $(PREFIX)/$(TARGET_NAME)/SDK/include/include_h/
	mkdir -p $(BUILDDIR)/AmiSSL
	lha xw=$(BUILDDIR)/AmiSSL $(SOURCESDIR)/$(UPSTREAM_AMISSL_TARBALL)
	cp -r $(BUILDDIR)/AmiSSL/AmiSSL/Developer/include/* $(PREFIX)/$(TARGET_NAME)/SDK/include/include_h/
	cp -r $(BUILDDIR)/AmiSSL/AmiSSL/Developer/lib/AmigaOS4/newlib/* $(PREFIX)/$(TARGET_NAME)/SDK/newlib/lib/
	cp -r $(BUILDDIR)/AmiSSL/AmiSSL/Developer/lib/AmigaOS4/libamisslstubs.a $(PREFIX)/$(TARGET_NAME)/SDK/newlib/lib/
	lha xfw=$(PREFIX)/$(TARGET_NAME)/SDK/ $(SOURCESDIR)/$(UPSTREAM_CLIB4_TARBALL)
	touch $@

###
# Rules to build and install lhasa
###

$(BUILDSTEPS)/lhasa.d: $(BUILDSTEPS)/lhasa-srcdir.d
	mkdir -p $(BUILDDIR)/lhasa
	mkdir -p $(BUILDDIR)/prefix
	cd $(BUILDDIR)/lhasa && $(SRCDIR)/lhasa/configure --prefix=$(BUILDDIR)/prefix
	cd $(BUILDDIR)/lhasa && make
	cd $(BUILDDIR)/lhasa && make install
	touch $@

$(BUILDSTEPS)/lhasa-srcdir.d: $(SOURCESDIR)/$(UPSTREAM_LHASA_TARBALL)
	unzip $(SOURCESDIR)/$(UPSTREAM_LHASA_TARBALL)
	mv lhasa-$(UPSTREAM_LHASA_VERSION) $(SRCDIR)/lhasa
	cd $(SRCDIR)/lhasa && autoreconf -i
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
	tar xJf $(SOURCESDIR)/$(UPSTREAM_GCC_TARBALL)
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

$(SOURCESDIR)/$(UPSTREAM_LHASA_TARBALL):
	wget -q -O $@ $(UPSTREAM_LHASA_URI)

$(SOURCESDIR)/$(UPSTREAM_NDK_TARBALL):
	wget -q -O $@ $(UPSTREAM_NDK_URI)

$(SOURCESDIR)/$(UPSTREAM_NDK_UPDATE_TARBALL):
	wget -q -O $@ $(UPSTREAM_NDK_UPDATE_URI)

$(SOURCESDIR)/$(UPSTREAM_CLIB4_TARBALL):
	wget -q -O $@ $(UPSTREAM_CLIB4_URI)

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

$(SOURCESDIR)/$(UPSTREAM_AMISSL_TARBALL):
	wget -q -O $@ $(UPSTREAM_AMISSL_URI)

###
# Rule to create directories for building
###

$(BUILDSTEPS)/buildsteps.d: | $(BUILDSTEPS) $(SOURCESDIR) $(SRCDIR)
	touch $@

$(BUILDSTEPS):
	mkdir -p $@

$(SOURCESDIR):
	mkdir -p $@

$(SRCDIR):
	mkdir -p $@
