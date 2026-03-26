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

# Components for most (if not all) platforms

# Compression libraries

# https://github.com/google/brotli
VERSION_BROTLI := 1.2.0
# https://zlib.net/
VERSION_ZLIB := 1.3.1

# Cryptography

# https://openssl-library.org/
VERSION_OPENSSL := 3.5.4

# Document parsers

# https://github.com/libexpat/libexpat
VERSION_LIBEXPAT := 2.7.3

# Image formats

# https://libjpeg-turbo.org/
VERSION_LIBJPEG_TURBO := 3.1.2
# http://www.libpng.org/pub/png/libpng.html
VERSION_LIBPNG := 1.6.56
# https://developers.google.com/speed/webp/
VERSION_LIBWEBP := 1.6.0

# Network transports and name resolution

# https://c-ares.org/
# https://github.com/c-ares/c-ares/releases/download/
VERSION_LIBCARES := 1.34.5
# https://curl.se/
VERSION_LIBCURL := 8.17.0

# Text processing

# https://github.com/JuliaStrings/utf8proc
VERSION_UTF8PROC := 2.11.3

############################################################################

# Platforms with no native iconv (AmigaOS, Atari, Windows)

# https://www.gnu.org/software/libiconv/
VERSION_LIBICONV := 1.18

# Platforms that don't use libjpeg-turbo (AmigaOS 3, Atari, Windows)

# http://libjpeg.sourceforge.net/
VERSION_LIBJPEG := 8d

# AmigaOS only components

# http://mission-base.com/peter/source/
# 1.7.0: https://github.com/peterGraf/pbl/releases
VERSION_LIBPBL := 1_04
# https://github.com/laurikari/tre/
VERSION_LIBTRE := 0.9.0
#
VERSION_ROADSHOW := 7.64.0-amissl

# Atari only components

# https://www.freetype.org/
VERSION_FREETYPE := 2.14.1

# https://github.com/berkus/hermes-pixel-format/archive/refs/tags/v1.3.3.tar.gz
VERSION_HERMES := 1.3.3
#
VERSION_LDG := SVN-20150827
# Git-20170304
VERSION_LIBCF := CVS-20130415
#
VERSION_WINDOM := 2.0.1

# Windows only components

#
VERSION_LIBGNURX := 2.5.1

############################################################################

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

ifeq ($(TARGET),m68k-unknown-amigaos)
  cflags__ := $(cflags__) -fomit-frame-pointer -m68020-60 -msoft-float
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
		-DBUILD_TESTING=OFF

NS_SDK_AUTOTOOLS_DOC_SUPPRESS=\
		--disable-doc \
		--disable-docs \
		--disable-gtk-doc \
		--disable-gtk-doc-html \
		--disable-documentation \
		--with-xmlto=no \
		--with-fop=no

RECIPES := $(CURDIR)/recipes
SOURCEDIR := $(CURDIR)/sources
BUILDDIR := $(CURDIR)/builddir-$(TARGET)
BUILDSTEPS := $(BUILDDIR)/build-steps

# Tools
# patch: refuse to countenance fuzz
PATCH := patch -F0
PRISTINE_TAR := $(CURDIR)/../tools/pristine-tar
REPACK := $(CURDIR)/../tools/repack

# Compute the SDK components
SDK_ITEMS :=

COMMON_SDK_ITEMS := zlib.d openssl.d libpng.d libcares.d libcurl.d libexpat.d utf8proc.d

LIBCURL_SSL := openssl
LIBCURL_DISABLES := $(patsubst %,--disable-%,ftp file dict ldap ldaps mqtt rtsp telnet gopher tftp pop3 imap smb smtp manual)
FREETYPE_ENV := LIBPNG_CFLAGS=$$($(GCCSDK_INSTALL_ENV)/bin/libpng-config --cflags) LIBPNG_LDFLAGS=$$($(GCCSDK_INSTALL_ENV)/bin/libpng-config --ldflags)

# Openssl target has to alter for mingw
TARGET_OPENSSL:=$(TARGET)

