#!/bin/make
# NetSurf cross compilation for amiga m68k

# sources

UPSTREAM_GCC_VERSION := 15.2.0
UPSTREAM_GCC_TARBALL := gcc-$(UPSTREAM_GCC_VERSION).tar.xz
UPSTREAM_GCC_URI := https://ftp.gnu.org/gnu/gcc/gcc-$(UPSTREAM_GCC_VERSION)/$(UPSTREAM_GCC_TARBALL)

UPSTREAM_BINUTILS_VERSION := 2.39
UPSTREAM_BINUTILS_TARBALL := binutils-$(UPSTREAM_BINUTILS_VERSION).tar.bz2
UPSTREAM_BINUTILS_URI := https://ftp.gnu.org/gnu/binutils/$(UPSTREAM_BINUTILS_TARBALL)

UPSTREAM_GMP_VERSION := 6.3.0
UPSTREAM_GMP_TARBALL := gmp-$(UPSTREAM_GMP_VERSION).tar.bz2
UPSTREAM_GMP_URI := http://ftp.gnu.org/gnu/gmp/$(UPSTREAM_GMP_TARBALL)

UPSTREAM_MPFR_VERSION := 4.2.2
UPSTREAM_MPFR_TARBALL := mpfr-$(UPSTREAM_MPFR_VERSION).tar.bz2
UPSTREAM_MPFR_URI := http://www.mpfr.org/mpfr-$(UPSTREAM_MPFR_VERSION)/$(UPSTREAM_MPFR_TARBALL)

UPSTREAM_MPC_VERSION := 1.3.1
UPSTREAM_MPC_TARBALL := mpc-$(UPSTREAM_MPC_VERSION).tar.gz
UPSTREAM_MPC_URI := https://ftp.gnu.org/gnu/mpc/$(UPSTREAM_MPC_TARBALL)

UPSTREAM_CLIB2_VERSION := 1_215
UPSTREAM_CLIB2_TARBALL := V$(UPSTREAM_CLIB2_VERSION).tar.gz
UPSTREAM_CLIB2_URI := https://github.com/adtools/clib2/archive/$(UPSTREAM_CLIB2_TARBALL)

UPSTREAM_NEWLIB_VERSION := 4c8cbd88df31cd06c89b258585ea5cd86cc9dc45
UPSTREAM_NEWLIB_TARBALL := $(UPSTREAM_NEWLIB_VERSION).tar.gz
UPSTREAM_NEWLIB_URI := https://github.com/AmigaPorts/newlib-cygwin/archive/$(UPSTREAM_NEWLIB_TARBALL)

UPSTREAM_NDK_TARBALL := NDK3.2R4.lha
UPSTREAM_NDK_URI := "http://hyperion-entertainment.biz/index.php/downloads?view=download&format=raw&file=126"

UPSTREAM_OPENURL_VERSION := 7.16
UPSTREAM_OPENURL_TARBALL := openurl-$(UPSTREAM_OPENURL_VERSION)
UPSTREAM_OPENURL_URI := https://github.com/jens-maus/libopenurl/archive/$(UPSTREAM_OPENURL_VERSION).tar.gz

UPSTREAM_AMISSL_VERSION := 5.17
UPSTREAM_AMISSL_TARBALL := AmiSSL-$(UPSTREAM_AMISSL_VERSION)-SDK.lha
UPSTREAM_AMISSL_URI := https://github.com/jens-maus/amissl/releases/download/$(UPSTREAM_AMISSL_VERSION)/$(UPSTREAM_AMISSL_TARBALL)

UPSTREAM_GUIGFX_TARBALL := guigfxlib.lha
UPSTREAM_GUIGFX_URI := http://neoscientists.org/~bifat/binarydistillery/$(UPSTREAM_GUIGFX_TARBALL)

UPSTREAM_RENDER_TARBALL := renderlib.lha
UPSTREAM_RENDER_URI := http://neoscientists.org/~bifat/binarydistillery/$(UPSTREAM_RENDER_TARBALL)

UPSTREAM_CODESETS_VERSION := 6.20
UPSTREAM_CODESETS_TARBALL := codesets-$(UPSTREAM_CODESETS_VERSION).lha
UPSTREAM_CODESETS_URI := https://github.com/jens-maus/libcodesets/releases/download/$(UPSTREAM_CODESETS_VERSION)/$(UPSTREAM_CODESETS_TARBALL)

# need to build a suitable version of bison or compiler build fails
UPSTREAM_BISON_VERSION := 2.7.1
UPSTREAM_BISON_TARBALL := bison-$(UPSTREAM_BISON_VERSION).tar.gz
UPSTREAM_BISON_URI := https://ftp.gnu.org/gnu/bison/$(UPSTREAM_BISON_TARBALL)

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
BISON_SRCDIR := $(SRCDIR)/bison

TARGET_NAME := m68k-unknown-amigaos

PREFIX ?= /opt/netsurf/$(TARGET_NAME)/cross

GCC_ENV_PARAMS := AUTOCONF=$(GCC_AUTOCONF) AUTOHEADER=$(GCC_AUTOHEADER) AUTOM4TE=$(GCC_AUTOM4TE) PATH="$(PREFIX)/bin:$(PATH)"

