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
VERSION_ZLIB := 1.2.8
VERSION_LIBICONV := 1.13.1
VERSION_LIBTRE := 0.8.0
VERSION_LIBXML := 2.8.0
VERSION_OPENSSL := 1.0.1e
VERSION_LIBPNG := 1.6.7
VERSION_LIBJPEG := 8d
VERSION_LIBLCMS := 2.3
VERSION_LIBMNG := 1.0.10
VERSION_LIBCARES := 1.9.1
VERSION_LIBCURL := 7.28.1
VERSION_LIBGNURX := 2.5.1
VERSION_LIBPBL := 1_04
VERSION_LIBCF := CVS-20120227
VERSION_LIBEXPAT := 2.1.0
VERSION_LDG := 2.32
VERSION_WINDOM := 2.0.1
VERSION_HERMES := 1.3.3
VERSION_FREETYPE := 2.4.10
VERSION_NSPR := 4.9.3
#VERSION_SPIDERMONKEY := 1.8.5
VERSION_SPIDERMONKEY := 1.7.0

# Path
path__ := $(GCCSDK_INSTALL_CROSSBIN):$(PATH)

# Tools
cc__ := $(wildcard $(GCCSDK_INSTALL_CROSSBIN)/*gcc)
cxx__ := $(wildcard $(GCCSDK_INSTALL_CROSSBIN)/*g++)
ar__ := $(wildcard $(GCCSDK_INSTALL_CROSSBIN)/*ar)
ranlib__ := $(wildcard $(GCCSDK_INSTALL_CROSSBIN)/*ranlib)

# Flags
cflags__ := -I$(GCCSDK_INSTALL_ENV)/include
cppflags__ := -I$(GCCSDK_INSTALL_ENV)/include
ldflags__ := -L$(GCCSDK_INSTALL_ENV)/lib
pkg_config_libdir__ := $(GCCSDK_INSTALL_ENV)/lib/pkgconfig

# Target
TARGET := $(shell $(cc__) -dumpmachine)

# Environment
env := PATH="$(path__)" CC="$(cc__)" CXX="$(cxx__)" AR="$(ar__)" RANLIB="$(ranlib__)" CFLAGS="$(cflags__)" CPPFLAGS="$(cppflags__)" LDFLAGS="$(ldflags__)" PKG_CONFIG_LIBDIR="$(pkg_config_libdir__)"

RECIPES := $(CURDIR)/recipes
SOURCEDIR := $(CURDIR)/sources
BUILDDIR := $(CURDIR)/builddir-$(TARGET)
BUILDSTEPS := $(BUILDDIR)/build-steps

FETCHSRC := $(CURDIR)/fetchsrc

# Compute the SDK components
SDK_ITEMS :=

COMMON_SDK_ITEMS := zlib.d openssl.d libpng.d libjpeg.d liblcms.d \
		    libmng.d libcares.d libcurl.d libexpat.d

LIBCURL_DISABLES := $(patsubst %,--disable-%,ftp file dict ldap telnet gopher tftp pop3 imap smtp manual)

# Target Specific setup
ifeq ($(TARGET),arm-unknown-riscos)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, $(COMMON_SDK_ITEMS) spidermonkey.d)
  EXTRAARGS_LIBCARES := --disable-shared
  EXTRAARGS_LIBCURL := --enable-nonblocking --enable-ares $(LIBCURL_DISABLES)
  ifeq ($(VERSION_SPIDERMONKEY),1.7.0)
    EXTRAARGS_SPIDERMONKEY := BUILD_OPT=1 OS_ARCH=RISCOS OS_CPUARCH=arm OS_CONFIG=RISCOS
  else
    EXTRAARGS_SPIDERMONKEY := --disable-tracejit --disable-thumb2 --without-thumb --enable-static --disable-shared --disable-shared-js --with-cpu-arch=armv3 --disable-tests --disable-methodjit
  endif
endif

ifeq ($(TARGET),m68k-atari-mint)
  EXTRAARGS_LIBXML := --enable-ipv6=no
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d $(COMMON_SDK_ITEMS) libcf.d ldg.d windom.d hermes.d freetype.d)
  EXTRAARGS_LIBCARES := --disable-shared
  EXTRAARGS_LIBCURL := --enable-optimize --enable-nonblocking --enable-ares --with-random="/dev/urandom" --disable-manual --disable-curldebug --disable-thread --disable-threaded-resolver curl_disallow_poll="yes"
  EXTRAARGS_LIBICONV := --enable-extra-encodings
endif

ifeq ($(TARGET),m5475-atari-mint)
  EXTRAARGS_LIBXML := --enable-ipv6=no
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d $(COMMON_SDK_ITEMS) libcf.d ldg.d windom.d hermes.d freetype.d)
  EXTRAARGS_LIBCARES := --disable-shared
  EXTRAARGS_LIBCURL := --enable-optimize --enable-nonblocking --enable-ares --with-random="/dev/urandom" --disable-manual --disable-curldebug --disable-thread --disable-threaded-resolver curl_disallow_poll="yes"
  EXTRAARGS_LIBICONV := --enable-extra-encodings
endif

ifeq ($(TARGET),ppc-amigaos)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d libtre.d libpbl.d $(COMMON_SDK_ITEMS))
  EXTRAARGS_LIBCARES := --disable-shared
  EXTRAARGS_LIBCURL := --disable-ntlm-wb --without-random
  VERSION_SPIDERMONKEY := 1.7.0
endif

ifeq ($(TARGET),i686-w64-mingw32)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d libgnurx.d $(COMMON_SDK_ITEMS))
  EXTRAARGS_LIBCURL := --enable-nonblocking --enable-ares --disable-ldap --without-random
endif

ifeq ($(TARGET),m68k-unknown-amigaos)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d libtre.d $(COMMON_SDK_ITEMS))
  EXTRAARGS_LIBCARES := --disable-shared --without-random
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
	for p in `ls $(RECIPES)/patches/zlib/*.p` ; do patch -d $(BUILDDIR)/zlib/zlib-$(VERSION_ZLIB) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/zlib/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/zlib/$(TARGET)/*.p` ; do patch -d $(BUILDDIR)/zlib/zlib-$(VERSION_ZLIB) -p0 <$$p ; done
endif
	touch $@

$(SOURCEDIR)/zlib-$(VERSION_ZLIB).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://zlib.net/$(subst $(SOURCEDIR)/,,$@)" $@

# libiconv
$(BUILDSTEPS)/libiconv.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/libiconv-src.d
	mkdir -p $(BUILDDIR)/libiconv
	cd $(BUILDDIR)/libiconv && tar xaf $(SOURCEDIR)/libiconv-$(VERSION_LIBICONV).tar.gz
	for p in `ls $(RECIPES)/patches/libiconv/*.p` ; do patch -d $(BUILDDIR)/libiconv/libiconv-$(VERSION_LIBICONV) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/libiconv/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/libiconv/$(TARGET)/*.p` ; do patch -d $(BUILDDIR)/libiconv/libiconv-$(VERSION_LIBICONV) -p0 <$$p ; done
endif
	cd $(BUILDDIR)/libiconv/libiconv-$(VERSION_LIBICONV) && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared $(EXTRAARGS_LIBICONV)
	cd $(BUILDDIR)/libiconv/libiconv-$(VERSION_LIBICONV) && $(env) make install
	touch $@

$(BUILDSTEPS)/libiconv-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/libiconv-$(VERSION_LIBICONV).tar.gz
	touch $@

$(SOURCEDIR)/libiconv-$(VERSION_LIBICONV).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://ftp.gnu.org/pub/gnu/libiconv/$(subst $(SOURCEDIR)/,,$@)" $@

# libgnurx
$(BUILDSTEPS)/libgnurx.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/libgnurx-src.d
	mkdir -p $(BUILDDIR)/libgnurx
	cd $(BUILDDIR)/libgnurx && tar xaf $(SOURCEDIR)/libgnurx-$(VERSION_LIBGNURX).tar.gz
	for p in `ls $(RECIPES)/patches/libgnurx/*.p` ; do patch -d $(BUILDDIR)/libgnurx/mingw-libgnurx-$(VERSION_LIBGNURX) -p0 <$$p ; done
	cd $(BUILDDIR)/libgnurx/mingw-libgnurx-$(VERSION_LIBGNURX) && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) 
	cd $(BUILDDIR)/libgnurx/mingw-libgnurx-$(VERSION_LIBGNURX) && $(env) make
	cd $(BUILDDIR)/libgnurx/mingw-libgnurx-$(VERSION_LIBGNURX) && $(env) make install
	touch $@

$(BUILDSTEPS)/libgnurx-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/libgnurx-$(VERSION_LIBGNURX).tar.gz
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
	for p in `ls $(RECIPES)/patches/windom/*.p` ; do patch -d $(BUILDDIR)/windom/windom-$(VERSION_WINDOM)-1 -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/windom/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/windom/$(TARGET)/*.p` ; do patch -d $(BUILDDIR)/windom/windom-$(VERSION_WINDOM)-1 -p0 <$$p ; done
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
	for p in `ls $(RECIPES)/patches/hermes/*.p` ; do patch -d $(BUILDDIR)/hermes/Hermes-$(VERSION_HERMES) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/hermes/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/hermes/$(TARGET)/*.p` ; do patch -d $(BUILDDIR)/hermes/Hermes-$(VERSION_HERMES) -p0 <$$p ; done
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
	cd $(BUILDDIR)/freetype/freetype-$(VERSION_FREETYPE) && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared
	cd $(BUILDDIR)/freetype/freetype-$(VERSION_FREETYPE) && $(env) make install 
	touch $@

$(BUILDSTEPS)/freetype-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/freetype-$(VERSION_FREETYPE).tar.bz2
	$(RM) -rf $(BUILDDIR)/freetype
	mkdir -p $(BUILDDIR)/freetype
	cd $(BUILDDIR)/freetype && tar xaf $(SOURCEDIR)/freetype-$(VERSION_FREETYPE).tar.bz2
	touch $@

$(SOURCEDIR)/freetype-$(VERSION_FREETYPE).tar.bz2:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://download.savannah.gnu.org/releases/freetype/freetype-$(VERSION_FREETYPE).tar.bz2" $@

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
	for p in `ls $(RECIPES)/patches/libpbl/*.p` ; do patch -d $(BUILDDIR)/libpbl/pbl_$(VERSION_LIBPBL)_04 -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/libpbl/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/libpbl/$(TARGET)/*.p` ; do patch -d $(BUILDDIR)/libpbl/pbl_$(VERSION_LIBPBL)_04 -p0 <$$p ; done
endif
	cd $(BUILDDIR)/libpbl/pbl_$(VERSION_LIBPBL)_04 && $(env) make install PREFIX=$(GCCSDK_INSTALL_ENV)
	touch $@

$(BUILDSTEPS)/libpbl-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/pbl_$(VERSION_LIBPBL).tar.gz
	touch $@

$(SOURCEDIR)/pbl_$(VERSION_LIBPBL).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://www.mission-base.com/peter/source/$(subst $(SOURCEDIR)/,,$@)" $@

# libXML2
$(BUILDSTEPS)/libxml2.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/zlib.d $(BUILDSTEPS)/libxml2-src.d
	mkdir -p $(BUILDDIR)/libxml2
	cd $(BUILDDIR)/libxml2 && tar xaf $(SOURCEDIR)/libxml2-$(VERSION_LIBXML).tar.gz
	for p in `ls $(RECIPES)/patches/libxml2/*.p` ; do patch -d $(BUILDDIR)/libxml2/libxml2-$(VERSION_LIBXML) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/libxml2/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/libxml2/$(TARGET)/*.p` ; do patch -d $(BUILDDIR)/libxml2/libxml2-$(VERSION_LIBXML) -p0 <$$p ; done
endif
	cd $(BUILDDIR)/libxml2/libxml2-$(VERSION_LIBXML) && $(env) CFLAGS=-O2 ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared --without-python $(EXTRAARGS_LIBXML)
	cd $(BUILDDIR)/libxml2/libxml2-$(VERSION_LIBXML) && $(env) make install
	touch $@

$(BUILDSTEPS)/libxml2-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/libxml2-$(VERSION_LIBXML).tar.gz
	touch $@

$(SOURCEDIR)/libxml2-$(VERSION_LIBXML).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "ftp://xmlsoft.org/libxml2/$(subst $(SOURCEDIR)/,,$@)" $@

# OpenSSL
$(BUILDSTEPS)/openssl.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/zlib.d $(BUILDSTEPS)/openssl-src.d
	mkdir -p $(BUILDDIR)/openssl
	cd $(BUILDDIR)/openssl && tar xaf $(SOURCEDIR)/openssl-$(VERSION_OPENSSL).tar.gz
	for p in `ls $(RECIPES)/patches/openssl/*.p` ; do patch -d $(BUILDDIR)/openssl/openssl-$(VERSION_OPENSSL) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/openssl/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/openssl/$(TARGET)/*.p` ; do patch -d $(BUILDDIR)/openssl/openssl-$(VERSION_OPENSSL) -p0 <$$p ; done
endif
	cd $(BUILDDIR)/openssl/openssl-$(VERSION_OPENSSL) && $(env) ./Configure --prefix=$(GCCSDK_INSTALL_ENV) $(TARGET) no-shared no-asm no-threads
	cd $(BUILDDIR)/openssl/openssl-$(VERSION_OPENSSL) && $(env) make install
	touch $@

$(BUILDSTEPS)/openssl-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/openssl-$(VERSION_OPENSSL).tar.gz
	touch $@

$(SOURCEDIR)/openssl-$(VERSION_OPENSSL).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://openssl.org/source/$(subst $(SOURCEDIR)/,,$@)" $@

# libPNG
$(BUILDSTEPS)/libpng.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/zlib.d $(BUILDSTEPS)/libpng-src.d
	mkdir -p $(BUILDDIR)/libpng
	cd $(BUILDDIR)/libpng && tar xaf $(SOURCEDIR)/libpng-$(VERSION_LIBPNG).tar.gz
ifneq ($(realpath $(RECIPES)/patches/libpng/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/libpng/$(TARGET)/*.p` ; do patch -d $(BUILDDIR)/libpng/libpng-$(VERSION_LIBPNG) -p0 <$$p ; done
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
	for p in `ls $(RECIPES)/patches/libjpeg/*.p` ; do patch -d $(BUILDDIR)/libjpeg/jpeg-$(VERSION_LIBJPEG) -p0 <$$p ; done
	cd $(BUILDDIR)/libjpeg/jpeg-$(VERSION_LIBJPEG) && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared
	cd $(BUILDDIR)/libjpeg/jpeg-$(VERSION_LIBJPEG) && $(env) make install
	touch $@

$(BUILDSTEPS)/libjpeg-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/jpegsrc.v$(VERSION_LIBJPEG).tar.gz
	touch $@

$(SOURCEDIR)/jpegsrc.v$(VERSION_LIBJPEG).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://ijg.org/files/$(subst $(SOURCEDIR)/,,$@)" $@

# liblcms
$(BUILDSTEPS)/liblcms.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/zlib.d $(BUILDSTEPS)/libjpeg.d $(BUILDSTEPS)/liblcms-src.d
	mkdir -p $(BUILDDIR)/liblcms
	cd $(BUILDDIR)/liblcms && tar xaf $(SOURCEDIR)/lcms2-$(VERSION_LIBLCMS).tar.gz
	for p in `ls $(RECIPES)/patches/liblcms/*.p` ; do patch -d $(BUILDDIR)/liblcms/lcms2-$(VERSION_LIBLCMS) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/liblcms/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/liblcms/$(TARGET)/*.p` ; do patch -d $(BUILDDIR)/liblcms/lcms2-$(VERSION_LIBLCMS) -p0 <$$p ; done
endif
	cd $(BUILDDIR)/liblcms/lcms2-$(VERSION_LIBLCMS) && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared
	cd $(BUILDDIR)/liblcms/lcms2-$(VERSION_LIBLCMS) && $(env) make install
	touch $@

$(BUILDSTEPS)/liblcms-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/lcms2-$(VERSION_LIBLCMS).tar.gz
	touch $@

$(SOURCEDIR)/lcms2-$(VERSION_LIBLCMS).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://downloads.sourceforge.net/project/lcms/lcms/$(VERSION_LIBLCMS)/$(subst $(SOURCEDIR)/,,$@)" $@


# libMNG
$(BUILDSTEPS)/libmng.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/zlib.d $(BUILDSTEPS)/libjpeg.d $(BUILDSTEPS)/liblcms.d $(BUILDSTEPS)/libmng-src.d
	mkdir -p $(BUILDDIR)/libmng
	cd $(BUILDDIR)/libmng && tar xaf $(SOURCEDIR)/libmng-$(VERSION_LIBMNG).tar.gz
	for p in `ls $(RECIPES)/patches/libmng/*.p` ; do patch -d $(BUILDDIR)/libmng/libmng-$(VERSION_LIBMNG) -p0 <$$p ; done
	cp $(BUILDDIR)/libmng/libmng-$(VERSION_LIBMNG)/makefiles/makefile.unix $(BUILDDIR)/libmng/libmng-$(VERSION_LIBMNG)/Makefile
	cd $(BUILDDIR)/libmng/libmng-$(VERSION_LIBMNG) && $(env) make install
	touch $@

$(BUILDSTEPS)/libmng-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/libmng-$(VERSION_LIBMNG).tar.gz
	touch $@

$(SOURCEDIR)/libmng-$(VERSION_LIBMNG).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://downloads.sourceforge.net/project/libmng/libmng-devel/$(VERSION_LIBMNG)/$(subst $(SOURCEDIR)/,,$@)" $@

# libcares
$(BUILDSTEPS)/libcares.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/libcares-src.d
	mkdir -p $(BUILDDIR)/libcares
	cd $(BUILDDIR)/libcares && tar xaf $(SOURCEDIR)/c-ares-$(VERSION_LIBCARES).tar.gz
	for p in `ls $(RECIPES)/patches/libcares/*.p` ; do patch -d $(BUILDDIR)/libcares/c-ares-$(VERSION_LIBCARES) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/libcares/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/libcares/$(TARGET)/*.p` ; do patch -d $(BUILDDIR)/libcares/c-ares-$(VERSION_LIBCARES) -p0 <$$p ; done
endif
	cd $(BUILDDIR)/libcares/c-ares-$(VERSION_LIBCARES) && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) $(EXTRAARGS_LIBCARES)
	cd $(BUILDDIR)/libcares/c-ares-$(VERSION_LIBCARES) && $(env) make install
	touch $@

$(BUILDSTEPS)/libcares-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/c-ares-$(VERSION_LIBCARES).tar.gz
	touch $@

$(SOURCEDIR)/c-ares-$(VERSION_LIBCARES).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://c-ares.haxx.se/download/$(subst $(SOURCEDIR)/,,$@)" $@


# libcurl
$(BUILDSTEPS)/libcurl.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/zlib.d $(BUILDSTEPS)/openssl.d $(BUILDSTEPS)/libcares.d $(BUILDSTEPS)/libcurl-src.d
	cd $(BUILDDIR)/libcurl/curl-$(VERSION_LIBCURL) && $(env) CFLAGS="" ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared --without-libidn ${EXTRAARGS_LIBCURL}
	cd $(BUILDDIR)/libcurl/curl-$(VERSION_LIBCURL) && $(env) make install
	touch $@

$(BUILDSTEPS)/libcurl-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/curl-$(VERSION_LIBCURL).tar.bz2
	mkdir -p $(BUILDDIR)/libcurl
	cd $(BUILDDIR)/libcurl && tar xaf $(SOURCEDIR)/curl-$(VERSION_LIBCURL).tar.bz2
	for p in `ls $(RECIPES)/patches/libcurl/*.p` ; do patch -d $(BUILDDIR)/libcurl/curl-$(VERSION_LIBCURL) -p0 <$$p ; done
ifneq ($(realpath $(RECIPES)/patches/libcurl/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/libcurl/$(TARGET)/*.p` ; do patch -d $(BUILDDIR)/libcurl/curl-$(VERSION_LIBCURL) -p0 <$$p ; done
endif
	touch $@

$(SOURCEDIR)/curl-$(VERSION_LIBCURL).tar.bz2:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://curl.haxx.se/download/$(subst $(SOURCEDIR)/,,$@)" $@


# libexpat
$(BUILDSTEPS)/libexpat.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/libexpat-src.d
	mkdir -p $(BUILDDIR)/libexpat
	cd $(BUILDDIR)/libexpat && tar xaf $(SOURCEDIR)/expat-$(VERSION_LIBEXPAT).tar.gz
	cd $(BUILDDIR)/libexpat/expat-$(VERSION_LIBEXPAT) && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) --disable-shared
	cd $(BUILDDIR)/libexpat/expat-$(VERSION_LIBEXPAT) && $(env) make install
	touch $@

$(BUILDSTEPS)/libexpat-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/expat-$(VERSION_LIBEXPAT).tar.gz
	touch $@

$(SOURCEDIR)/expat-$(VERSION_LIBEXPAT).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://downloads.sourceforge.net/project/expat/expat/$(VERSION_LIBEXPAT)/expat-$(VERSION_LIBEXPAT).tar.gz" $@


# nspr
$(BUILDSTEPS)/nspr.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/nspr-src.d
	cd $(BUILDDIR)/nspr/nspr-$(VERSION_NSPR)/mozilla/nsprpub && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --disable-shared --without-thumb --without-thumb-interwork --with-arch=armv3
	cd $(BUILDDIR)/nspr/nspr-$(VERSION_NSPR)/mozilla/nsprpub && $(env) make
	cd $(BUILDDIR)/nspr/nspr-$(VERSION_NSPR)/mozilla/nsprpub && $(env) make install
	touch $@

$(BUILDSTEPS)/nspr-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/nspr-$(VERSION_NSPR).tar.gz
	$(RM) -rf $(BUILDDIR)/nspr
	mkdir -p $(BUILDDIR)/nspr
	cd $(BUILDDIR)/nspr && tar xaf $(SOURCEDIR)/nspr-$(VERSION_NSPR).tar.gz
	for p in `ls $(RECIPES)/patches/nspr/$(TARGET)/*.p` ; do patch -d $(BUILDDIR)/nspr/nspr-$(VERSION_NSPR) -p0 <$$p ; done
	touch $@

$(SOURCEDIR)/nspr-$(VERSION_NSPR).tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "https://ftp.mozilla.org/pub/mozilla.org/nspr/releases/v$(VERSION_NSPR)/src/nspr-$(VERSION_NSPR).tar.gz" $@


# spidermonkey (libmozjs) 
$(BUILDSTEPS)/spidermonkey.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/nspr.d $(BUILDSTEPS)/spidermonkey-src.d
ifeq ($(VERSION_SPIDERMONKEY),1.7.0)
	$(env) make -C $(BUILDDIR)/spidermonkey/js-$(VERSION_SPIDERMONKEY)/js/src -f Makefile.ref JS_DIST=$(GCCSDK_INSTALL_ENV) $(EXTRAARGS_SPIDERMONKEY)
	$(env) make -C $(BUILDDIR)/spidermonkey/js-$(VERSION_SPIDERMONKEY)/js/src -f Makefile.ref JS_DIST=$(GCCSDK_INSTALL_ENV) $(EXTRAARGS_SPIDERMONKEY) export
else
	cd $(BUILDDIR)/spidermonkey/js-$(VERSION_SPIDERMONKEY)/js/src && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET)  --with-system-nspr --with-nspr-prefix=$(GCCSDK_INSTALL_ENV) $(EXTRAARGS_SPIDERMONKEY)
	cd $(BUILDDIR)/spidermonkey/js-$(VERSION_SPIDERMONKEY)/js/src && $(env) make 
	cd $(BUILDDIR)/spidermonkey/js-$(VERSION_SPIDERMONKEY)/js/src && $(env) make install
endif 
	touch $@

$(BUILDSTEPS)/spidermonkey-src.d: $(BUILDSTEPS)/sourcedir.d $(SOURCEDIR)/spidermonkey-$(VERSION_SPIDERMONKEY).tar.gz
	$(RM) -rf $(BUILDDIR)/spidermonkey
	mkdir -p $(BUILDDIR)/spidermonkey
	cd $(BUILDDIR)/spidermonkey && tar xaf $(SOURCEDIR)/spidermonkey-$(VERSION_SPIDERMONKEY).tar.gz
ifeq ($(VERSION_SPIDERMONKEY),1.7.0)
	mkdir $(BUILDDIR)/spidermonkey/js-$(VERSION_SPIDERMONKEY)
	mv $(BUILDDIR)/spidermonkey/js $(BUILDDIR)/spidermonkey/js-$(VERSION_SPIDERMONKEY)/
	mkdir $(BUILDDIR)/spidermonkey/js-$(VERSION_SPIDERMONKEY)/js/src/RISCOS_OPT.OBJ
endif
	for p in `ls $(RECIPES)/patches/spidermonkey/$(TARGET)-$(VERSION_SPIDERMONKEY)/*.p` ; do patch -d $(BUILDDIR)/spidermonkey/js-$(VERSION_SPIDERMONKEY) -p0 <$$p ; done
	touch $@

$(SOURCEDIR)/spidermonkey-1.8.5.tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://ftp.mozilla.org/pub/mozilla.org/js/js185-1.0.0.tar.gz" $@


$(SOURCEDIR)/spidermonkey-1.7.0.tar.gz:
	$(FETCHSRC) sdk $(subst $(SOURCEDIR)/,,$@) "http://ftp.mozilla.org/pub/mozilla.org/js/js-1.7.0.tar.gz" $@