# Target Specific setup
ifeq ($(TARGET),arm-riscos-gnueabi)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libbrotli.d $(COMMON_SDK_ITEMS) libjpeg-turbo.d libwebp.d)
  EXTRAARGS_LIBCARES := --disable-shared 'ac_cv_have_decl_getservbyport_r=no'
  LIBCURL_ENV := LIBS="-lbrotlidec -lbrotlicommon"
  EXTRAARGS_LIBCURL := --enable-nonblocking --enable-ares --with-random="/dev/urandom" $(LIBCURL_DISABLES)
  EXTRAARGS_OPENSSL := no-asm
  ENV_OPENSSL := CPPFLAGS="-DOPENSSL_NO_UNIX_SOCK"
endif

ifeq ($(TARGET),arm-unknown-riscos)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libbrotli.d $(COMMON_SDK_ITEMS) libjpeg-turbo.d libwebp.d)
  LIBBROTLI_ENV := CFLAGS="-std=c99"
  EXTRAARGS_LIBCARES := --disable-shared 'ac_cv_have_decl_getservbyport_r=no'
  LIBCURL_ENV := LIBS="-lbrotlidec -lbrotlicommon"
  EXTRAARGS_LIBCURL := --enable-nonblocking --enable-ares --with-random="/dev/urandom" $(LIBCURL_DISABLES)
  # Disable the expat testsuite as it requires C++11, which GCC4.7 does not support
  EXTRAARGS_LIBEXPAT := --with-tests=no
  EXTRAARGS_OPENSSL := no-asm
  ENV_OPENSSL := CPPFLAGS="-DOPENSSL_NO_UNIX_SOCK"
endif

ifeq ($(TARGET),m68k-atari-mint)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d $(COMMON_SDK_ITEMS) libjpeg.d libcf.d ldg.d windom.d hermes.d freetype.d)
  EXTRAARGS_LIBCARES := --disable-shared 'ac_cv_have_decl_getservbyport_r=no'
  EXTRAARGS_LIBCURL := --enable-optimize --enable-nonblocking --enable-ares --with-random="/dev/urandom" --disable-manual --disable-curldebug --disable-thread --disable-threaded-resolver 
  EXTRAARGS_LIBICONV := --enable-extra-encodings
  LIBICONV_ENV := CFLAGS="-DPATH_MAX=1024"
  EXTRAARGS_OPENSSL := no-tests
  ENV_OPENSSL := CPPFLAGS="-DOPENSSL_NO_UNIX_SOCK"
endif

ifeq ($(TARGET),m5475-atari-mint)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d $(COMMON_SDK_ITEMS) libjpeg.d libcf.d ldg.d windom.d hermes.d freetype.d)
  EXTRAARGS_LIBCARES := --disable-shared 'ac_cv_have_decl_getservbyport_r=no'
  EXTRAARGS_LIBCURL := --enable-optimize --enable-nonblocking --enable-ares --with-random="/dev/urandom" --disable-manual --disable-curldebug --disable-thread --disable-threaded-resolver
  EXTRAARGS_LIBICONV := --enable-extra-encodings
  LIBICONV_ENV := CFLAGS="-DPATH_MAX=1024"
  EXTRAARGS_OPENSSL := no-asm no-tests
  ENV_OPENSSL := CPPFLAGS="-DOPENSSL_NO_UNIX_SOCK"
endif

ifeq ($(TARGET),ppc-amigaos)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d tre.d pbl.d $(COMMON_SDK_ITEMS) libjpeg-turbo.d)
  LIBCURL_SSL := amissl
  EXTRAARGS_LIBCARES := --disable-shared
  EXTRAARGS_LIBCURL := --disable-ntlm-wb --without-random --enable-threaded-resolver --disable-ipv6 LIBS=-lpthread
  ENV_OPENSSL := CPPFLAGS="-DOPENSSL_NO_UNIX_SOCK"
endif

