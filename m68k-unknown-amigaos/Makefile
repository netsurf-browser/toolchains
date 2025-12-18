#!/bin/make
# NetSurf cross compilation for amiga m68k

# sources

UPSTREAM_GCC_VERSION := 3.4.6
UPSTREAM_GCC_TARBALL := gcc_$(UPSTREAM_GCC_VERSION).tar.bz2

UPSTREAM_BINUTILS_VERSION := 2.14
UPSTREAM_BINUTILS_REVISION := 630
UPSTREAM_BINUTILS_PATCH := +adtools$(UPSTREAM_BINUTILS_REVISION)
UPSTREAM_BINUTILS_TARBALL := binutils_$(UPSTREAM_BINUTILS_VERSION)$(UPSTREAM_BINUTILS_PATCH).tar.bz2

UPSTREAM_GMP_VERSION := 4.3.2
UPSTREAM_GMP_TARBALL := gmp_$(UPSTREAM_GMP_VERSION).tar.bz2

# Would use 3.0.0, but that dislikes in-tree gmp sources
UPSTREAM_MPFR_VERSION := 2.4.2
UPSTREAM_MPFR_TARBALL := mpfr_$(UPSTREAM_MPFR_VERSION).tar.bz2

UPSTREAM_MPC_VERSION := 0.8.2
UPSTREAM_MPC_TARBALL := mpc_$(UPSTREAM_MPC_VERSION).tar.gz

UPSTREAM_CLIB2_VERSION := 1_214
UPSTREAM_CLIB2_TARBALL := clib2_$(UPSTREAM_CLIB2_VERSION).tar.gz

UPSTREAM_NDK_TARBALL := ndk-includes_3.9.tar.bz2

UPSTREAM_OPENURL_VERSION := 7.16
UPSTREAM_OPENURL_TARBALL := openurl_$(UPSTREAM_OPENURL_VERSION).tar.gz

UPSTREAM_AMISSL_VERSION := 5.17
UPSTREAM_AMISSL_TARBALL := amissl_$(UPSTREAM_AMISSL_VERSION).tar.xz

UPSTREAM_GUIGFX_VERSION := 20.0
UPSTREAM_GUIGFX_TARBALL := guigfxlib_$(UPSTREAM_GUIGFX_VERSION).tar.xz

UPSTREAM_RENDER_VERSION := 40.8
UPSTREAM_RENDER_TARBALL := renderlib_$(UPSTREAM_RENDER_VERSION).tar.xz

UPSTREAM_CODESETS_VERSION := 6.20
UPSTREAM_CODESETS_TARBALL := codesets_$(UPSTREAM_CODESETS_VERSION).tar.xz

# need to build a suitable version of bison or compiler build fails
UPSTREAM_BISON_VERSION := 2.7.1
UPSTREAM_BISON_TARBALL := bison_$(UPSTREAM_BISON_VERSION).tar.gz

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
BISON_SRCDIR := $(SRCDIR)/bison

TARGET_NAME := m68k-unknown-amigaos

PREFIX ?= /opt/netsurf/$(TARGET_NAME)/cross

PRISTINE_TAR := $(TOP)/../tools/pristine-tar
REPACK := $(TOP)/../tools/repack

GCC_ENV_PARAMS := AUTOCONF=$(GCC_AUTOCONF) AUTOHEADER=$(GCC_AUTOHEADER) AUTOM4TE=$(GCC_AUTOM4TE) PATH="$(PREFIX)/bin:$(PATH)"

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

$(BUILDSTEPS)/stage2.d: $(BUILDSTEPS)/clib2.d
	cd $(BUILDDIR) && $(GCC_ENV_PARAMS) make all
	cd $(BUILDDIR) && $(GCC_ENV_PARAMS) make install
	touch $@

###
# Rules to build and install Clib2
###

$(BUILDSTEPS)/clib2.d: $(BUILDSTEPS)/bootstrap-compiler.d $(BUILDSTEPS)/ndk.d $(BUILDSTEPS)/clib2-src.d
	cd $(BUILDDIR)/clib2 && PATH="$(PREFIX)/bin:$(PATH)" make -f GNUmakefile.68k
	cp -r $(BUILDDIR)/clib2/include $(PREFIX)/$(TARGET_NAME)
	cp -r $(BUILDDIR)/clib2/lib $(PREFIX)/$(TARGET_NAME)
	ln -sf $(PREFIX)/$(TARGET_NAME)/lib/ncrt0.o $(PREFIX)/$(TARGET_NAME)/lib/crt0.o
	touch $@


