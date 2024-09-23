#
# NetSurf Project toolchain SDK build
#
# Required environment:
#
# GCCSDK_INSTALL_ENV      -- /path/to/install/prefix
# GCCSDK_INSTALL_CROSSBIN -- /path/to/toolchain/bin
#
# supported targets are:
#                       arm-riscos-gnueabi
#                       arm-unknown-riscos  
#                       m68k-atari-mint
#			m5475-atari-mint
#                       ppc-amigaos 
#                       i686-w64-mingw32    
#                       x86_64-w64-mingw32
#                       m68k-unknown-amigaos

ifeq ($(GCCSDK_INSTALL_ENV),)
  $(error GCCSDK_INSTALL_ENV not set)
endif

ifeq ($(GCCSDK_INSTALL_CROSSBIN),)
  $(error GCCSDK_INSTALL_CROSSBIN not set)
endif

# Upstream package versions
# https://zlib.net/
VERSION_ZLIB := 1.2.13
CKSUM_ZLIB := b3a24de97a8fdbc835b9833169501030b8977031bcb54b3b3ac13740f846ab30
# https://www.gnu.org/software/libiconv/
VERSION_LIBICONV := 1.17
CKSUM_LIBICONV := 8f74213b56238c85a50a5329f77e06198771e70dd9a739779f4c02f65d971313
# https://laurikari.net/tre/
VERSION_LIBTRE := 0.8.0
CKSUM_LIBTRE := 8dc642c2cde02b2dac6802cdbe2cda201daf79c4ebcbb3ea133915edf1636658
# https://www.openssl.org/
VERSION_OPENSSL := 3.0.12
CKSUM_OPENSSL := f93c9e8edde5e9166119de31755fc87b4aa34863662f67ddfcba14d0b6b69b61
# http://www.libpng.org/pub/png/libpng.html
VERSION_LIBPNG := 1.6.40
CKSUM_LIBPNG := 8f720b363aa08683c9bf2a563236f45313af2c55d542b5481ae17dd8d183bb42
# http://libjpeg.sourceforge.net/
VERSION_LIBJPEG := 8d
CKSUM_LIBJPEG := 00029b1473f0f0ea72fbca3230e8cb25797fbb27e58ae2e46bb8bf5a806fe0b3
# https://libjpeg-turbo.org/
VERSION_LIBJPEG_TURBO := 3.0.1
CKSUM_LIBJPEG_TURBO := 22429507714ae147b3acacd299e82099fce5d9f456882fc28e252e4579ba2a75
# https://developers.google.com/speed/webp/
VERSION_LIBWEBP := 1.3.2
CKSUM_LIBWEBP := 2a499607df669e40258e53d0ade8035ba4ec0175244869d1025d460562aa09b4
# https://c-ares.org/
VERSION_LIBCARES := 1.23.0
CKSUM_LIBCARES := cb614ecf78b477d35963ebffcf486fc9d55cc3d3216f00700e71b7d4868f79f5
# https://curl.se/
VERSION_LIBCURL := 8.5.0
CKSUM_LIBCURL := ce4b6a6655431147624aaf582632a36fe1ade262d5fab385c60f78942dd8d87b
#
VERSION_LIBGNURX := 2.5.1
CKSUM_LIBGNURX := 7147b7f806ec3d007843b38e19f42a5b7c65894a57ffc297a76b0dcd5f675d76
# http://mission-base.com/peter/source/
# 1.7.0: https://github.com/peterGraf/pbl/releases
VERSION_LIBPBL := 1_04
CKSUM_LIBPBL := 20f0ab1af09a22a8f3cb3ce3832247a6b51d8db57211f6bb0794af9485229a67
# Git-20170304
VERSION_LIBCF := CVS-20130415
CKSUM_LIBCF := 5d25c4faa2f5b7fc106bb47d093646f79d0fa72a34aaf8122110a2e4cf9eb6cc
# https://github.com/libexpat/libexpat
VERSION_LIBEXPAT := 2.5.0
CKSUM_LIBEXPAT := 6f0e6e01f7b30025fa05c85fdad1e5d0ec7fd35d9f61b22f34998de11969ff67
#
VERSION_LDG := SVN-20150827
CKSUM_LDG := ca7959d31b46cb1e755005eddcc0daf6a91f7faad7d869d82e94d98d20650d94
#
VERSION_WINDOM := 2.0.1
CKSUM_WINDOM := 2539c15c22f7b8ca48c015f0c3d33f3bd1666cc8aa56eef6f38de2cc98a3479e
# https://github.com/berkus/hermes-pixel-format/archive/refs/tags/v1.3.3.tar.gz
VERSION_HERMES := 1.3.3
CKSUM_HERMES := e86374606105d2a251d4406e5f9fca8d09e2fd971ebb0dfd342e40953676c46e
# https://www.freetype.org/
VERSION_FREETYPE := 2.13.2
CKSUM_FREETYPE := 1ac27e16c134a7f2ccea177faba19801131116fd682efc1f5737037c5db224b5
#
VERSION_ROADSHOW := 7_64_0-amissl
CKSUM_ROADSHOW := 5af092c2319f184d49f5eda72d160231b3f3551f9884f8136c531521059a4ad2