.PHONY: all clean distclean
all: $(BUILDSTEPS)/stage2.d

clean:
	rm -fr $(BUILDDIR)

distclean: clean
	rm -fr $(SOURCESDIR)

###
# Rules to build the full compiler
###

$(BUILDSTEPS)/stage2.d: $(BUILDSTEPS)/newlib.d
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
# Rules to build and install newlib
###

$(BUILDSTEPS)/newlib.d: $(BUILDSTEPS)/bootstrap-compiler.d $(BUILDSTEPS)/ndk.d $(BUILDSTEPS)/newlib-src.d
	cp -r $(BUILDDIR)/newlib/newlib/libc/include $(PREFIX)/$(TARGET_NAME)
	cp $(BUILDDIR)/newlib/newlib/libc/sys/amigaos/include/stabs.h $(PREFIX)/$(TARGET_NAME)/include
	cd $(BUILDDIR)/newlib/newlib && PATH="$(PREFIX)/bin:$(PATH)" CC="$(PREFIX)/bin/$(TARGET_NAME)-gcc" ./configure --host=$(TARGET_NAME) --prefix=$(PREFIX) --enable-newlib-io-long-long --enable-newlib-io-c99-formats --enable-newlib-reent-small --enable-newlib-mb --enable-newlib-long-time_t
	cd $(BUILDDIR)/newlib/newlib && PATH="$(PREFIX)/bin:$(PATH)" make
	cd $(BUILDDIR)/newlib/newlib && PATH="$(PREFIX)/bin:$(PATH)" make install
	touch $@


$(BUILDSTEPS)/newlib-src.d: $(SOURCESDIR)/$(UPSTREAM_NEWLIB_TARBALL)
	$(RM) -rf $(SOURCESDIR)/newlib-cygwin-$(UPSTREAM_NEWLIB_VERSION)
	$(RM) -rf $(BUILDDIR)/newlib
	cd $(SOURCESDIR) && tar xaf $(SOURCESDIR)/$(UPSTREAM_NEWLIB_TARBALL)
	mv $(SOURCESDIR)/newlib-cygwin-$(UPSTREAM_NEWLIB_VERSION) $(BUILDDIR)/newlib
	$(RM) -rf $(SOURCESDIR)/newlib-cygwin-$(UPSTREAM_NEWLIB_VERSION)
	for p in `ls $(RECIPES)/patches/newlib/*.p` ; do patch -d $(BUILDDIR)/newlib -p0 <$$p || exit $0 ; done
	for dir in `find $(RECIPES)/files/newlib/ -type d | grep -v '\.svn' | sed 's#$(RECIPES)/files/newlib##'` ; do mkdir -p $(BUILDDIR)/newlib$$dir ; done
	for file in `find $(RECIPES)/files/newlib/ -type f | grep -v '\.svn' | sed 's#$(RECIPES)/files/newlib##'` ; do cp -p $(RECIPES)/files/newlib$$file $(BUILDDIR)/newlib$$file ; done
	touch $@


###
# Rules to install the NDK
###