ifeq ($(TARGET),m68k-unknown-amigaos)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, roadshow.d libiconv.d tre.d pbl.d $(COMMON_SDK_ITEMS) libjpeg.d)
  EXTRAARGS_LIBCARES := --disable-shared --without-random
  LIBCARES_ENV := LIBS="-lm -lnet -lunix"
  LIBCURL_SSL := amissl
  EXTRAARGS_LIBCURL := --disable-ntlm-wb --without-random --disable-threaded-resolver --with-zlib --disable-rt --disable-ipv6
  LIBICONV_ENV := CFLAGS="-DPATH_MAX=1024 -DRLIMIT_NOFILE=6"
  LIBJPEG_ENV := LDFLAGS="-lm"
  LIBJPEG_TURBO_ENV := LDFLAGS="-lm"
  # XXX: build openssl against bsdsocket.library?
  # ENV_OPENSSL := CFLAGS="-I$(GCCSDK_INSTALL_ENV)/netinclude" CPPFLAGS="-D__NO_NET_API"
  ENV_OPENSSL := CPPFLAGS="-DOPENSSL_NO_UNIX_SOCK"
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

ifeq ($(TARGET),i686-w64-mingw32)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d libgnurx.d $(COMMON_SDK_ITEMS) libjpeg.d libwebp.d)
  EXTRAARGS_LIBCARES := --disable-shared
  EXTRAARGS_LIBCURL := --enable-nonblocking --enable-ares --disable-ldap --without-random
  LIBCURL_ENV := CFLAGS="-DCARES_STATICLIB" LIBS="-lgdi32 -lcrypt32 -liphlpapi"
  TARGET_OPENSSL := mingw
  ENV_OPENSSL := CPPFLAGS="-DOPENSSL_NO_UNIX_SOCK" WINDRES="$(windres__)"
  EXTRAARGS_OPENSSL := no-asm enable-capieng
endif

ifeq ($(TARGET),x86_64-w64-mingw32)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d libgnurx.d $(COMMON_SDK_ITEMS) libjpeg.d libwebp.d)
  EXTRAARGS_LIBCARES := --disable-shared
  EXTRAARGS_LIBCURL := --enable-nonblocking --enable-ares --disable-ldap --without-random
  LIBCURL_ENV := CFLAGS="-DCARES_STATICLIB" LIBS="-lgdi32 -lcrypt32 -liphlpapi"
  TARGET_OPENSSL := mingw64
  EXTRAARGS_OPENSSL := no-asm enable-capieng
  ENV_OPENSSL := CPPFLAGS="-DOPENSSL_NO_UNIX_SOCK" WINDRES="$(windres__)"
endif


ifeq ($(SDK_ITEMS),)
  $(error Unable to compute SDK components for target $(TARGET))
endif

.PHONY: all clean distclean source-archives
all: $(SDK_ITEMS)

source-archives: $(BUILDSTEPS)/sources.d

clean:
	rm -fr $(BUILDDIR)

distclean: clean
	rm -fr $(SOURCEDIR)

# Sourcedir
$(BUILDSTEPS)/sourcedir.d: $(BUILDSTEPS)/builddir.d | $(SOURCEDIR)
	touch $@

$(SOURCEDIR):
	mkdir -p $(SOURCEDIR)

# Builddir
$(BUILDSTEPS)/builddir.d: | $(BUILDSTEPS)
	touch $@

$(BUILDSTEPS):
	mkdir -p $(BUILDSTEPS)

$(BUILDSTEPS)/sources.d: $(BUILDSTEPS)/builddir.d $(foreach item,$(SDK_ITEMS),$(basename $(item))-gensrc.d)
	touch $@

define uc
$(shell echo $(1) | tr a-z A-Z)
endef

# Patch source code from sdk recipes
#
# 1: module name to patch
# 2: path to source code
define patch_source
$(if $(realpath $(RECIPES)/patches/$(1)),for p in `ls $(RECIPES)/patches/$(1)/*.p` ; do $(PATCH) -d $(2) -p0 <$$p ; done)
$(if $(realpath $(RECIPES)/patches/$(1)/$(TARGET)),for p in `ls $(RECIPES)/patches/$(1)/$(TARGET)/*.p` ; do $(PATCH) -d $(2) -p0 <$$p ; done)
endef