# Path
path__ := $(GCCSDK_INSTALL_CROSSBIN):$(PATH)

# Tools
cc__ := $(word 1,$(wildcard $(GCCSDK_INSTALL_CROSSBIN)/*gcc))
cxx__ := $(word 1,$(wildcard $(GCCSDK_INSTALL_CROSSBIN)/*g++))
ar__ := $(word 1,$(wildcard $(GCCSDK_INSTALL_CROSSBIN)/*ar))
ranlib__ := $(word 1,$(wildcard $(GCCSDK_INSTALL_CROSSBIN)/*ranlib))
windres__ := $(word 1,$(wildcard $(GCCSDK_INSTALL_CROSSBIN)/*windres))

# Flags
cflags__ := -I$(GCCSDK_INSTALL_ENV)/include
cppflags__ := -I$(GCCSDK_INSTALL_ENV)/include
ldflags__ := -L$(GCCSDK_INSTALL_ENV)/lib
pkg_config_libdir__ := $(GCCSDK_INSTALL_ENV)/lib/pkgconfig

# Target
TARGET := $(shell $(cc__) -dumpmachine)

ifeq ($(findstring -riscos,$(TARGET)),-riscos)
  cflags__ := $(cflags__) -mpoke-function-name
  ifeq ($(findstring -gnueabi,$(TARGET)),-gnueabi)
    cflags__ := $(cflags__) -funwind-tables
  endif
endif

# Environment
# Newer autotools objects to -I appearing in CFLAGS
cleanenv := PATH="$(path__)" CC="$(cc__)" CXX="$(cxx__)" AR="$(ar__)" RANLIB="$(ranlib__)" CPPFLAGS="$(cppflags__)" LDFLAGS="$(ldflags__)" PKG_CONFIG_LIBDIR="$(pkg_config_libdir__)"
env := $(cleanenv) CFLAGS="$(cflags__)"

# Cross-compiling CMake invocation.
NS_SDK_CMAKE_CMD=cmake \
		-DCMAKE_INSTALL_PREFIX="$(GCCSDK_INSTALL_ENV)" \
		-DBUILD_SHARED_LIBS=OFF \
		-DCMAKE_COLOR_MAKEFILE=OFF \
		-DBUILD_DOC=OFF \
		-DBUILD_DOCS=OFF \
		-DBUILD_EXAMPLE=OFF \
		-DBUILD_EXAMPLES=OFF \
		-DBUILD_TEST=OFF \
		-DBUILD_TESTS=OFF \
		-DBUILD_TESTING=OFF \

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

LIBCURL_SSL := openssl
LIBCURL_DISABLES := $(patsubst %,--disable-%,ftp file dict ldap ldaps mqtt rtsp telnet gopher tftp pop3 imap smb smtp manual)
FREETYPE_ENV := LIBPNG_CFLAGS=$$($(GCCSDK_INSTALL_ENV)/bin/libpng-config --cflags) LIBPNG_LDFLAGS=$$($(GCCSDK_INSTALL_ENV)/bin/libpng-config --ldflags)

# per target environments
LIBJPEG_TURBO_BUILDDIR:=$(BUILDDIR)/libjpegturbo

# Openssl target has to alter for mingw
TARGET_OPENSSL:=$(TARGET)

# Target Specific setup
ifeq ($(TARGET),arm-riscos-gnueabi)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, $(COMMON_SDK_ITEMS) libjpeg-turbo.d libwebp.d)
  EXTRAARGS_LIBCARES := --disable-shared
  EXTRAARGS_LIBCURL := --enable-nonblocking --enable-ares --with-random="/dev/urandom" $(LIBCURL_DISABLES)
  EXTRAARGS_OPENSSL := no-asm
endif

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
  LIBCURL_ENV := CFLAGS="-DCARES_STATICLIB" LIBS="-lgdi32 -lcrypt32 -liphlpapi"
  TARGET_OPENSSL := mingw
  ENV_OPENSSL := WINDRES="$(windres__)"
  EXTRAARGS_OPENSSL := no-asm enable-capieng
endif

ifeq ($(TARGET),x86_64-w64-mingw32)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d libgnurx.d $(COMMON_SDK_ITEMS) libjpeg.d libwebp.d)
  EXTRAARGS_LIBCARES := --disable-shared
  EXTRAARGS_LIBCURL := --enable-nonblocking --enable-ares --disable-ldap --without-random
  LIBCURL_ENV := CFLAGS="-DCARES_STATICLIB" LIBS="-lgdi32 -lcrypt32 -liphlpapi"
  TARGET_OPENSSL := mingw64
  EXTRAARGS_OPENSSL := no-asm enable-capieng
endif

ifeq ($(TARGET),m68k-unknown-amigaos)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, roadshow.d libiconv.d libtre.d libpbl.d $(COMMON_SDK_ITEMS) libjpeg.d)
  EXTRAARGS_LIBCARES := --disable-shared --without-random
  LIBCARES_ENV := LIBS="-lm"
  LIBCURL_SSL := amissl
  EXTRAARGS_LIBCURL := --disable-ntlm-wb --without-random --enable-threaded-resolver --with-zlib --enable-pthreads --disable-ipv6 LIBS="-lpthread -lamiga -lm"
  LIBICONV_ENV := CFLAGS="-DPATH_MAX=1024 -DRLIMIT_NOFILE=6"
  LIBJPEG_ENV := LDFLAGS="-lm"
  LIBJPEG_TURBO_ENV := LDFLAGS="-lm"
  # XXX: build openssl against bsdsocket.library?
  # ENV_OPENSSL := CFLAGS="-I$(GCCSDK_INSTALL_ENV)/netinclude" CPPFLAGS="-D__NO_NET_API"
  ifeq ($(LIBCURL_SSL),openssl)
    LIBCURL_ENV := LIBS="-lm -lnet"
  else
    LIBCURL_ENV := CFLAGS="-I$(GCCSDK_INSTALL_ENV)/netinclude -D__NO_NET_API -DHAVE_IOCTLSOCKET_CAMEL_FIONBIO" LIBS="-lm"
  endif
  # XXX: libgcc is meant to be implied, but for some reason that
  # results in ld evaluating it after libm and thus failing to find symbols.
  # Hack around it by forcing the link order here.
  LIBEXPAT_ENV := LDFLAGS="-lgcc -lm"
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
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://zlib.net/fossils/$(subst $(SOURCEDIR)/,,$@)" $(CKSUM_ZLIB) $@

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
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://ftp.gnu.org/pub/gnu/libiconv/$(subst $(SOURCEDIR)/,,$@)" $(CKSUM_LIBICONV) $@

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
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://downloads.sourceforge.net/project/mingw/Other/UserContributed/regex/mingw-regex-$(VERSION_LIBGNURX)/mingw-libgnurx-$(VERSION_LIBGNURX)-src.tar.gz" $(CKSUM_LIBGNURX) $@

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
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://vincent.riviere.free.fr/soft/m68k-atari-mint/archives/cflib-$(VERSION_LIBCF).tar.bz2" $(CKSUM_LIBCF) $@

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
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://downloads.sourceforge.net/project/windom/windom/$(VERSION_WINDOM)/windom-$(VERSION_WINDOM)-1.zip" $(CKSUM_WINDOM) $@

# atari ldg library
$(BUILDSTEPS)/ldg.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/ldg-src.d
	mkdir -p $(BUILDDIR)/ldg
	cd $(BUILDDIR)/ldg && tar xjf $(SOURCEDIR)/ldg-$(VERSION_LDG).tar.bz2
	$(env) make -C $(BUILDDIR)/ldg/ldg-$(VERSION_LDG)/src/devel -f gcc.mak CROSS=yes PREFIX=$(GCCSDK_INSTALL_ENV) CC=${TARGET}-gcc AR=${TARGET}-ar
	cp $(BUILDDIR)/ldg/ldg-$(VERSION_LDG)/lib/gcc/libldg.a $(GCCSDK_INSTALL_ENV)/lib/
	cp $(BUILDDIR)/ldg/ldg-$(VERSION_LDG)/include/ldg.h $(GCCSDK_INSTALL_ENV)/include/ldg.h
	mkdir -p $(GCCSDK_INSTALL_ENV)/include/ldg
	cp $(BUILDDIR)/ldg/ldg-$(VERSION_LDG)/include/ldg/mem.h $(GCCSDK_INSTALL_ENV)/include/ldg/
	touch $@

$(BUILDSTEPS)/ldg-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/ldg-$(VERSION_LDG).tar.bz2
	touch $@

$(SOURCEDIR)/ldg-$(VERSION_LDG).tar.bz2:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://vincent.riviere.free.fr/soft/m68k-atari-mint/archives/ldg-$(VERSION_LDG).tar.bz2" $(CKSUM_LDG) $@

# hermes bitmap lib
$(BUILDSTEPS)/hermes.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/hermes-src.d
	cd $(BUILDDIR)/hermes/hermes-pixel-format-$(VERSION_HERMES)
	for p in `ls $(RECIPES)/patches/hermes/*.p` ; do $(PATCH) -d $(BUILDDIR)/hermes/hermes-pixel-format-$(VERSION_HERMES) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/hermes/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/hermes/$(TARGET)/*.p` ; do $(PATCH) -d $(BUILDDIR)/hermes/hermes-pixel-format-$(VERSION_HERMES) -p0 <$$p ; done
endif
	cd $(BUILDDIR)/hermes/hermes-pixel-format-$(VERSION_HERMES) && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared --disable-x86asm
	cd $(BUILDDIR)/hermes/hermes-pixel-format-$(VERSION_HERMES) && $(env) make CFLAGS="-D__BIG_ENDIAN__" install
	touch $@

$(BUILDSTEPS)/hermes-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/hermes-$(VERSION_HERMES).tar.gz
	$(RM) -rf $(BUILDDIR)/hermes
	mkdir -p $(BUILDDIR)/hermes
	cd $(BUILDDIR)/hermes && tar xaf $(SOURCEDIR)/hermes-$(VERSION_HERMES).tar.gz
	touch $@

$(SOURCEDIR)/hermes-$(VERSION_HERMES).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://github.com/berkus/hermes-pixel-format/archive/refs/tags/v$(VERSION_HERMES).tar.gz" $(CKSUM_HERMES) $@

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
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://download.savannah.gnu.org/releases/freetype/freetype-$(VERSION_FREETYPE).tar.gz" $(CKSUM_FREETYPE) $@

# amiga roadshow sdk
$(BUILDSTEPS)/roadshow.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/roadshow-src.d
	mkdir -p $(BUILDDIR)/Roadshow-SDK
	lha xw=$(BUILDDIR)/Roadshow-SDK $(SOURCEDIR)/Roadshow-SDK-NS-$(VERSION_ROADSHOW).lha
	mkdir -p $(GCCSDK_INSTALL_ENV)/netinclude
	cp -r $(BUILDDIR)/Roadshow-SDK/SDK/netinclude/* $(GCCSDK_INSTALL_ENV)/netinclude/
	touch $@

$(BUILDSTEPS)/roadshow-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/Roadshow-SDK-NS-$(VERSION_ROADSHOW).lha
	touch $@

$(SOURCEDIR)/Roadshow-SDK-NS-$(VERSION_ROADSHOW).lha:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://github.com/chris-y/curl/releases/download/curl-$(VERSION_ROADSHOW)/Roadshow-SDK-NS.lha" $(CKSUM_ROADSHOW) $@

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
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://laurikari.net/tre/$(subst $(SOURCEDIR)/,,$@)" $(CKSUM_LIBTRE) $@

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
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://www.mission-base.com/peter/source/$(subst $(SOURCEDIR)/,,$@)" $(CKSUM_LIBPBL) $@

# OpenSSL
$(BUILDSTEPS)/openssl.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/zlib.d $(BUILDSTEPS)/openssl-src.d
ifeq ($(LIBCURL_SSL),openssl)
	cd $(BUILDDIR)/openssl/openssl-$(VERSION_OPENSSL) && $(env) $(ENV_OPENSSL) ./Configure --prefix=$(GCCSDK_INSTALL_ENV) $(TARGET_OPENSSL) no-shared no-threads $(EXTRAARGS_OPENSSL)
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
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://www.openssl.org/source/$(subst $(SOURCEDIR)/,,$@)" $(CKSUM_OPENSSL) $@

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
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://downloads.sourceforge.net/libpng/$(subst $(SOURCEDIR)/,,$@)" $(CKSUM_LIBPNG) $@


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
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://ijg.org/files/$(subst $(SOURCEDIR)/,,$@)" $(CKSUM_LIBJPEG) $@

# libjpegturbo
$(BUILDSTEPS)/libjpeg-turbo.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/libjpeg-turbo-src.d
	mkdir -p $(LIBJPEG_TURBO_BUILDDIR)
	cd $(LIBJPEG_TURBO_BUILDDIR) && tar xaf $(SOURCEDIR)/libjpeg-turbo-src-$(VERSION_LIBJPEG_TURBO).tar.gz
	for p in `ls $(RECIPES)/patches/libjpegturbo/*.p` ; do $(PATCH) -d $(LIBJPEG_TURBO_BUILDDIR)/libjpeg-turbo-$(VERSION_LIBJPEG_TURBO) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/libjpegturbo/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/libjpegturbo/$(TARGET)/*.p` ; do $(PATCH) -d $(LIBJPEG_TURBO_BUILDDIR)/libjpeg-turbo-$(VERSION_LIBJPEG_TURBO) -p0 <$$p ; done
endif
	mkdir -p $(LIBJPEG_TURBO_BUILDDIR)/libjpeg-turbo-$(VERSION_LIBJPEG_TURBO)/ns-sdk-build
	cd $(LIBJPEG_TURBO_BUILDDIR)/libjpeg-turbo-$(VERSION_LIBJPEG_TURBO)/ns-sdk-build && $(env) $(LIBJPEG_TURBO_ENV) $(NS_SDK_CMAKE_CMD) \
			-DENABLE_STATIC=ON \
			-DENABLE_SHARED=OFF \
			..
	cd $(LIBJPEG_TURBO_BUILDDIR)/libjpeg-turbo-$(VERSION_LIBJPEG_TURBO)/ns-sdk-build && $(env) make install VERBOSE=1
	touch $@

$(BUILDSTEPS)/libjpeg-turbo-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/libjpeg-turbo-src-$(VERSION_LIBJPEG_TURBO).tar.gz
	touch $@

$(SOURCEDIR)/libjpeg-turbo-src-$(VERSION_LIBJPEG_TURBO).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://github.com/libjpeg-turbo/libjpeg-turbo/releases/download/$(VERSION_LIBJPEG_TURBO)/libjpeg-turbo-$(VERSION_LIBJPEG_TURBO).tar.gz" $(CKSUM_LIBJPEG_TURBO) $@


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
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://storage.googleapis.com/downloads.webmproject.org/releases/webp/libwebp-$(VERSION_LIBWEBP).tar.gz" $(CKSUM_LIBWEBP) $@


# libcares
$(BUILDSTEPS)/libcares.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/libcares-src.d
	mkdir -p $(BUILDDIR)/libcares
	cd $(BUILDDIR)/libcares && tar xaf $(SOURCEDIR)/c-ares-$(VERSION_LIBCARES).tar.gz
	for p in `ls $(RECIPES)/patches/libcares/*.p` ; do $(PATCH) -d $(BUILDDIR)/libcares/c-ares-$(VERSION_LIBCARES) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/libcares/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/libcares/$(TARGET)/*.p` ; do $(PATCH) -d $(BUILDDIR)/libcares/c-ares-$(VERSION_LIBCARES) -p0 <$$p ; done
endif
	cd $(BUILDDIR)/libcares/c-ares-$(VERSION_LIBCARES) && $(cleanenv) $(LIBCARES_ENV) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) $(EXTRAARGS_LIBCARES)
	cd $(BUILDDIR)/libcares/c-ares-$(VERSION_LIBCARES) && $(cleanenv) make install
	touch $@

$(BUILDSTEPS)/libcares-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/c-ares-$(VERSION_LIBCARES).tar.gz
	touch $@

$(SOURCEDIR)/c-ares-$(VERSION_LIBCARES).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://c-ares.org/download/$(subst $(SOURCEDIR)/,,$@)" $(CKSUM_LIBCARES) $@


# libcurl
$(BUILDSTEPS)/libcurl.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/zlib.d $(BUILDSTEPS)/openssl.d $(BUILDSTEPS)/libcares.d $(BUILDSTEPS)/libcurl-src.d
	cd $(BUILDDIR)/libcurl/curl-$(VERSION_LIBCURL) && $(env) CFLAGS="" $(LIBCURL_ENV) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared --with-$(LIBCURL_SSL) --without-libidn2 ${EXTRAARGS_LIBCURL}
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
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://curl.se/download/$(subst $(SOURCEDIR)/,,$@)" $(CKSUM_LIBCURL) $@


# libexpat
$(BUILDSTEPS)/libexpat.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/libexpat-src.d
	cd $(BUILDDIR)/libexpat/expat-$(VERSION_LIBEXPAT) && $(env) $(LIBEXPAT_ENV) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared
	cd $(BUILDDIR)/libexpat/expat-$(VERSION_LIBEXPAT) && $(env) $(LIBEXPAT_ENV) make install
	touch $@

$(BUILDSTEPS)/libexpat-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/expat-$(VERSION_LIBEXPAT).tar.bz2
	mkdir -p $(BUILDDIR)/libexpat
	cd $(BUILDDIR)/libexpat && tar xaf $(SOURCEDIR)/expat-$(VERSION_LIBEXPAT).tar.bz2
ifneq ($(realpath $(RECIPES)/patches/libexpat/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/libexpat/$(TARGET)/*.p` ; do $(PATCH) -d $(BUILDDIR)/libexpat/expat-$(VERSION_LIBEXPAT) -p0 <$$p ; done
endif
	touch $@

$(SOURCEDIR)/expat-$(VERSION_LIBEXPAT).tar.bz2:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://github.com/libexpat/libexpat/releases/download/R_$(subst .,_,$(VERSION_LIBEXPAT))/expat-$(VERSION_LIBEXPAT).tar.bz2" $(CKSUM_LIBEXPAT) $@