$(BUILDSTEPS)/ndk.d: $(SOURCESDIR)/$(UPSTREAM_NDK_TARBALL) $(SOURCESDIR)/$(UPSTREAM_OPENURL_TARBALL) $(SOURCESDIR)/$(UPSTREAM_GUIGFX_TARBALL) $(SOURCESDIR)/$(UPSTREAM_RENDER_TARBALL) $(SOURCESDIR)/$(UPSTREAM_CODESETS_TARBALL) $(SOURCESDIR)/$(UPSTREAM_AMISSL_TARBALL)
	mkdir -p $(BUILDDIR)/ndk/NDK3.2
	lha xw=$(BUILDDIR)/ndk/NDK3.2 $(SOURCESDIR)/$(UPSTREAM_NDK_TARBALL)
	for p in `ls $(RECIPES)/patches/ndk/*.p` ; do patch -d $(BUILDDIR)/ndk -p0 <$$p || exit $0 ; done
	for dir in `find $(RECIPES)/files/ndk/ -type d | grep -v '\.svn' | sed 's#$(RECIPES)/files/ndk##'` ; do mkdir -p $(BUILDDIR)/ndk/NDK3.2$$dir ; done
	for file in `find $(RECIPES)/files/ndk/ -type f | grep -v '\.svn' | sed 's#$(RECIPES)/files/ndk##'` ; do cp -p $(RECIPES)/files/ndk$$file $(BUILDDIR)/ndk/NDK3.2$$file ; done
	mkdir -p $(PREFIX)/$(TARGET_NAME)/ndk-include
	cp -r $(BUILDDIR)/ndk/NDK3.2/Include_H/* $(PREFIX)/$(TARGET_NAME)/ndk-include
	mkdir -p $(BUILDDIR)/openurl
	cd $(BUILDDIR)/openurl && tar xaf $(SOURCESDIR)/$(UPSTREAM_OPENURL_TARBALL)
	cp -r $(BUILDDIR)/openurl/libopenurl-$(UPSTREAM_OPENURL_VERSION)/include/* $(PREFIX)/$(TARGET_NAME)/ndk-include/
	mkdir -p $(BUILDDIR)/guigfxlib
	lha xw=$(BUILDDIR)/guigfxlib $(SOURCESDIR)/$(UPSTREAM_GUIGFX_TARBALL)
	cp -r $(BUILDDIR)/guigfxlib/include/* $(PREFIX)/$(TARGET_NAME)/ndk-include/
	mkdir -p $(BUILDDIR)/renderlib
	lha xw=$(BUILDDIR)/renderlib $(SOURCESDIR)/$(UPSTREAM_RENDER_TARBALL)
	cp -r $(BUILDDIR)/renderlib/renderlib/include/* $(PREFIX)/$(TARGET_NAME)/ndk-include/
	mkdir -p $(BUILDDIR)/codesets
	lha xw=$(BUILDDIR)/codesets $(SOURCESDIR)/$(UPSTREAM_CODESETS_TARBALL)
	cp -r $(BUILDDIR)/codesets/codesets/Developer/include/* $(PREFIX)/$(TARGET_NAME)/ndk-include/
	mkdir -p $(BUILDDIR)/AmiSSL
	lha xw=$(BUILDDIR)/AmiSSL $(SOURCESDIR)/$(UPSTREAM_AMISSL_TARBALL)
	cp -r $(BUILDDIR)/AmiSSL/AmiSSL/Developer/include/* $(PREFIX)/$(TARGET_NAME)/ndk-include/
	cp -r $(BUILDDIR)/AmiSSL/AmiSSL/Developer/lib/AmigaOS3/* $(PREFIX)/$(TARGET_NAME)/lib/
	touch $@

###
# Rules to build and install the bootstrap compiler
###

$(BUILDSTEPS)/bootstrap-compiler.d: $(BUILDSTEPS)/bison.d $(BUILDSTEPS)/srcdir-step3.d $(BUILDSTEPS)/binutils.d
	cd $(BUILDDIR) && $(GCC_ENV_PARAMS) $(GCC_SRCDIR)/configure \
				--prefix=$(PREFIX) \
				--target=$(TARGET_NAME) \
				--disable-threads \
				--disable-libssp \
				--disable-nls --disable-c-mbchar \
				--enable-languages=c --enable-checking=no \
				--enable-c99 --with-cross-host \
				--disable-multilib --without-x \
				--enable-maintainer-mode --disable-shared \
				--without-headers \
				--disable-libatomic
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
	tar xJf $(SOURCESDIR)/$(UPSTREAM_GCC_TARBALL)
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
	cd $(BUILDDIR)/binutils && CFLAGS="-m32" LDFLAGS="-m32" $(BINUTILS_SRCDIR)/configure --prefix=$(PREFIX) --target=$(TARGET_NAME) --disable-nls --disable-werror --enable-plugins
	cd $(BUILDDIR)/binutils && make
	cd $(BUILDDIR)/binutils && make install
	touch $@

$(BUILDSTEPS)/binutils-srcdir.d: $(SOURCESDIR)/$(UPSTREAM_BINUTILS_TARBALL)
	tar xjf $(SOURCESDIR)/$(UPSTREAM_BINUTILS_TARBALL)
	mv binutils-$(UPSTREAM_BINUTILS_VERSION) $(BINUTILS_SRCDIR)
	for p in `ls $(RECIPES)/patches/binutils/*.p` ; do patch -d $(BINUTILS_SRCDIR) -p0 <$$p ; done
	for dir in `find $(RECIPES)/files/binutils/ -type d | grep -v '\.svn' | sed 's#$(RECIPES)/files/binutils##'` ; do mkdir -p $(BINUTILS_SRCDIR)$$dir ; done
	for file in `find $(RECIPES)/files/binutils/ -type f | grep -v '\.svn' | sed 's#$(RECIPES)/files/binutils##'` ; do cp -p $(RECIPES)/files/binutils$$file $(BINUTILS_SRCDIR)$$file ; done
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

$(SOURCESDIR)/$(UPSTREAM_CLIB2_TARBALL):
	wget -q -O $@ $(UPSTREAM_CLIB2_URI)

$(SOURCESDIR)/$(UPSTREAM_NEWLIB_TARBALL):
	wget -q -O $@ $(UPSTREAM_NEWLIB_URI)

$(SOURCESDIR)/$(UPSTREAM_NDK_TARBALL):
	wget -q -O $@ $(UPSTREAM_NDK_URI)

$(SOURCESDIR)/$(UPSTREAM_OPENURL_TARBALL):
	wget -q -O $@ $(UPSTREAM_OPENURL_URI)

$(SOURCESDIR)/$(UPSTREAM_GUIGFX_TARBALL):
	wget -q -O $@ $(UPSTREAM_GUIGFX_URI)

$(SOURCESDIR)/$(UPSTREAM_RENDER_TARBALL):
	wget -q -O $@ $(UPSTREAM_RENDER_URI)

$(SOURCESDIR)/$(UPSTREAM_BISON_TARBALL):
	wget -q -O $@ $(UPSTREAM_BISON_URI)

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