$(BUILDSTEPS)/clib2-src.d: $(SOURCESDIR)/$(UPSTREAM_CLIB2_TARBALL)
	$(RM) -rf $(SOURCESDIR)/clib2-$(UPSTREAM_CLIB2_VERSION)
	$(RM) -rf $(BUILDDIR)/clib2
	cd $(SOURCESDIR) && tar xaf $(SOURCESDIR)/$(UPSTREAM_CLIB2_TARBALL)
	mv $(SOURCESDIR)/clib2-$(UPSTREAM_CLIB2_VERSION)/library $(BUILDDIR)/clib2
	$(RM) -rf $(SOURCESDIR)/clib2-$(UPSTREAM_CLIB2_VERSION)
	for p in `ls $(RECIPES)/patches/clib2/*.p` ; do patch -d $(BUILDDIR)/clib2 -p0 <$$p || exit $0 ; done
	for dir in `find $(RECIPES)/files/clib2/ -type d | grep -v '\.svn' | sed 's#$(RECIPES)/files/clib2##'` ; do mkdir -p $(BUILDDIR)/clib2$$dir ; done
	for file in `find $(RECIPES)/files/clib2/ -type f | grep -v '\.svn' | sed 's#$(RECIPES)/files/clib2##'` ; do cp -p $(RECIPES)/files/clib2$$file $(BUILDDIR)/clib2$$file ; done
	touch $@


###
# Rules to install the NDK
###

