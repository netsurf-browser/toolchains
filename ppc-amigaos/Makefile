# NetSurf build script for Amigaos 4 PPC cross compliation toolchain.

###
# NDK pieces (only used for building minimised NDK)
###

UPSTREAM_LHASA_VERSION := 887d68ebca0cbad392af3709203745d202a2020b
UPSTREAM_LHASA_TARBALL := lhasa-$(UPSTREAM_LHASA_VERSION).zip

UPSTREAM_NDK_VERSION := 54.16
UPSTREAM_NDK_TARBALL := SDK_$(UPSTREAM_NDK_VERSION).lha

UPSTREAM_NDK_UPDATE_VERSION := update2
UPSTREAM_NDK_UPDATE_TARBALL := SDK_addon_final_edition_$(UPSTREAM_NDK_UPDATE_VERSION).zip

###
# Toolchain components
###

UPSTREAM_GCC_VERSION := 11.3.0
UPSTREAM_GCC_TARBALL := gcc_$(UPSTREAM_GCC_VERSION).tar.xz

UPSTREAM_BINUTILS_VERSION := 2.40
UPSTREAM_BINUTILS_TARBALL := binutils_$(UPSTREAM_BINUTILS_VERSION).tar.bz2

UPSTREAM_GMP_VERSION := 6.1.2
UPSTREAM_GMP_TARBALL := gmp_$(UPSTREAM_GMP_VERSION).tar.bz2

UPSTREAM_MPFR_VERSION := 3.1.6
UPSTREAM_MPFR_TARBALL := mpfr_$(UPSTREAM_MPFR_VERSION).tar.bz2

UPSTREAM_MPC_VERSION := 1.0.3
UPSTREAM_MPC_TARBALL := mpc_$(UPSTREAM_MPC_VERSION).tar.gz

# Pre-minimised NDK, built from pieces above.
# See the minimised-ndk make target for details.
MINIMISED_NDK_VERSION := $(UPSTREAM_NDK_VERSION)+$(UPSTREAM_NDK_UPDATE_VERSION)
MINIMISED_NDK_TARBALL := aos4-ndk_$(MINIMISED_NDK_VERSION).tar.xz

UPSTREAM_CLIB4_VERSION := 1.6.0
UPSTREAM_CLIB4_TARBALL := clib4_$(UPSTREAM_CLIB4_VERSION).tar.xz

UPSTREAM_OPENURL_VERSION := 7.16
UPSTREAM_OPENURL_TARBALL := openurl_$(UPSTREAM_OPENURL_VERSION).tar.gz

UPSTREAM_AMISSL_VERSION := 5.17
UPSTREAM_AMISSL_TARBALL := amissl_$(UPSTREAM_AMISSL_VERSION).tar.xz

UPSTREAM_ONCHIPMEM_VERSION := 53.1
UPSTREAM_ONCHIPMEM_TARBALL := onchipmem_$(UPSTREAM_ONCHIPMEM_VERSION).tar.xz

UPSTREAM_GUIGFX_VERSION := 20.0
UPSTREAM_GUIGFX_TARBALL := guigfxlib_$(UPSTREAM_GUIGFX_VERSION).tar.xz

UPSTREAM_RENDER_VERSION := 40.8
UPSTREAM_RENDER_TARBALL := renderlib_$(UPSTREAM_RENDER_VERSION).tar.xz

UPSTREAM_CODESETS_VERSION := 6.20
UPSTREAM_CODESETS_TARBALL := codesets_$(UPSTREAM_CODESETS_VERSION).tar.xz

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

PRISTINE_TAR := $(TOP)/../tools/pristine-tar
REPACK := $(TOP)/../tools/repack

.PHONY: all clean distclean source-archives
all: $(BUILDSTEPS)/stage2.d

source-archives: $(BUILDSTEPS)/sources.d

clean:
	rm -fr $(BUILDDIR)

distclean: clean
	rm -fr $(SOURCESDIR)

###
# Rules to build the full compiler
###

GCC_ENV_PARAMS := AUTOCONF=$(GCC_AUTOCONF) AUTOHEADER=$(GCC_AUTOHEADER) AUTOM4TE=$(GCC_AUTOM4TE) PATH="$(PREFIX)/bin:$(PATH)" CXXFLAGS="-fpermissive"

$(BUILDSTEPS)/stage2.d: $(BUILDSTEPS)/srcdir-step3.d $(BUILDSTEPS)/binutils.d $(BUILDSTEPS)/ndk.d
	cd $(BUILDDIR) && $(GCC_ENV_PARAMS) CFLAGS="-Wno-incompatible-pointer-types" $(GCC_SRCDIR)/configure \
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