# rule to build patched sources from compressed tar
# if tar base is not expicitly specified the
# module name is used.
#
# 1: module name
# 2: module version
# 3: optional tarball suffix (default: .tar.gz)
# 4: optional tar base to which "-$(2)" will be appended (default: $(1))
define patched_source_rule

# directory with patched source
BUILDDIR_$(call uc,$(1)) := $$(BUILDDIR)/$(1)/$(if $(4),$(4),$(1))-$(2)
# original source tarball
SOURCETARBALL_$(call uc,$(1)) := $$(SOURCEDIR)/$(1)_$(2)$(if $(3),$(3),.tar.gz)
# patched source build step
BUILDSTEP_SRC_$(call uc,$(1)) := $$(BUILDSTEPS)/$(1)-src-$(2).d

$$(BUILDSTEPS)/$(1)-src-$(2).d: $$(SOURCETARBALL_$(call uc,$(1)))
	mkdir -p $$(BUILDDIR)/$(1)
	$$(RM) -rf $$(BUILDDIR_$(call uc,$(1)))
	cd $$(BUILDDIR)/$(1) && tar xaf $$<
	$$(call patch_source,$(1),$$(BUILDDIR_$(call uc,$(1))))
	touch $$@

$$(SOURCETARBALL_$(call uc,$(1))): $$(BUILDSTEPS)/sourcedir.d
	cd $$(CURDIR)/.. && $$(PRISTINE_TAR) checkout $$(subst $$(SOURCEDIR)/,,$$@) && mv $$(CURDIR)/../$$(subst $$(SOURCEDIR)/,,$$@) $$@
endef

#######
# zlib
#######

$(eval $(call patched_source_rule,zlib,$(VERSION_ZLIB)))

$(BUILDSTEPS)/zlib.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_ZLIB)
	cd $(BUILDDIR_ZLIB) && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --static
	cd $(BUILDDIR_ZLIB) && $(env) make install
	touch $@

$(BUILDSTEPS)/zlib-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_ZLIB)) "https://zlib.net/fossils/$(subst _,-,$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_ZLIB)))"
	touch $@


###########
# libiconv
###########

$(eval $(call patched_source_rule,libiconv,$(VERSION_LIBICONV)))

$(BUILDSTEPS)/libiconv.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_LIBICONV)
	cd $(BUILDDIR_LIBICONV) && $(env) $(LIBICONV_ENV) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) $(NS_SDK_AUTOTOOLS_DOC_SUPPRESS) --disable-shared $(EXTRAARGS_LIBICONV)
	cd $(BUILDDIR_LIBICONV) && $(env) make install
	touch $@

$(BUILDSTEPS)/libiconv-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_LIBICONV)) "https://ftp.gnu.org/pub/gnu/libiconv/$(subst _,-,$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_LIBICONV)))"
	touch $@

###########
# utf8proc
###########

$(eval $(call patched_source_rule,utf8proc,$(VERSION_UTF8PROC)))

$(BUILDSTEPS)/utf8proc.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_UTF8PROC)
	mkdir -p $(BUILDDIR_UTF8PROC)/ns-sdk-build
	cd $(BUILDDIR_UTF8PROC)/ns-sdk-build && $(env) $(UTF8PROC_ENV) $(NS_SDK_CMAKE_CMD) -DCMAKE_BUILD_TYPE=Release ..
	cd $(BUILDDIR_UTF8PROC)/ns-sdk-build && $(env) make install VERBOSE=1
	touch $@

$(BUILDSTEPS)/utf8proc-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_UTF8PROC)) "https://github.com/JuliaStrings/utf8proc/archive/refs/tags/v$(VERSION_UTF8PROC).tar.gz"
	touch $@

###########
# libgnurx
###########

$(eval $(call patched_source_rule,libgnurx,$(VERSION_LIBGNURX),.tar.gz,mingw-libgnurx))

$(BUILDSTEPS)/libgnurx.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/libgnurx-src2.d
	cd $(BUILDDIR_LIBGNURX) && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) $(NS_SDK_AUTOTOOLS_DOC_SUPPRESS) --enable-static --disable-shared
	cd $(BUILDDIR_LIBGNURX) && $(env) make
	cd $(BUILDDIR_LIBGNURX) && $(env) make install
	touch $@