$(BUILDSTEPS)/ndk.d: $(SOURCESDIR)/$(UPSTREAM_NDK_TARBALL) $(SOURCESDIR)/$(UPSTREAM_OPENURL_TARBALL) $(SOURCESDIR)/$(UPSTREAM_GUIGFX_TARBALL) $(SOURCESDIR)/$(UPSTREAM_RENDER_TARBALL) $(SOURCESDIR)/$(UPSTREAM_CODESETS_TARBALL) $(SOURCESDIR)/$(UPSTREAM_AMISSL_TARBALL)
	mkdir -p $(PREFIX)/$(TARGET_NAME)
	tar -C $(PREFIX)/$(TARGET_NAME) --strip-components=2 -xjf $(SOURCESDIR)/$(UPSTREAM_NDK_TARBALL)
	for p in `ls $(RECIPES)/patches/ndk/*.p` ; do patch -d $(PREFIX)/$(TARGET_NAME) -p0 <$$p ; done
	for dir in `find $(RECIPES)/files/ndk/ -type d | grep -v '\.svn' | sed 's#$(RECIPES)/files/ndk##'` ; do mkdir -p $(PREFIX)/$(TARGET_NAME)$$dir ; done
	for file in `find $(RECIPES)/files/ndk/ -type f | grep -v '\.svn' | sed 's#$(RECIPES)/files/ndk##'` ; do cp -p $(RECIPES)/files/ndk$$file $(PREFIX)/$(TARGET_NAME)$$file ; done
	mkdir -p $(BUILDDIR)/openurl
	tar -C $(BUILDDIR)/openurl -xaf $(SOURCESDIR)/$(UPSTREAM_OPENURL_TARBALL)
	cp -r $(BUILDDIR)/openurl/libopenurl-$(UPSTREAM_OPENURL_VERSION)/include/* $(PREFIX)/$(TARGET_NAME)/sys-include/
	mkdir -p $(BUILDDIR)/guigfxlib
	tar -C $(BUILDDIR)/guigfxlib -xaf $(SOURCESDIR)/$(UPSTREAM_GUIGFX_TARBALL)
	cp -r $(BUILDDIR)/guigfxlib/guigfxlib-$(UPSTREAM_GUIGFX_VERSION)/include/* $(PREFIX)/$(TARGET_NAME)/sys-include/
	mkdir -p $(BUILDDIR)/renderlib
	tar -C $(BUILDDIR)/renderlib -xaf $(SOURCESDIR)/$(UPSTREAM_RENDER_TARBALL)
	cp -r $(BUILDDIR)/renderlib/renderlib-$(UPSTREAM_RENDER_VERSION)/include/* $(PREFIX)/$(TARGET_NAME)/sys-include/
	mkdir -p $(BUILDDIR)/codesets
	tar -C $(BUILDDIR)/codesets -xaf $(SOURCESDIR)/$(UPSTREAM_CODESETS_TARBALL)
	cp -r $(BUILDDIR)/codesets/codesets-$(UPSTREAM_CODESETS_VERSION)/codesets/Developer/include/* $(PREFIX)/$(TARGET_NAME)/sys-include/
	mkdir -p $(BUILDDIR)/AmiSSL
	tar -C $(BUILDDIR)/AmiSSL -xaf $(SOURCESDIR)/$(UPSTREAM_AMISSL_TARBALL)
	cp -r $(BUILDDIR)/AmiSSL/amissl-$(UPSTREAM_AMISSL_VERSION)/AmiSSL/Developer/include/* $(PREFIX)/$(TARGET_NAME)/sys-include/
	cp -r $(BUILDDIR)/AmiSSL/amissl-$(UPSTREAM_AMISSL_VERSION)/AmiSSL/Developer/lib/AmigaOS3/* $(PREFIX)/$(TARGET_NAME)/lib/
	touch $@

###
# Rules to build and install the bootstrap compiler
###

$(BUILDSTEPS)/bootstrap-compiler.d: $(BUILDSTEPS)/bison.d $(BUILDSTEPS)/srcdir-step3.d $(BUILDSTEPS)/binutils.d
	cd $(BUILDDIR) && $(GCC_ENV_PARAMS) CFLAGS="-Wno-implicit-int -Wno-implicit-function-declaration -Wno-incompatible-pointer-types" $(GCC_SRCDIR)/configure \
				--prefix=$(PREFIX) \
				--target=$(TARGET_NAME) \
				--disable-threads \
				--disable-nls --disable-c-mbchar \
				--enable-languages=c --enable-checking=no \
				--enable-c99 --with-cross-host \
				--disable-multilib --without-x \
				--enable-maintainer-mode --disable-shared \
				--without-headers
	cd $(BUILDDIR) && $(GCC_ENV_PARAMS) make all-gcc
	cd $(BUILDDIR) && $(GCC_ENV_PARAMS) make install-gcc
	touch $@

###
# Rules to create the GCC source tree
###

$(BUILDSTEPS)/srcdir-step3.d: $(BUILDSTEPS)/srcdir-step2.d
	for p in `ls $(RECIPES)/patches/gcc/*.p` ; do patch -d $(GCC_SRCDIR) -p0 <$$p ; done
	for dir in `find $(RECIPES)/files/gcc/ -type d | grep -v '\.svn' | sed 's#$(RECIPES)/files/gcc##'` ; do mkdir -p $(GCC_SRCDIR)$$dir ; done
	for file in `find $(RECIPES)/files/gcc/ -type f | grep -v '\.svn' | sed 's#$(RECIPES)/files/gcc##'` ; do cp -p $(RECIPES)/files/gcc$$file $(GCC_SRCDIR)$$file ; done
	touch $@

$(BUILDSTEPS)/srcdir-step2.d: $(BUILDSTEPS)/srcdir-step1.d $(SOURCESDIR)/$(UPSTREAM_GMP_TARBALL) $(SOURCESDIR)/$(UPSTREAM_MPFR_TARBALL) $(SOURCESDIR)/$(UPSTREAM_MPC_TARBALL)
	tar xaf $(SOURCESDIR)/$(UPSTREAM_GMP_TARBALL)
	mv gmp-$(UPSTREAM_GMP_VERSION) $(GCC_SRCDIR)/gmp
	tar xaf $(SOURCESDIR)/$(UPSTREAM_MPFR_TARBALL)
	mv mpfr-$(UPSTREAM_MPFR_VERSION) $(GCC_SRCDIR)/mpfr
	tar xaf $(SOURCESDIR)/$(UPSTREAM_MPC_TARBALL)
	mv mpc-$(UPSTREAM_MPC_VERSION) $(GCC_SRCDIR)/mpc
	touch $@

$(BUILDSTEPS)/srcdir-step1.d: $(BUILDSTEPS)/$(UPSTREAM_GCC_TARBALL).d
	tar xjf $(SOURCESDIR)/$(UPSTREAM_GCC_TARBALL)
	mv gcc-$(UPSTREAM_GCC_VERSION) $(GCC_SRCDIR)
	touch $@

$(BUILDSTEPS)/$(UPSTREAM_GCC_TARBALL).d: $(BUILDSTEPS)/buildsteps.d $(SOURCESDIR)/$(UPSTREAM_GCC_TARBALL)
	touch $@

###
# Rules to build bison for the bootstrap compiler build
###

$(BUILDSTEPS)/bison.d: $(BUILDSTEPS)/srcdir-bison.d
	mkdir -p $(BUILDDIR)/bison
	cd $(BUILDDIR)/bison && $(BISON_SRCDIR)/configure --prefix=$(PREFIX)
	cd $(BUILDDIR)/bison && make
	cd $(BUILDDIR)/bison && make install
	touch $@

$(BUILDSTEPS)/srcdir-bison.d: $(BUILDSTEPS)/$(UPSTREAM_BISON_TARBALL).d
	tar axf $(SOURCESDIR)/$(UPSTREAM_BISON_TARBALL)
	mv bison-$(UPSTREAM_BISON_VERSION) $(BISON_SRCDIR)
	for p in `ls $(RECIPES)/patches/bison/*.p` ; do patch -d $(BISON_SRCDIR) -p0 <$$p ; done
	touch $@

$(BUILDSTEPS)/$(UPSTREAM_BISON_TARBALL).d: $(BUILDSTEPS)/buildsteps.d $(SOURCESDIR)/$(UPSTREAM_BISON_TARBALL)
	touch $@

###
# Rules to build and install binutils
###

# Ugh. Upstream binutils is not remotely 64-bit safe.
# Build a 32bit binary until this gets fixed
$(BUILDSTEPS)/binutils.d: $(BUILDSTEPS)/binutils-srcdir.d
	mkdir -p $(BUILDDIR)/binutils
	cd $(BUILDDIR)/binutils && CFLAGS="-m32 -Wno-implicit-int -Wno-implicit-function-declaration" LDFLAGS="-m32" $(BINUTILS_SRCDIR)/configure --prefix=$(PREFIX) --target=$(TARGET_NAME) --disable-nls --disable-werror
	cd $(BUILDDIR)/binutils && make
	cd $(BUILDDIR)/binutils && make install
	touch $@

$(BUILDSTEPS)/binutils-srcdir.d: $(BUILDSTEPS)/buildsteps.d $(SOURCESDIR)/$(UPSTREAM_BINUTILS_TARBALL)
	tar xaf $(SOURCESDIR)/$(UPSTREAM_BINUTILS_TARBALL)
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

$(SOURCESDIR)/$(UPSTREAM_CLIB2_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_NDK_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_OPENURL_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_GUIGFX_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_RENDER_TARBALL):
	cd $(TOP)/.. && $(PRISTINE_TAR) checkout $(subst $(SOURCESDIR)/,,$@) && mv $(TOP)/../$(subst $(SOURCESDIR)/,,$@) $@

$(SOURCESDIR)/$(UPSTREAM_BISON_TARBALL):
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

UPSTREAM_GCC_URI := http://ftp.gnu.org/gnu/gcc/gcc-$(UPSTREAM_GCC_VERSION)/$(subst _,-,$(UPSTREAM_GCC_TARBALL))
UPSTREAM_BINUTILS_URI := http://svn.code.sf.net/p/adtools/code/branches/binutils/$(UPSTREAM_BINUTILS_VERSION)/@$(UPSTREAM_BINUTILS_REVISION)
UPSTREAM_GMP_URI := http://ftp.gnu.org/gnu/gmp/$(subst _,-,$(UPSTREAM_GMP_TARBALL))
UPSTREAM_MPFR_URI := http://www.mpfr.org/mpfr-$(UPSTREAM_MPFR_VERSION)/$(subst _,-,$(UPSTREAM_MPFR_TARBALL))
UPSTREAM_MPC_URI := http://www.multiprecision.org/downloads/$(subst _,-,$(UPSTREAM_MPC_TARBALL))
UPSTREAM_CLIB2_URI := https://github.com/adtools/clib2/archive/V$(UPSTREAM_CLIB2_VERSION).tar.gz
UPSTREAM_NDK_URI := http://kas1e.mikendezign.com/zerohero_crosscompilers_backup/files/m68k-amigaos/ndk-3.9-includes.tar.bz2
UPSTREAM_OPENURL_URI := https://github.com/jens-maus/libopenurl/archive/$(UPSTREAM_OPENURL_VERSION).tar.gz
UPSTREAM_AMISSL_URI := https://github.com/jens-maus/amissl/releases/download/$(UPSTREAM_AMISSL_VERSION)/AmiSSL-$(UPSTREAM_AMISSL_VERSION)-SDK.lha
UPSTREAM_GUIGFX_URI := https://web.archive.org/web/20240715085500if_/http://neoscientists.org/~bifat/binarydistillery/guigfxlib.lha
UPSTREAM_RENDER_URI := https://web.archive.org/web/20240715085500if_/http://neoscientists.org/~bifat/binarydistillery/renderlib.lha
UPSTREAM_CODESETS_URI := https://github.com/jens-maus/libcodesets/releases/download/$(UPSTREAM_CODESETS_VERSION)/codesets-$(UPSTREAM_CODESETS_VERSION).lha
UPSTREAM_BISON_URI := https://ftp.gnu.org/gnu/bison/$(subst _,-,$(UPSTREAM_BISON_TARBALL))

$(BUILDSTEPS)/sources.d: $(BUILDSTEPS)/buildsteps.d $(BUILDDIR)/$(UPSTREAM_GCC_TARBALL) $(BUILDDIR)/$(UPSTREAM_BINUTILS_TARBALL) $(BUILDDIR)/$(UPSTREAM_GMP_TARBALL) $(BUILDDIR)/$(UPSTREAM_MPFR_TARBALL) $(BUILDDIR)/$(UPSTREAM_MPC_TARBALL) $(BUILDDIR)/$(UPSTREAM_CLIB2_TARBALL) $(BUILDDIR)/$(UPSTREAM_NDK_TARBALL) $(BUILDDIR)/$(UPSTREAM_OPENURL_TARBALL) $(BUILDDIR)/$(UPSTREAM_AMISSL_TARBALL) $(BUILDDIR)/$(UPSTREAM_GUIGFX_TARBALL) $(BUILDDIR)/$(UPSTREAM_RENDER_TARBALL) $(BUILDDIR)/$(UPSTREAM_CODESETS_TARBALL) $(BUILDDIR)/$(UPSTREAM_BISON_TARBALL)
	touch $@

$(BUILDDIR)/$(UPSTREAM_GCC_TARBALL):
	wget -q -O $@ $(UPSTREAM_GCC_URI)

$(BUILDDIR)/$(UPSTREAM_BINUTILS_TARBALL):
	rm -rf $(BUILDDIR)/sources/binutils
	svn export $(UPSTREAM_BINUTILS_URI) $(BUILDDIR)/sources/binutils-$(UPSTREAM_BINUTILS_VERSION)
	tar -C $(BUILDDIR)/sources -caf $@ binutils-$(UPSTREAM_BINUTILS_VERSION)

$(BUILDDIR)/$(UPSTREAM_GMP_TARBALL):
	wget -q -O $@ $(UPSTREAM_GMP_URI)

$(BUILDDIR)/$(UPSTREAM_MPFR_TARBALL):
	wget -q -O $@ $(UPSTREAM_MPFR_URI)

$(BUILDDIR)/$(UPSTREAM_MPC_TARBALL):
	wget -q -O $@ $(UPSTREAM_MPC_URI)

$(BUILDDIR)/$(UPSTREAM_CLIB2_TARBALL):
	wget -q -O $@ $(UPSTREAM_CLIB2_URI)

$(BUILDDIR)/$(UPSTREAM_NDK_TARBALL):
	wget -q -O $@ $(UPSTREAM_NDK_URI)

$(BUILDDIR)/$(UPSTREAM_OPENURL_TARBALL):
	wget -q -O $@ $(UPSTREAM_OPENURL_URI)

$(BUILDDIR)/$(UPSTREAM_AMISSL_TARBALL):
	rm -rf $(BUILDDIR)/sources/amissl
	mkdir -p $(BUILDDIR)/sources/amissl
	wget -q -O $(BUILDDIR)/sources/amissl/amissl-$(UPSTREAM_AMISSL_VERSION).lha $(UPSTREAM_AMISSL_URI)
	$(REPACK) $(BUILDDIR)/sources/amissl/amissl-$(UPSTREAM_AMISSL_VERSION).lha $@

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

$(BUILDDIR)/$(UPSTREAM_BISON_TARBALL):
	wget -q -O $@ $(UPSTREAM_MPC_URI)
