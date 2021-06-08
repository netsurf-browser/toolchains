#
# NetSurf Project toolchain SDK build
#
# Required environment:
#
# GCCSDK_INSTALL_ENV      -- /path/to/install/prefix
# GCCSDK_INSTALL_CROSSBIN -- /path/to/toolchain/bin
#
# supported targets are:
#                       arm-unknown-riscos  
#                       m68k-atari-mint
#			m5475-atari-mint
#                       ppc-amigaos 
#                       i686-w64-mingw32    
#                       m68k-unknown-amigaos

ifeq ($(GCCSDK_INSTALL_ENV),)
  $(error GCCSDK_INSTALL_ENV not set)
endif

ifeq ($(GCCSDK_INSTALL_CROSSBIN),)
  $(error GCCSDK_INSTALL_CROSSBIN not set)
endif

# Upstream package versions
# https://zlib.net/
VERSION_ZLIB := 1.2.11
# https://www.gnu.org/software/libiconv/
VERSION_LIBICONV := 1.16
# https://laurikari.net/tre/
VERSION_LIBTRE := 0.8.0
# https://www.openssl.org/
VERSION_OPENSSL := 1.1.0l
# http://www.libpng.org/pub/png/libpng.html
VERSION_LIBPNG := 1.6.37
# http://libjpeg.sourceforge.net/
VERSION_LIBJPEG := 8d
# https://libjpeg-turbo.org/
# libjpeg turbo have dropped autotools support VERSION_LIBJPEG_TURBO := 2.0.2
VERSION_LIBJPEG_TURBO := 1.5.3
# https://developers.google.com/speed/webp/
VERSION_LIBWEBP := 1.0.3
# https://c-ares.haxx.se/
VERSION_LIBCARES := 1.15.0
# https://curl.haxx.se/
VERSION_LIBCURL := 7.77.0
VERSION_LIBGNURX := 2.5.1
# http://mission-base.com/peter/source/
VERSION_LIBPBL := 1_04
VERSION_LIBCF := CVS-20130415
# https://github.com/libexpat/libexpat
VERSION_LIBEXPAT := 2.2.9
VERSION_LDG := 2.32
VERSION_WINDOM := 2.0.1
VERSION_HERMES := 1.3.3
# https://www.freetype.org/
VERSION_FREETYPE := 2.10.1

# Path
path__ := $(GCCSDK_INSTALL_CROSSBIN):$(PATH)