$(BUILDSTEPS)/libgnurx-src2.d: $(BUILDSTEP_SRC_LIBGNURX)
	cd $(BUILDDIR_LIBGNURX) && $(env) libtoolize --copy
	cd $(BUILDDIR_LIBGNURX) && $(env) aclocal
	cd $(BUILDDIR_LIBGNURX) && $(env) autoconf
	cd $(BUILDDIR_LIBGNURX) && $(env) automake --add-missing
	touch $@

$(BUILDSTEPS)/libgnurx-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_LIBGNURX)) "https://downloads.sourceforge.net/project/mingw/Other/UserContributed/regex/mingw-regex-$(VERSION_LIBGNURX)/mingw-libgnurx-$(VERSION_LIBGNURX)-src.tar.gz"
	touch $@


###################
# atari cf library
###################

$(eval $(call patched_source_rule,libcf,$(VERSION_LIBCF),.tar.bz2,cflib))

$(BUILDSTEPS)/libcf.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_LIBCF)
	$(env) make -C $(BUILDDIR_LIBCF)/cflib CROSSPREFIX=${TARGET}- PREFIX=$(GCCSDK_INSTALL_ENV) CROSS=yes
	mkdir -p $(GCCSDK_INSTALL_ENV)/lib/mshort
	mkdir -p $(GCCSDK_INSTALL_ENV)/lib/m68020-60
	mkdir -p $(GCCSDK_INSTALL_ENV)/lib/m5475
	$(env) make -C $(BUILDDIR_LIBCF)/cflib CROSSPREFIX=${TARGET}- PREFIX=$(GCCSDK_INSTALL_ENV) CROSS=yes install
	touch $@

$(BUILDSTEPS)/libcf-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_LIBCF)) "https://ci.netsurf-browser.org/toolchain/sdk/libcf-$(VERSION_LIBCF).tar.bz2"
	touch $@


#######################
# atari windom library
#######################

$(eval $(call patched_source_rule,windom,$(VERSION_WINDOM)))

$(BUILDSTEPS)/windom.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_WINDOM)
	$(env) make -C $(BUILDDIR_WINDOM)/src -f gcc.mak CROSS=yes CROSSPREFIX=${TARGET} M68K_ATARI_MINT_CFLAGS="-I$(GCCSDK_INSTALL_ENV)/include/" 
	$(env) make -C $(BUILDDIR_WINDOM)/src -f gcc.mak CROSS=yes CROSSPREFIX=${TARGET} PREFIX=$(GCCSDK_INSTALL_ENV) install
	touch $@

$(BUILDSTEPS)/windom-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(basename $(basename $(subst $(SOURCEDIR)/,,$(SOURCETARBALL_WINDOM)))).zip "https://downloads.sourceforge.net/project/windom/windom/$(VERSION_WINDOM)/windom-$(VERSION_WINDOM)-1.zip"
	$(REPACK) $(BUILDDIR)/$(basename $(basename $(subst $(SOURCEDIR)/,,$(SOURCETARBALL_WINDOM)))).zip $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_WINDOM))
	touch $@


####################
# atari ldg library
####################

$(eval $(call patched_source_rule,ldg,$(VERSION_LDG),.tar.bz2))

$(BUILDSTEPS)/ldg.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_LDG)
	$(env) make -C $(BUILDDIR_LDG)/src/devel -f gcc.mak CROSS=yes PREFIX=$(GCCSDK_INSTALL_ENV) CC=${TARGET}-gcc AR=${TARGET}-ar
	cp $(BUILDDIR_LDG)/lib/gcc/libldg.a $(GCCSDK_INSTALL_ENV)/lib/
	cp $(BUILDDIR_LDG)/include/ldg.h $(GCCSDK_INSTALL_ENV)/include/ldg.h
	mkdir -p $(GCCSDK_INSTALL_ENV)/include/ldg
	cp $(BUILDDIR_LDG)/include/ldg/mem.h $(GCCSDK_INSTALL_ENV)/include/ldg/
	touch $@