$(BUILDSTEPS)/ndk.d: $(SOURCESDIR)/$(MINIMISED_NDK_TARBALL) $(SOURCESDIR)/$(UPSTREAM_OPENURL_TARBALL) $(SOURCESDIR)/$(UPSTREAM_ONCHIPMEM_TARBALL) $(SOURCESDIR)/$(UPSTREAM_GUIGFX_TARBALL) $(SOURCESDIR)/$(UPSTREAM_RENDER_TARBALL) $(SOURCESDIR)/$(UPSTREAM_CODESETS_TARBALL) $(SOURCESDIR)/$(UPSTREAM_AMISSL_TARBALL) $(SOURCESDIR)/$(UPSTREAM_CLIB4_TARBALL)
	mkdir -p $(BUILDDIR)/ndk
	tar -C $(BUILDDIR)/ndk -xaf $(SOURCESDIR)/$(MINIMISED_NDK_TARBALL) --strip-components=1
	mkdir -p $(BUILDDIR)/openurl
	tar -C $(BUILDDIR)/openurl -xaf $(SOURCESDIR)/$(UPSTREAM_OPENURL_TARBALL)
	cp -r $(BUILDDIR)/openurl/libopenurl-$(UPSTREAM_OPENURL_VERSION)/include/* $(BUILDDIR)/ndk/include/include_h/
	mkdir -p $(BUILDDIR)/guigfxlib
	tar -C $(BUILDDIR)/guigfxlib -xaf $(SOURCESDIR)/$(UPSTREAM_GUIGFX_TARBALL)
	cp -r $(BUILDDIR)/guigfxlib/guigfxlib-$(UPSTREAM_GUIGFX_VERSION)/include_os4/* $(BUILDDIR)/ndk/include/include_h/
	mkdir -p $(BUILDDIR)/renderlib
	tar -C $(BUILDDIR)/renderlib -xaf $(SOURCESDIR)/$(UPSTREAM_RENDER_TARBALL)
	cp -r $(BUILDDIR)/renderlib/renderlib-$(UPSTREAM_RENDER_VERSION)/include_os4/* $(BUILDDIR)/ndk/include/include_h/
	mkdir -p $(BUILDDIR)/codesets
	tar -C $(BUILDDIR)/codesets -xaf $(SOURCESDIR)/$(UPSTREAM_CODESETS_TARBALL)
	cp -r $(BUILDDIR)/codesets/codesets-$(UPSTREAM_CODESETS_VERSION)/codesets/Developer/include/* $(BUILDDIR)/ndk/include/include_h/
	mkdir -p $(BUILDDIR)/AmiSSL
	tar -C $(BUILDDIR)/AmiSSL -xaf $(SOURCESDIR)/$(UPSTREAM_AMISSL_TARBALL)
	cp -r $(BUILDDIR)/AmiSSL/amissl-$(UPSTREAM_AMISSL_VERSION)/AmiSSL/Developer/include/* $(BUILDDIR)/ndk/include/include_h/
	cp -r $(BUILDDIR)/AmiSSL/amissl-$(UPSTREAM_AMISSL_VERSION)/AmiSSL/Developer/lib/AmigaOS4/newlib/* $(BUILDDIR)/ndk/newlib/lib/
	cp -r $(BUILDDIR)/AmiSSL/amissl-$(UPSTREAM_AMISSL_VERSION)/AmiSSL/Developer/lib/AmigaOS4/libamisslstubs.a $(BUILDDIR)/ndk/newlib/lib/
	mkdir -p $(BUILDDIR)/clib4
	tar -C $(BUILDDIR)/clib4 -xaf $(SOURCESDIR)/$(UPSTREAM_CLIB4_TARBALL)
	cp -r $(BUILDDIR)/clib4/clib4-$(UPSTREAM_CLIB4_VERSION) $(BUILDDIR)/ndk/clib4
	rm -rf $(PREFIX)/$(TARGET_NAME)/SDK
	mkdir -p $(PREFIX)/$(TARGET_NAME)/SDK
	cp -r $(BUILDDIR)/ndk/* $(PREFIX)/$(TARGET_NAME)/SDK
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
	cd $(BUILDDIR)/binutils && CFLAGS="-Wno-implicit-function-declaration" $(BINUTILS_SRCDIR)/configure --prefix=$(PREFIX) --target=$(TARGET_NAME) --disable-nls --enable-plugins --disable-werror
	cd $(BUILDDIR)/binutils && make
	cd $(BUILDDIR)/binutils && make install
	touch $@

$(BUILDSTEPS)/binutils-srcdir.d: $(BUILDSTEPS)/buildsteps.d $(SOURCESDIR)/$(UPSTREAM_BINUTILS_TARBALL)
	tar xjf $(SOURCESDIR)/$(UPSTREAM_BINUTILS_TARBALL)
	mv binutils-$(UPSTREAM_BINUTILS_VERSION) $(BINUTILS_SRCDIR)
	for p in `ls $(RECIPES)/patches/binutils/*.p` ; do patch -d $(BINUTILS_SRCDIR) -p0 <$$p ; done
	touch $@

###
# Rules to fetch upstream sources
###

$(SOURCESDIR)/$(UPSTREAM_GCC_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_GMP_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_MPFR_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_MPC_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_BINUTILS_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(MINIMISED_NDK_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_CLIB4_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_OPENURL_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_ONCHIPMEM_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_GUIGFX_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_RENDER_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_CODESETS_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_AMISSL_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

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

###
# Rules to build source archives (via source-archives target)
###

UPSTREAM_LHASA_URI := https://github.com/fragglet/lhasa/archive/$(UPSTREAM_LHASA_VERSION).zip
UPSTREAM_NDK_URI := "http://hyperion-entertainment.biz/index.php/downloads?view=download&format=raw&file=127"
UPSTREAM_NDK_UPDATE_URI := http://kas1e.mikendezign.com/aos4/SDK_addon_final_edition_$(UPSTREAM_NDK_UPDATE_VERSION).zip
UPSTREAM_GCC_URI := http://ftp.gnu.org/gnu/gcc/gcc-$(UPSTREAM_GCC_VERSION)/$(subst _,-,$(UPSTREAM_GCC_TARBALL))
UPSTREAM_BINUTILS_URI := ftp://ftp.gnu.org/gnu/binutils/$(subst _,-,$(UPSTREAM_BINUTILS_TARBALL))
UPSTREAM_GMP_URI := http://ftp.gnu.org/gnu/gmp/$(subst _,-,$(UPSTREAM_GMP_TARBALL))
UPSTREAM_MPFR_URI := http://www.mpfr.org/mpfr-$(UPSTREAM_MPFR_VERSION)/$(subst _,-,$(UPSTREAM_MPFR_TARBALL))
UPSTREAM_MPC_URI := http://ftp.gnu.org/gnu/mpc/$(subst _,-,$(UPSTREAM_MPC_TARBALL))
UPSTREAM_CLIB4_URI := https://github.com/AmigaLabs/clib4/releases/download/v$(UPSTREAM_CLIB4_VERSION)/clib4-v$(UPSTREAM_CLIB4_VERSION).lha
UPSTREAM_OPENURL_URI := https://github.com/jens-maus/libopenurl/archive/$(UPSTREAM_OPENURL_VERSION).tar.gz
UPSTREAM_AMISSL_URI := https://github.com/jens-maus/amissl/releases/download/$(UPSTREAM_AMISSL_VERSION)/AmiSSL-$(UPSTREAM_AMISSL_VERSION)-SDK.lha
UPSTREAM_ONCHIPMEM_URI := http://www.acube-systems.biz/download/onchipmem_res-$(UPSTREAM_ONCHIPMEM_VERSION).lha
UPSTREAM_GUIGFX_URI := https://web.archive.org/web/20240715085500if_/http://neoscientists.org/~bifat/binarydistillery/guigfxlib.lha
UPSTREAM_RENDER_URI := https://web.archive.org/web/20240715085500if_/http://neoscientists.org/~bifat/binarydistillery/renderlib.lha
UPSTREAM_CODESETS_URI := https://github.com/jens-maus/libcodesets/releases/download/$(UPSTREAM_CODESETS_VERSION)/codesets-$(UPSTREAM_CODESETS_VERSION).lha

$(BUILDSTEPS)/sources.d: $(BUILDSTEPS)/buildsteps.d $(BUILDDIR)/$(MINIMISED_NDK_TARBALL) $(BUILDDIR)/$(UPSTREAM_GCC_TARBALL) $(BUILDDIR)/$(UPSTREAM_BINUTILS_TARBALL) $(BUILDDIR)/$(UPSTREAM_GMP_TARBALL) $(BUILDDIR)/$(UPSTREAM_MPFR_TARBALL) $(BUILDDIR)/$(UPSTREAM_MPC_TARBALL) $(BUILDDIR)/$(UPSTREAM_CLIB4_TARBALL) $(BUILDDIR)/$(UPSTREAM_OPENURL_TARBALL) $(BUILDDIR)/$(UPSTREAM_AMISSL_TARBALL) $(BUILDDIR)/$(UPSTREAM_ONCHIPMEM_TARBALL) $(BUILDDIR)/$(UPSTREAM_GUIGFX_TARBALL) $(BUILDDIR)/$(UPSTREAM_RENDER_TARBALL) $(BUILDDIR)/$(UPSTREAM_CODESETS_TARBALL)
	touch $@

$(BUILDSTEPS)/lhasa.d:
	rm -rf $(BUILDDIR)/lhasa $(BUILDDIR)/prefix
	mkdir -p $(BUILDDIR)/lhasa
	mkdir -p $(BUILDDIR)/prefix
	wget -q -O $(BUILDDIR)/lhasa/$(UPSTREAM_LHASA_TARBALL) $(UPSTREAM_LHASA_URI)
	unzip $(BUILDDIR)/lhasa/$(UPSTREAM_LHASA_TARBALL) -d $(BUILDDIR)/lhasa
	cd $(BUILDDIR)/lhasa/lhasa-$(UPSTREAM_LHASA_VERSION) && autoreconf -i
	cd $(BUILDDIR)/lhasa/lhasa-$(UPSTREAM_LHASA_VERSION) && ./configure --prefix=$(BUILDDIR)/prefix
	cd $(BUILDDIR)/lhasa/lhasa-$(UPSTREAM_LHASA_VERSION) && make
	cd $(BUILDDIR)/lhasa/lhasa-$(UPSTREAM_LHASA_VERSION) && make install
	touch $@
$(BUILDDIR)/$(MINIMISED_NDK_TARBALL): $(BUILDSTEPS)/lhasa.d
	rm -rf $(BUILDDIR)/ndk-src
	mkdir -p $(BUILDDIR)/ndk-src
	wget -q -O $(BUILDDIR)/ndk-src/$(UPSTREAM_NDK_TARBALL) $(UPSTREAM_NDK_URI)
	wget -q -O $(BUILDDIR)/ndk-src/$(UPSTREAM_NDK_UPDATE_TARBALL) $(UPSTREAM_NDK_UPDATE_URI)
	mkdir -p $(BUILDDIR)/ndk-src/tmp/
	$(BUILDDIR)/prefix/bin/lha xw=$(BUILDDIR)/ndk-src $(BUILDDIR)/ndk-src/$(UPSTREAM_NDK_TARBALL)
	$(BUILDDIR)/prefix/bin/lha xw=$(BUILDDIR)/ndk-src/tmp $(BUILDDIR)/ndk-src/SDK_Install/base.lha
	$(BUILDDIR)/prefix/bin/lha xw=$(BUILDDIR)/ndk-src/tmp $(BUILDDIR)/ndk-src/SDK_Install/execsg_sdk-54.31.lha
	$(BUILDDIR)/prefix/bin/lha xw=$(BUILDDIR)/ndk-src/tmp $(BUILDDIR)/ndk-src/SDK_Install/newlib-53.80.lha
	$(BUILDDIR)/prefix/bin/lha xw=$(BUILDDIR)/ndk-src/tmp $(BUILDDIR)/ndk-src/SDK_Install/clib2.lha
	$(BUILDDIR)/prefix/bin/lha xw=$(BUILDDIR)/ndk-src/tmp $(BUILDDIR)/ndk-src/SDK_Install/pthreads-53.12.lha
	unzip -o $(BUILDDIR)/ndk-src/$(UPSTREAM_NDK_UPDATE_TARBALL) -d $(BUILDDIR)/ndk-src/tmp
	mkdir -p $(BUILDDIR)/ndk-src/$(subst _,-,$(basename $(basename $(notdir $@))))
	cp -r $(BUILDDIR)/ndk-src/tmp/Include $(BUILDDIR)/ndk-src/$(subst _,-,$(basename $(basename $(notdir $@))))/include
	cp -r $(BUILDDIR)/ndk-src/tmp/newlib $(BUILDDIR)/ndk-src/$(subst _,-,$(basename $(basename $(notdir $@))))/newlib
	cp -r $(BUILDDIR)/ndk-src/tmp/clib2 $(BUILDDIR)/ndk-src/$(subst _,-,$(basename $(basename $(notdir $@))))/clib2
	cp -r $(BUILDDIR)/ndk-src/tmp/Local/common/include/* $(BUILDDIR)/ndk-src/$(subst _,-,$(basename $(basename $(notdir $@))))/newlib/include/
	cp -r $(BUILDDIR)/ndk-src/tmp/Local/newlib/lib/* $(BUILDDIR)/ndk-src/$(subst _,-,$(basename $(basename $(notdir $@))))/newlib/lib/
	cp -r $(BUILDDIR)/ndk-src/tmp/SDK/Include/* $(BUILDDIR)/ndk-src/$(subst _,-,$(basename $(basename $(notdir $@))))/include
	cp -r $(BUILDDIR)/ndk-src/tmp/SDK/local/newlib/lib/* $(BUILDDIR)/ndk-src/$(subst _,-,$(basename $(basename $(notdir $@))))/newlib/lib/
	cp -r $(BUILDDIR)/ndk-src/tmp/SDK/local/common/include/* $(BUILDDIR)/ndk-src/$(subst _,-,$(basename $(basename $(notdir $@))))/newlib/include/
	tar -C $(BUILDDIR)/ndk-src -caf $@ $(subst _,-,$(basename $(basename $(notdir $@))))

$(BUILDDIR)/$(UPSTREAM_GCC_TARBALL):
	wget -q -O $@ $(UPSTREAM_GCC_URI)

$(BUILDDIR)/$(UPSTREAM_BINUTILS_TARBALL):
	wget -q -O $@ $(UPSTREAM_GCC_URI)

$(BUILDDIR)/$(UPSTREAM_GMP_TARBALL):
	wget -q -O $@ $(UPSTREAM_GMP_URI)

$(BUILDDIR)/$(UPSTREAM_MPFR_TARBALL):
	wget -q -O $@ $(UPSTREAM_MPFR_URI)

$(BUILDDIR)/$(UPSTREAM_MPC_TARBALL):
	wget -q -O $@ $(UPSTREAM_MPC_URI)

$(BUILDDIR)/$(UPSTREAM_CLIB4_TARBALL):
	wget -q -O $@ $(UPSTREAM_CLIB4_URI)

$(BUILDDIR)/$(UPSTREAM_OPENURL_TARBALL):
	wget -q -O $@ $(UPSTREAM_OPENURL_URI)

$(BUILDDIR)/$(UPSTREAM_AMISSL_TARBALL):
	rm -rf $(BUILDDIR)/sources/amissl
	mkdir -p $(BUILDDIR)/sources/amissl
	wget -q -O $(BUILDDIR)/sources/amissl/amissl-$(UPSTREAM_AMISSL_VERSION).lha $(UPSTREAM_AMISSL_URI)
	$(REPACK) $(BUILDDIR)/sources/amissl/amissl-$(UPSTREAM_AMISSL_VERSION).lha $@

$(BUILDDIR)/$(UPSTREAM_ONCHIPMEM_TARBALL):
	rm -rf $(BUILDDIR)/sources/onchipmem
	mkdir -p $(BUILDDIR)/sources/onchipmem
	wget -q -O $(BUILDDIR)/sources/onchipmem/onchipmem-$(UPSTREAM_ONCHIPMEM_VERSION).lha $(UPSTREAM_ONCHIPMEM_URI)
	$(REPACK) $(BUILDDIR)/sources/onchipmem/onchipmem-$(UPSTREAM_ONCHIPMEM_VERSION).lha $@

$(BUILDDIR)/$(UPSTREAM_GUIGFX_TARBALL):
	rm -rf $(BUILDDIR)/sources/guigfx
	mkdir -p $(BUILDDIR)/sources/guigfx
	wget -q -O $(BUILDDIR)/sources/guigfx/guigfx-$(UPSTREAM_GUIGFX_VERSION).lha $(UPSTREAM_GUIGFX_URI)
	$(REPACK) $(BUILDDIR)/sources/guigfx/guigfx-$(UPSTREAM_GUIGFX_VERSION).lha $@

$(BUILDDIR)/$(UPSTREAM_RENDER_TARBALL):
	rm -rf $(BUILDDIR)/sources/render
	mkdir -p $(BUILDDIR)/sources/render
	wget -q -O $(BUILDDIR)/sources/render/render-$(UPSTREAM_RENDER_VERSION).lha $(UPSTREAM_RENDER_URI)
	$(REPACK) $(BUILDDIR)/sources/render/render-$(UPSTREAM_RENDER_VERSION).lha $@

$(BUILDDIR)/$(UPSTREAM_CODESETS_TARBALL):
	rm -rf $(BUILDDIR)/sources/codesets
	mkdir -p $(BUILDDIR)/sources/codesets
	wget -q -O $(BUILDDIR)/sources/codesets/codesets-$(UPSTREAM_CODESETS_VERSION).lha $(UPSTREAM_CODESETS_URI)
	$(REPACK) $(BUILDDIR)/sources/codesets/codesets-$(UPSTREAM_CODESETS_VERSION).lha $@