# Tools
cc__ := $(word 1,$(wildcard $(GCCSDK_INSTALL_CROSSBIN)/*gcc))
cxx__ := $(word 1,$(wildcard $(GCCSDK_INSTALL_CROSSBIN)/*g++))
ar__ := $(word 1,$(wildcard $(GCCSDK_INSTALL_CROSSBIN)/*ar))
ranlib__ := $(word 1,$(wildcard $(GCCSDK_INSTALL_CROSSBIN)/*ranlib))

# Flags
cflags__ := -I$(GCCSDK_INSTALL_ENV)/include
cppflags__ := -I$(GCCSDK_INSTALL_ENV)/include
ldflags__ := -L$(GCCSDK_INSTALL_ENV)/lib
pkg_config_libdir__ := $(GCCSDK_INSTALL_ENV)/lib/pkgconfig

# Target
TARGET := $(shell $(cc__) -dumpmachine)

# Environment
# Newer autotools objects to -I appearing in CFLAGS
cleanenv := PATH="$(path__)" CC="$(cc__)" CXX="$(cxx__)" AR="$(ar__)" RANLIB="$(ranlib__)" CPPFLAGS="$(cppflags__)" LDFLAGS="$(ldflags__)" PKG_CONFIG_LIBDIR="$(pkg_config_libdir__)"
env := $(cleanenv) CFLAGS="$(cflags__)"

RECIPES := $(CURDIR)/recipes
SOURCEDIR := $(CURDIR)/sources
BUILDDIR := $(CURDIR)/builddir-$(TARGET)
BUILDSTEPS := $(BUILDDIR)/build-steps

# Tools
# patch: refuse to countenance fuzz
PATCH := patch -F0
FETCHSRC := $(CURDIR)/fetchsrc

# Compute the SDK components
SDK_ITEMS :=

COMMON_SDK_ITEMS := zlib.d openssl.d libpng.d libcares.d libcurl.d libexpat.d

LIBCURL_DISABLES := $(patsubst %,--disable-%,ftp file dict ldap ldaps rtsp telnet gopher tftp pop3 imap smb smtp manual)
FREETYPE_ENV := LIBPNG_CFLAGS=$$($(GCCSDK_INSTALL_ENV)/bin/libpng-config --cflags) LIBPNG_LDFLAGS=$$($(GCCSDK_INSTALL_ENV)/bin/libpng-config --ldflags)

# per target environments
LIBJPEG_TURBO_BUILDDIR:=$(BUILDDIR)/libjpegturbo

# Target Specific setup
ifeq ($(TARGET),arm-unknown-riscos)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, $(COMMON_SDK_ITEMS) libjpeg-turbo.d libwebp.d)
  EXTRAARGS_LIBCARES := --disable-shared
  EXTRAARGS_LIBCURL := --enable-nonblocking --enable-ares --with-random="/dev/urandom" $(LIBCURL_DISABLES)
  EXTRAARGS_OPENSSL := no-asm
endif

ifeq ($(TARGET),m68k-atari-mint)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d $(COMMON_SDK_ITEMS) libjpeg.d libcf.d ldg.d windom.d hermes.d freetype.d)
  EXTRAARGS_LIBCARES := --disable-shared
  EXTRAARGS_LIBCURL := --enable-optimize --enable-nonblocking --enable-ares --with-random="/dev/urandom" --disable-manual --disable-curldebug --disable-thread --disable-threaded-resolver 
  EXTRAARGS_LIBICONV := --enable-extra-encodings
  LIBICONV_ENV := CFLAGS="-DPATH_MAX=1024"
endif

ifeq ($(TARGET),m5475-atari-mint)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d $(COMMON_SDK_ITEMS) libjpeg.d libcf.d ldg.d windom.d hermes.d freetype.d)
  EXTRAARGS_LIBCARES := --disable-shared
  EXTRAARGS_LIBCURL := --enable-optimize --enable-nonblocking --enable-ares --with-random="/dev/urandom" --disable-manual --disable-curldebug --disable-thread --disable-threaded-resolver
  EXTRAARGS_LIBICONV := --enable-extra-encodings
  LIBICONV_ENV := CFLAGS="-DPATH_MAX=1024"
  EXTRAARGS_OPENSSL := no-asm
endif

ifeq ($(TARGET),ppc-amigaos)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d libtre.d libpbl.d $(COMMON_SDK_ITEMS) libjpeg-turbo.d)
  EXTRAARGS_LIBCARES := --disable-shared
  EXTRAARGS_LIBCURL := --disable-ntlm-wb --without-random --enable-threaded-resolver --disable-ipv6 LIBS=-lpthread
endif

ifeq ($(TARGET),i686-w64-mingw32)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d libgnurx.d $(COMMON_SDK_ITEMS) libjpeg.d libwebp.d)
  EXTRAARGS_LIBCARES := --disable-shared
  EXTRAARGS_LIBCURL := --enable-nonblocking --enable-ares --disable-ldap --without-random
  LIBCURL_ENV := CFLAGS="-DCARES_STATICLIB" LIBS="-lgdi32 -lcrypt32"
  EXTRAARGS_OPENSSL := no-asm enable-capieng
endif

ifeq ($(TARGET),m68k-unknown-amigaos)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, roadshow.d libiconv.d libtre.d libpbl.d $(COMMON_SDK_ITEMS) libjpeg.d)
  EXTRAARGS_LIBCARES := --disable-shared --without-random
  EXTRAARGS_LIBCURL := --disable-ntlm-wb --without-random --disable-threaded-resolver --with-amissl --with-zlib --disable-rt --disable-ipv6
  LIBICONV_ENV := CFLAGS="-DPATH_MAX=1024"
  LIBJPEG_ENV := LDFLAGS="-lm"
  LIBJPEG_TURBO_ENV := LDFLAGS="-lm"
  LIBCURL_ENV := CFLAGS="-I$(GCCSDK_INSTALL_ENV)/netinclude -D__NO_NET_API" LIBS="-lm"
  LIBEXPAT_ENV := LDFLAGS="-lm"
endif

ifeq ($(SDK_ITEMS),)
  $(error Unable to compute SDK components for target $(TARGET))
endif

.PHONY: all clean distclean
all: $(SDK_ITEMS)

clean:
	rm -fr $(BUILDDIR)

distclean: clean
	rm -fr $(SOURCEDIR)

# Sourcedir
$(BUILDSTEPS)/sourcedir.d: $(BUILDSTEPS)/builddir.d
	mkdir -p $(SOURCEDIR)
	touch $@

# Builddir
$(BUILDSTEPS)/builddir.d:
	mkdir -p $(BUILDSTEPS)
	touch $@

# zlib
$(BUILDSTEPS)/zlib.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/zlib-src.d
	cd $(BUILDDIR)/zlib/zlib-$(VERSION_ZLIB) && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --static
	cd $(BUILDDIR)/zlib/zlib-$(VERSION_ZLIB) && $(env) make install
	touch $@

$(BUILDSTEPS)/zlib-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/zlib-$(VERSION_ZLIB).tar.gz
	mkdir -p $(BUILDDIR)/zlib
	cd $(BUILDDIR)/zlib && tar xaf $(SOURCEDIR)/zlib-$(VERSION_ZLIB).tar.gz
	for p in `ls $(RECIPES)/patches/zlib/*.p` ; do $(PATCH) -d $(BUILDDIR)/zlib/zlib-$(VERSION_ZLIB) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/zlib/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/zlib/$(TARGET)/*.p` ; do $(PATCH) -d $(BUILDDIR)/zlib/zlib-$(VERSION_ZLIB) -p0 <$$p ; done
endif
	touch $@

$(SOURCEDIR)/zlib-$(VERSION_ZLIB).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://zlib.net/$(subst $(SOURCEDIR)/,,$@)" $@

# libiconv
$(BUILDSTEPS)/libiconv.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/libiconv-src.d
	mkdir -p $(BUILDDIR)/libiconv
	cd $(BUILDDIR)/libiconv && tar xaf $(SOURCEDIR)/libiconv-$(VERSION_LIBICONV).tar.gz
	for p in `ls $(RECIPES)/patches/libiconv/*.p` ; do $(PATCH) -d $(BUILDDIR)/libiconv/libiconv-$(VERSION_LIBICONV) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/libiconv/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/libiconv/$(TARGET)/*.p` ; do $(PATCH) -d $(BUILDDIR)/libiconv/libiconv-$(VERSION_LIBICONV) -p0 <$$p ; done
endif
	cd $(BUILDDIR)/libiconv/libiconv-$(VERSION_LIBICONV) && $(env) $(LIBICONV_ENV) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared $(EXTRAARGS_LIBICONV)
	cd $(BUILDDIR)/libiconv/libiconv-$(VERSION_LIBICONV) && $(env) make install
	touch $@

$(BUILDSTEPS)/libiconv-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/libiconv-$(VERSION_LIBICONV).tar.gz
	touch $@

$(SOURCEDIR)/libiconv-$(VERSION_LIBICONV).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://ftp.gnu.org/pub/gnu/libiconv/$(subst $(SOURCEDIR)/,,$@)" $@

# libgnurx
$(BUILDSTEPS)/libgnurx.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/libgnurx-src.d
	cd $(BUILDDIR)/libgnurx/mingw-libgnurx-$(VERSION_LIBGNURX) && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --enable-static --disable-shared
	cd $(BUILDDIR)/libgnurx/mingw-libgnurx-$(VERSION_LIBGNURX) && $(env) make
	cd $(BUILDDIR)/libgnurx/mingw-libgnurx-$(VERSION_LIBGNURX) && $(env) make install
	touch $@

$(BUILDSTEPS)/libgnurx-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/libgnurx-$(VERSION_LIBGNURX).tar.gz
	mkdir -p $(BUILDDIR)/libgnurx
	cd $(BUILDDIR)/libgnurx && tar xaf $(SOURCEDIR)/libgnurx-$(VERSION_LIBGNURX).tar.gz
	for p in $$(ls $(RECIPES)/patches/libgnurx/*.p) ; do $(PATCH) -d $(BUILDDIR)/libgnurx/mingw-libgnurx-$(VERSION_LIBGNURX) -p0 <$$p ; done
	cd $(BUILDDIR)/libgnurx/mingw-libgnurx-$(VERSION_LIBGNURX) && $(env) libtoolize --copy
	cd $(BUILDDIR)/libgnurx/mingw-libgnurx-$(VERSION_LIBGNURX) && $(env) aclocal
	cd $(BUILDDIR)/libgnurx/mingw-libgnurx-$(VERSION_LIBGNURX) && $(env) autoconf
	cd $(BUILDDIR)/libgnurx/mingw-libgnurx-$(VERSION_LIBGNURX) && $(env) automake --add-missing
	touch $@

$(SOURCEDIR)/libgnurx-$(VERSION_LIBGNURX).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://downloads.sourceforge.net/project/mingw/Other/UserContributed/regex/mingw-regex-$(VERSION_LIBGNURX)/mingw-libgnurx-$(VERSION_LIBGNURX)-src.tar.gz" $@

# atari cf library
$(BUILDSTEPS)/libcf.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/libcf-src.d
	$(env) make -C $(BUILDDIR)/libcf/cflib-$(VERSION_LIBCF)/cflib CROSSPREFIX=${TARGET}- PREFIX=$(GCCSDK_INSTALL_ENV) CROSS=yes
	mkdir -p $(GCCSDK_INSTALL_ENV)/lib/mshort
	mkdir -p $(GCCSDK_INSTALL_ENV)/lib/m68020-60
	mkdir -p $(GCCSDK_INSTALL_ENV)/lib/m5475
	$(env) make -C $(BUILDDIR)/libcf/cflib-$(VERSION_LIBCF)/cflib CROSSPREFIX=${TARGET}- PREFIX=$(GCCSDK_INSTALL_ENV) CROSS=yes install
	touch $@

$(BUILDSTEPS)/libcf-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/libcf-$(VERSION_LIBCF).tar.bz2
	$(RM) -rf $(BUILDDIR)/libcf
	mkdir -p $(BUILDDIR)/libcf
	cd $(BUILDDIR)/libcf && tar xaf $(SOURCEDIR)/libcf-$(VERSION_LIBCF).tar.bz2
	touch $@

$(SOURCEDIR)/libcf-$(VERSION_LIBCF).tar.bz2:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://vincent.riviere.free.fr/soft/m68k-atari-mint/archives/cflib-$(VERSION_LIBCF).tar.bz2" $@

# atari windom library
$(BUILDSTEPS)/windom.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/windom-src.d
	mkdir -p $(BUILDDIR)/windom
	cd $(BUILDDIR)/windom && unzip $(SOURCEDIR)/windom-$(VERSION_WINDOM).zip
	for p in `ls $(RECIPES)/patches/windom/*.p` ; do $(PATCH) -d $(BUILDDIR)/windom/windom-$(VERSION_WINDOM)-1 -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/windom/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/windom/$(TARGET)/*.p` ; do $(PATCH) -d $(BUILDDIR)/windom/windom-$(VERSION_WINDOM)-1 -p0 <$$p ; done
endif
	$(env) make -C $(BUILDDIR)/windom/windom-$(VERSION_WINDOM)-1/src -f gcc.mak CROSS=yes CROSSPREFIX=${TARGET} M68K_ATARI_MINT_CFLAGS="-I$(GCCSDK_INSTALL_ENV)/include/" 
	$(env) make -C $(BUILDDIR)/windom/windom-$(VERSION_WINDOM)-1/src -f gcc.mak CROSS=yes CROSSPREFIX=${TARGET} PREFIX=$(GCCSDK_INSTALL_ENV) install
	touch $@

$(BUILDSTEPS)/windom-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/windom-$(VERSION_WINDOM).zip
	touch $@

$(SOURCEDIR)/windom-$(VERSION_WINDOM).zip:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://downloads.sourceforge.net/project/windom/windom/$(VERSION_WINDOM)/windom-$(VERSION_WINDOM)-1.zip" $@

# atari ldg library
$(BUILDSTEPS)/ldg.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/ldg-src.d
	mkdir -p $(BUILDDIR)/ldg
	cd $(BUILDDIR)/ldg && unzip $(SOURCEDIR)/ldg-$(VERSION_LDG).zip
	$(env) make -C $(BUILDDIR)/ldg/ldg-$(VERSION_LDG)/src/devel -f gcc.mak CROSS=yes PREFIX=$(GCCSDK_INSTALL_ENV) CC=${TARGET}-gcc AR=${TARGET}-ar
	cp $(BUILDDIR)/ldg/ldg-$(VERSION_LDG)/lib/gcc/libldg.a $(GCCSDK_INSTALL_ENV)/lib/
	cp $(BUILDDIR)/ldg/ldg-$(VERSION_LDG)/include/ldg.h $(GCCSDK_INSTALL_ENV)/include/ldg.h
	mkdir -p $(GCCSDK_INSTALL_ENV)/include/ldg
	cp $(BUILDDIR)/ldg/ldg-$(VERSION_LDG)/include/ldg/mem.h $(GCCSDK_INSTALL_ENV)/include/ldg/
	touch $@

$(BUILDSTEPS)/ldg-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/ldg-$(VERSION_LDG).zip
	touch $@

$(SOURCEDIR)/ldg-$(VERSION_LDG).zip:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://ldg.org.free.fr/distrib/ldg-$(VERSION_LDG).zip" $@

# hermes bitmap lib
$(BUILDSTEPS)/hermes.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/hermes-src.d
	cd $(BUILDDIR)/hermes/Hermes-$(VERSION_HERMES)
	for p in `ls $(RECIPES)/patches/hermes/*.p` ; do $(PATCH) -d $(BUILDDIR)/hermes/Hermes-$(VERSION_HERMES) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/hermes/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/hermes/$(TARGET)/*.p` ; do $(PATCH) -d $(BUILDDIR)/hermes/Hermes-$(VERSION_HERMES) -p0 <$$p ; done
endif
	cd $(BUILDDIR)/hermes/Hermes-$(VERSION_HERMES) && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared --disable-x86asm
	cd $(BUILDDIR)/hermes/Hermes-$(VERSION_HERMES) && $(env) make CFLAGS="-D__BIG_ENDIAN__" install 
	touch $@

$(BUILDSTEPS)/hermes-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/hermes-$(VERSION_HERMES).tar.bz2
	$(RM) -rf $(BUILDDIR)/hermes
	mkdir -p $(BUILDDIR)/hermes
	cd $(BUILDDIR)/hermes && tar xaf $(SOURCEDIR)/hermes-$(VERSION_HERMES).tar.bz2
	touch $@

$(SOURCEDIR)/hermes-$(VERSION_HERMES).tar.bz2:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://clanlib.org/download/legacy/Hermes-$(VERSION_HERMES).tar.bz2" $@

# freetype font lib
$(BUILDSTEPS)/freetype.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/freetype-src.d
	cd $(BUILDDIR)/freetype/freetype-$(VERSION_FREETYPE) && $(env) $(FREETYPE_ENV) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared
	cd $(BUILDDIR)/freetype/freetype-$(VERSION_FREETYPE) && $(env) make
	cd $(BUILDDIR)/freetype/freetype-$(VERSION_FREETYPE) && $(env) make install 
	touch $@

$(BUILDSTEPS)/freetype-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/freetype-$(VERSION_FREETYPE).tar.gz
	$(RM) -rf $(BUILDDIR)/freetype
	mkdir -p $(BUILDDIR)/freetype
	cd $(BUILDDIR)/freetype && tar xaf $(SOURCEDIR)/freetype-$(VERSION_FREETYPE).tar.gz
ifneq ($(realpath $(RECIPES)/files/freetype/$(TARGET)),)
	cp -r $(RECIPES)/files/freetype/$(TARGET)/* $(BUILDDIR)/freetype/freetype-$(VERSION_FREETYPE)/
endif
	touch $@

$(SOURCEDIR)/freetype-$(VERSION_FREETYPE).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://download.savannah.gnu.org/releases/freetype/freetype-$(VERSION_FREETYPE).tar.gz" $@

# amiga roadshow sdk
$(BUILDSTEPS)/roadshow.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/roadshow-src.d
	mkdir -p $(BUILDDIR)/Roadshow-SDK
	lha xw=$(BUILDDIR)/Roadshow-SDK $(SOURCEDIR)/Roadshow-SDK-NS.lha
	mkdir -p $(GCCSDK_INSTALL_ENV)/netinclude
	cp -r $(BUILDDIR)/Roadshow-SDK/SDK/netinclude/* $(GCCSDK_INSTALL_ENV)/netinclude/
	touch $@

$(BUILDSTEPS)/roadshow-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/Roadshow-SDK-NS.lha
	touch $@

$(SOURCEDIR)/Roadshow-SDK-NS.lha:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://github.com/chris-y/curl/releases/download/curl-7_64_0-amissl/Roadshow-SDK-NS.lha" $@

# regex
$(BUILDSTEPS)/libtre.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/libtre-src.d
	mkdir -p $(BUILDDIR)/libtre
	cd $(BUILDDIR)/libtre && tar xaf $(SOURCEDIR)/tre-$(VERSION_LIBTRE).tar.bz2
	cd $(BUILDDIR)/libtre/tre-$(VERSION_LIBTRE) && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared
	cd $(BUILDDIR)/libtre/tre-$(VERSION_LIBTRE) && $(env) make install
	cp $(RECIPES)/files/libtre/regex.h $(GCCSDK_INSTALL_ENV)/include/regex.h
	touch $@

$(BUILDSTEPS)/libtre-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/tre-$(VERSION_LIBTRE).tar.bz2
	touch $@

$(SOURCEDIR)/tre-$(VERSION_LIBTRE).tar.bz2:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://laurikari.net/tre/$(subst $(SOURCEDIR)/,,$@)" $@

# libpbl
$(BUILDSTEPS)/libpbl.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/libpbl-src.d
	mkdir -p $(BUILDDIR)/libpbl
	cd $(BUILDDIR)/libpbl && tar xaf $(SOURCEDIR)/pbl_$(VERSION_LIBPBL).tar.gz
	for p in `ls $(RECIPES)/patches/libpbl/*.p` ; do $(PATCH) -d $(BUILDDIR)/libpbl/pbl_$(VERSION_LIBPBL)_04 -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/libpbl/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/libpbl/$(TARGET)/*.p` ; do $(PATCH) -d $(BUILDDIR)/libpbl/pbl_$(VERSION_LIBPBL)_04 -p0 <$$p ; done
endif
	cd $(BUILDDIR)/libpbl/pbl_$(VERSION_LIBPBL)_04 && $(env) make install PREFIX=$(GCCSDK_INSTALL_ENV)
	touch $@

$(BUILDSTEPS)/libpbl-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/pbl_$(VERSION_LIBPBL).tar.gz
	touch $@

$(SOURCEDIR)/pbl_$(VERSION_LIBPBL).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://www.mission-base.com/peter/source/$(subst $(SOURCEDIR)/,,$@)" $@

# OpenSSL
$(BUILDSTEPS)/openssl.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/zlib.d $(BUILDSTEPS)/openssl-src.d
ifneq ($(TARGET),m68k-unknown-amigaos)
	cd $(BUILDDIR)/openssl/openssl-$(VERSION_OPENSSL) && $(env) ./Configure --prefix=$(GCCSDK_INSTALL_ENV) $(TARGET) no-shared no-threads $(EXTRAARGS_OPENSSL)
	cd $(BUILDDIR)/openssl/openssl-$(VERSION_OPENSSL) && $(env) make install
endif
	touch $@

# prepare the openssl source tree
$(BUILDSTEPS)/openssl-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/openssl-$(VERSION_OPENSSL).tar.gz
	$(RM) -rf $(BUILDDIR)/openssl
	mkdir -p $(BUILDDIR)/openssl
	cd $(BUILDDIR)/openssl && tar xaf $(SOURCEDIR)/openssl-$(VERSION_OPENSSL).tar.gz
	for p in `ls $(RECIPES)/patches/openssl/*.p` ; do $(PATCH) -d $(BUILDDIR)/openssl/openssl-$(VERSION_OPENSSL) -p0 <$$p || exit $0 ; done
ifneq ($(realpath $(RECIPES)/patches/openssl/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/openssl/$(TARGET)/*.p` ; do $(PATCH) -d $(BUILDDIR)/openssl/openssl-$(VERSION_OPENSSL) -p0 <$$p || exit $0 ; done
endif
ifneq ($(realpath $(RECIPES)/files/openssl/$(TARGET)),)
	cp -r $(RECIPES)/files/openssl/$(TARGET)/* $(BUILDDIR)/openssl/openssl-$(VERSION_OPENSSL)
endif
	touch $@

# get the openssl source tar
$(SOURCEDIR)/openssl-$(VERSION_OPENSSL).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://www.openssl.org/source/$(subst $(SOURCEDIR)/,,$@)" $@

# libPNG
$(BUILDSTEPS)/libpng.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/zlib.d $(BUILDSTEPS)/libpng-src.d
	mkdir -p $(BUILDDIR)/libpng
	cd $(BUILDDIR)/libpng && tar xaf $(SOURCEDIR)/libpng-$(VERSION_LIBPNG).tar.gz
ifneq ($(realpath $(RECIPES)/patches/libpng/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/libpng/$(TARGET)/*.p` ; do $(PATCH) -d $(BUILDDIR)/libpng/libpng-$(VERSION_LIBPNG) -p0 <$$p ; done
endif
	cd $(BUILDDIR)/libpng/libpng-$(VERSION_LIBPNG) && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared
	cd $(BUILDDIR)/libpng/libpng-$(VERSION_LIBPNG) && $(env) make install
	touch $@

$(BUILDSTEPS)/libpng-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/libpng-$(VERSION_LIBPNG).tar.gz
	touch $@

$(SOURCEDIR)/libpng-$(VERSION_LIBPNG).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://downloads.sourceforge.net/libpng/$(subst $(SOURCEDIR)/,,$@)" $@


# libjpeg
$(BUILDSTEPS)/libjpeg.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/libjpeg-src.d
	mkdir -p $(BUILDDIR)/libjpeg
	cd $(BUILDDIR)/libjpeg && tar xaf $(SOURCEDIR)/jpegsrc.v$(VERSION_LIBJPEG).tar.gz
	for p in `ls $(RECIPES)/patches/libjpeg/*.p` ; do $(PATCH) -d $(BUILDDIR)/libjpeg/jpeg-$(VERSION_LIBJPEG) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/libjpeg/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/libjpeg/$(TARGET)/*.p` ; do $(PATCH) -d $(BUILDDIR)/libjpeg/jpeg-$(VERSION_LIBJPEG) -p0 <$$p ; done
endif
	cd $(BUILDDIR)/libjpeg/jpeg-$(VERSION_LIBJPEG) && $(env) $(LIBJPEG_ENV) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared
	cd $(BUILDDIR)/libjpeg/jpeg-$(VERSION_LIBJPEG) && $(env) make install
	touch $@

$(BUILDSTEPS)/libjpeg-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/jpegsrc.v$(VERSION_LIBJPEG).tar.gz
	touch $@

$(SOURCEDIR)/jpegsrc.v$(VERSION_LIBJPEG).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://ijg.org/files/$(subst $(SOURCEDIR)/,,$@)" $@


# libjpegturbo
$(BUILDSTEPS)/libjpeg-turbo.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/libjpeg-turbo-src.d
	mkdir -p $(LIBJPEG_TURBO_BUILDDIR)
	cd $(LIBJPEG_TURBO_BUILDDIR) && tar xaf $(SOURCEDIR)/libjpeg-turbo-src-$(VERSION_LIBJPEG_TURBO).tar.gz
	for p in `ls $(RECIPES)/patches/libjpegturbo/*.p` ; do $(PATCH) -d $(LIBJPEG_TURBO_BUILDDIR)/libjpeg-turbo-$(VERSION_LIBJPEG_TURBO) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/libjpegturbo/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/libjpegturbo/$(TARGET)/*.p` ; do $(PATCH) -d $(LIBJPEG_TURBO_BUILDDIR)/libjpeg-turbo-$(VERSION_LIBJPEG_TURBO) -p0 <$$p ; done
endif
	cd $(LIBJPEG_TURBO_BUILDDIR)/libjpeg-turbo-$(VERSION_LIBJPEG_TURBO) && $(env) $(LIBJPEG_TURBO_ENV) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared
	cd $(LIBJPEG_TURBO_BUILDDIR)/libjpeg-turbo-$(VERSION_LIBJPEG_TURBO) && $(env) make install
	touch $@

$(BUILDSTEPS)/libjpeg-turbo-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/libjpeg-turbo-src-$(VERSION_LIBJPEG_TURBO).tar.gz
	touch $@

$(SOURCEDIR)/libjpeg-turbo-src-$(VERSION_LIBJPEG_TURBO).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://sourceforge.net/projects/libjpeg-turbo/files/$(VERSION_LIBJPEG_TURBO)/libjpeg-turbo-$(VERSION_LIBJPEG_TURBO).tar.gz/download" $@


# libwebp
$(BUILDSTEPS)/libwebp.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/libwebp-src.d
	mkdir -p $(BUILDDIR)/libwebp
	cd $(BUILDDIR)/libwebp && tar xaf $(SOURCEDIR)/libwebp-$(VERSION_LIBWEBP).tar.gz
	for p in `ls $(RECIPES)/patches/libwebp/*.p` ; do $(PATCH) -d $(BUILDDIR)/libwebp/libwebp-$(VERSION_LIBWEBP) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/libwebp/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/libwebp/$(TARGET)/*.p` ; do $(PATCH) -d $(BUILDDIR)/libwebp/libwebp-$(VERSION_LIBWEBP) -p0 <$$p ; done
endif
	cd $(BUILDDIR)/libwebp/libwebp-$(VERSION_LIBWEBP) && $(env) $(LIBWEBP_ENV) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared $(EXTRAARGS_LIBWEBP)
	cd $(BUILDDIR)/libwebp/libwebp-$(VERSION_LIBWEBP) && $(env) make install
	touch $@

$(BUILDSTEPS)/libwebp-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/libwebp-$(VERSION_LIBWEBP).tar.gz
	touch $@

$(SOURCEDIR)/libwebp-$(VERSION_LIBWEBP).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://storage.googleapis.com/downloads.webmproject.org/releases/webp/libwebp-$(VERSION_LIBWEBP).tar.gz" $@


# libcares
$(BUILDSTEPS)/libcares.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/libcares-src.d
	mkdir -p $(BUILDDIR)/libcares
	cd $(BUILDDIR)/libcares && tar xaf $(SOURCEDIR)/c-ares-$(VERSION_LIBCARES).tar.gz
	for p in `ls $(RECIPES)/patches/libcares/*.p` ; do $(PATCH) -d $(BUILDDIR)/libcares/c-ares-$(VERSION_LIBCARES) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/libcares/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/libcares/$(TARGET)/*.p` ; do $(PATCH) -d $(BUILDDIR)/libcares/c-ares-$(VERSION_LIBCARES) -p0 <$$p ; done
endif
	cd $(BUILDDIR)/libcares/c-ares-$(VERSION_LIBCARES) && $(cleanenv) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) $(EXTRAARGS_LIBCARES)
	cd $(BUILDDIR)/libcares/c-ares-$(VERSION_LIBCARES) && $(cleanenv) make install
	touch $@

$(BUILDSTEPS)/libcares-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/c-ares-$(VERSION_LIBCARES).tar.gz
	touch $@

$(SOURCEDIR)/c-ares-$(VERSION_LIBCARES).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://c-ares.haxx.se/download/$(subst $(SOURCEDIR)/,,$@)" $@


# libcurl
$(BUILDSTEPS)/libcurl.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/zlib.d $(BUILDSTEPS)/openssl.d $(BUILDSTEPS)/libcares.d $(BUILDSTEPS)/libcurl-src.d
	cd $(BUILDDIR)/libcurl/curl-$(VERSION_LIBCURL) && $(env) CFLAGS="" $(LIBCURL_ENV) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared --without-libidn ${EXTRAARGS_LIBCURL}
	cd $(BUILDDIR)/libcurl/curl-$(VERSION_LIBCURL) && $(env) make install
	touch $@

$(BUILDSTEPS)/libcurl-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/curl-$(VERSION_LIBCURL).tar.bz2
	mkdir -p $(BUILDDIR)/libcurl
	cd $(BUILDDIR)/libcurl && tar xaf $(SOURCEDIR)/curl-$(VERSION_LIBCURL).tar.bz2
	for p in `ls $(RECIPES)/patches/libcurl/*.p` ; do $(PATCH) -d $(BUILDDIR)/libcurl/curl-$(VERSION_LIBCURL) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/libcurl/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/libcurl/$(TARGET)/*.p` ; do $(PATCH) -d $(BUILDDIR)/libcurl/curl-$(VERSION_LIBCURL) -p0 <$$p ; done
endif
	touch $@

$(SOURCEDIR)/curl-$(VERSION_LIBCURL).tar.bz2:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://curl.haxx.se/download/$(subst $(SOURCEDIR)/,,$@)" $@


# libexpat
$(BUILDSTEPS)/libexpat.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/libexpat-src.d
	mkdir -p $(BUILDDIR)/libexpat
	cd $(BUILDDIR)/libexpat && tar xaf $(SOURCEDIR)/expat-$(VERSION_LIBEXPAT).tar.bz2
	cd $(BUILDDIR)/libexpat/expat-$(VERSION_LIBEXPAT) && $(env) $(LIBEXPAT_ENV) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared
	cd $(BUILDDIR)/libexpat/expat-$(VERSION_LIBEXPAT) && $(env) $(LIBEXPAT_ENV) make install
	touch $@

$(BUILDSTEPS)/libexpat-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/expat-$(VERSION_LIBEXPAT).tar.bz2
	touch $@

$(SOURCEDIR)/expat-$(VERSION_LIBEXPAT).tar.bz2:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://downloads.sourceforge.net/project/expat/expat/$(VERSION_LIBEXPAT)/expat-$(VERSION_LIBEXPAT).tar.bz2" $@