$(BUILDSTEPS)/ldg-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_LDG)) "http://vincent.riviere.free.fr/soft/m68k-atari-mint/archives/ldg-$(VERSION_LDG).tar.bz2"
	touch $@


####################
# hermes bitmap lib
####################

$(eval $(call patched_source_rule,hermes,$(VERSION_HERMES),.tar.gz,hermes-pixel-format))

$(BUILDSTEPS)/hermes.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_HERMES)
	cd $(BUILDDIR_HERMES) && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) $(NS_SDK_AUTOTOOLS_DOC_SUPPRESS) --disable-shared --disable-x86asm
	cd $(BUILDDIR_HERMES) && $(env) make CFLAGS="-D__BIG_ENDIAN__" install
	touch $@

$(BUILDSTEPS)/hermes-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_HERMES)) "https://github.com/berkus/hermes-pixel-format/archive/refs/tags/v$(VERSION_HERMES).tar.gz"
	touch $@


####################
# freetype font lib
####################

$(eval $(call patched_source_rule,freetype,$(VERSION_FREETYPE)))

$(BUILDSTEPS)/freetype.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_FREETYPE)
	cd $(BUILDDIR_FREETYPE) && $(env) $(FREETYPE_ENV) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) $(NS_SDK_AUTOTOOLS_DOC_SUPPRESS) --disable-shared --with-harfbuzz=no
	cd $(BUILDDIR_FREETYPE) && $(env) make
	cd $(BUILDDIR_FREETYPE) && $(env) make install 
	touch $@

$(BUILDSTEPS)/freetype-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_FREETYPE)) "https://download.savannah.gnu.org/releases/freetype/freetype-$(VERSION_FREETYPE).tar.gz"
	touch $@


#####################
# amiga roadshow sdk
#####################

$(eval $(call patched_source_rule,roadshow,$(VERSION_ROADSHOW)))

$(BUILDSTEPS)/roadshow.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_ROADSHOW)
	mkdir -p $(GCCSDK_INSTALL_ENV)/netinclude
	cp -r $(BUILDDIR_ROADSHOW)/netinclude/* $(GCCSDK_INSTALL_ENV)/netinclude/
	touch $@

$(BUILDSTEPS)/roadshow-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(basename $(basename $(subst $(SOURCEDIR)/,,$(SOURCETARBALL_ROADSHOW)))).lha "https://github.com/chris-y/curl/releases/download/curl-$(subst .,_,$(VERSION_ROADSHOW))/Roadshow-SDK-NS.lha"
	$(REPACK) $(BUILDDIR)/$(basename $(basename $(subst $(SOURCEDIR)/,,$(SOURCETARBALL_ROADSHOW)))).lha $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_ROADSHOW))
	touch $@


########
# regex
########

$(eval $(call patched_source_rule,tre,$(VERSION_LIBTRE),.tar.gz))

$(BUILDSTEPS)/tre.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_TRE)
	cd $(BUILDDIR_TRE) && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) $(NS_SDK_AUTOTOOLS_DOC_SUPPRESS) --disable-shared
	cd $(BUILDDIR_TRE) && $(env) make install
	cp $(RECIPES)/files/libtre/regex.h $(GCCSDK_INSTALL_ENV)/include/regex.h
	touch $@

$(BUILDSTEPS)/tre-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_TRE)) "https://github.com/laurikari/tre/releases/download/v$(VERSION_LIBTRE)/$(subst _,-,$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_TRE)))"
	touch $@


########
# libpbl
########

$(eval $(call patched_source_rule,pbl,$(VERSION_LIBPBL)))
# This one is awkward so we get to override the generated value
BUILDDIR_PBL := $(BUILDDIR)/pbl/pbl_$(VERSION_LIBPBL)_04

$(BUILDSTEPS)/pbl.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_PBL)
	cd $(BUILDDIR_PBL) && $(env) make install PREFIX=$(GCCSDK_INSTALL_ENV)
	touch $@

$(BUILDSTEPS)/pbl-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_PBL)) "https://www.mission-base.com/peter/source/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_PBL))"
	touch $@


#########
# OpenSSL
#########

$(eval $(call patched_source_rule,openssl,$(VERSION_OPENSSL)))

$(BUILDSTEPS)/openssl.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/zlib.d $(BUILDSTEPS)/openssl-src2.d
ifeq ($(LIBCURL_SSL),openssl)
	cd $(BUILDDIR_OPENSSL) && $(env) $(ENV_OPENSSL) ./Configure --prefix=$(GCCSDK_INSTALL_ENV) $(TARGET_OPENSSL) no-shared no-threads $(EXTRAARGS_OPENSSL)
	cd $(BUILDDIR_OPENSSL) && $(env) make install
endif
	touch $@

# prepare the openssl source tree
$(BUILDSTEPS)/openssl-src2.d: $(BUILDSTEP_SRC_OPENSSL)
ifneq ($(realpath $(RECIPES)/files/openssl/$(TARGET)),)
	cp -r $(RECIPES)/files/openssl/$(TARGET)/* $(BUILDDIR_OPENSSL)
endif
	touch $@

$(BUILDSTEPS)/openssl-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_OPENSSL)) "https://github.com/openssl/openssl/releases/download/openssl-$(VERSION_OPENSSL)/$(subst _,-,$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_OPENSSL)))"
	touch $@


#########
# libPNG
#########

$(eval $(call patched_source_rule,libpng,$(VERSION_LIBPNG)))

$(BUILDSTEPS)/libpng.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/zlib.d $(BUILDSTEP_SRC_LIBPNG)
	cd $(BUILDDIR_LIBPNG) && $(env) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) $(NS_SDK_AUTOTOOLS_DOC_SUPPRESS) --disable-shared
	cd $(BUILDDIR_LIBPNG) && $(env) make install
	touch $@

$(BUILDSTEPS)/libpng-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_LIBPNG)) "https://downloads.sourceforge.net/libpng/$(subst _,-,$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_LIBPNG)))"
	touch $@


##########
# libjpeg
##########

$(eval $(call patched_source_rule,libjpeg,$(VERSION_LIBJPEG),.tar.gz,jpeg))

$(BUILDSTEPS)/libjpeg.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_LIBJPEG)
	cd $(BUILDDIR_LIBJPEG) && $(env) $(LIBJPEG_ENV) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) $(NS_SDK_AUTOTOOLS_DOC_SUPPRESS) --disable-shared
	cd $(BUILDDIR_LIBJPEG) && $(env) make install
	touch $@

$(BUILDSTEPS)/libjpeg-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_LIBJPEG)) "https://ijg.org/files/jpegsrc.v$(VERSION_LIBJPEG).tar.gz"
	touch $@


###############
# libjpegturbo
###############

$(eval $(call patched_source_rule,libjpegturbo,$(VERSION_LIBJPEG_TURBO),.tar.gz,libjpeg-turbo))

# build jpeg turbo library with cmake
$(BUILDSTEPS)/libjpeg-turbo.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_LIBJPEGTURBO)
	mkdir -p $(BUILDDIR_LIBJPEGTURBO)/ns-sdk-build
	cd $(BUILDDIR_LIBJPEGTURBO)/ns-sdk-build && $(env) $(LIBJPEG_TURBO_ENV) $(NS_SDK_CMAKE_CMD) -DENABLE_STATIC=ON -DENABLE_SHARED=OFF ..
	cd $(BUILDDIR_LIBJPEGTURBO)/ns-sdk-build && $(env) make install VERBOSE=1
	touch $@

$(BUILDSTEPS)/libjpeg-turbo-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_LIBJPEGTURBO)) "https://github.com/libjpeg-turbo/libjpeg-turbo/releases/download/$(VERSION_LIBJPEG_TURBO)/libjpeg-turbo-$(VERSION_LIBJPEG_TURBO).tar.gz"
	touch $@


############
# libbrotli
############

$(eval $(call patched_source_rule,brotli,$(VERSION_BROTLI)))

# build brotli with cmake
$(BUILDSTEPS)/libbrotli.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_BROTLI)
	mkdir -p $(BUILDDIR_BROTLI)/ns-sdk-build
	cd $(BUILDDIR_BROTLI)/ns-sdk-build && $(env) $(LIBBROTLI_ENV) $(NS_SDK_CMAKE_CMD) -DCMAKE_BUILD_TYPE=Release ..
	cd $(BUILDDIR_BROTLI)/ns-sdk-build && $(env) make install VERBOSE=1
	touch $@

$(BUILDSTEPS)/libbrotli-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_BROTLI)) "https://github.com/google/brotli/archive/refs/tags/v$(VERSION_BROTLI).tar.gz"
	touch $@


##########
# libwebp
##########

$(eval $(call patched_source_rule,libwebp,$(VERSION_LIBWEBP)))

$(BUILDSTEPS)/libwebp.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_LIBWEBP)
	cd $(BUILDDIR_LIBWEBP) && $(env) $(LIBWEBP_ENV) LIBPNG_CONFIG="$(GCCSDK_INSTALL_ENV)/bin/libpng-config --static" ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) $(NS_SDK_AUTOTOOLS_DOC_SUPPRESS) --disable-shared $(EXTRAARGS_LIBWEBP)
	cd $(BUILDDIR_LIBWEBP) && $(env) make install
	touch $@

$(BUILDSTEPS)/libwebp-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_LIBWEBP)) "https://storage.googleapis.com/downloads.webmproject.org/releases/webp/libwebp-$(VERSION_LIBWEBP).tar.gz"
	touch $@


###########
# libcares
###########

$(eval $(call patched_source_rule,libcares,$(VERSION_LIBCARES),.tar.gz,c-ares))

$(BUILDSTEPS)/libcares.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_LIBCARES)
	cd $(BUILDDIR_LIBCARES) && $(cleanenv) $(LIBCARES_ENV) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) $(NS_SDK_AUTOTOOLS_DOC_SUPPRESS) $(EXTRAARGS_LIBCARES)
	cd $(BUILDDIR_LIBCARES) && $(cleanenv) make install
	touch $@

$(BUILDSTEPS)/libcares-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_LIBCARES)) "https://github.com/c-ares/c-ares/releases/download/v$(VERSION_LIBCARES)/c-ares-$(VERSION_LIBCARES).tar.gz"
	touch $@


##########
# libcurl
##########

$(eval $(call patched_source_rule,libcurl,$(VERSION_LIBCURL),.tar.gz,curl))

$(BUILDSTEPS)/libcurl.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/zlib.d $(BUILDSTEPS)/openssl.d $(BUILDSTEPS)/libcares.d $(BUILDSTEP_SRC_LIBCURL)
	cd $(BUILDDIR_LIBCURL) && $(env) CFLAGS="" $(LIBCURL_ENV) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) $(NS_SDK_AUTOTOOLS_DOC_SUPPRESS) --disable-shared --with-$(LIBCURL_SSL) --without-libidn2 --without-libpsl ${EXTRAARGS_LIBCURL}
	cd $(BUILDDIR_LIBCURL) && $(env) make install
	touch $@

$(BUILDSTEPS)/libcurl-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_LIBCURL)) "https://curl.se/download/curl-$(VERSION_LIBCURL).tar.gz"
	touch $@


###########
# libexpat
###########

$(eval $(call patched_source_rule,libexpat,$(VERSION_LIBEXPAT),.tar.gz,expat))

$(BUILDSTEPS)/libexpat.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_LIBEXPAT)
	cd $(BUILDDIR_LIBEXPAT) && $(env) $(LIBEXPAT_ENV) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) $(NS_SDK_AUTOTOOLS_DOC_SUPPRESS) --disable-shared $(EXTRAARGS_LIBEXPAT)
	cd $(BUILDDIR_LIBEXPAT) && $(env) $(LIBEXPAT_ENV) make install
	touch $@

$(BUILDSTEPS)/libexpat-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_LIBEXPAT)) "https://github.com/libexpat/libexpat/releases/download/R_$(subst .,_,$(VERSION_LIBEXPAT))/expat-$(VERSION_LIBEXPAT).tar.gz"
	touch $@
