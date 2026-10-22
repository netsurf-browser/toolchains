#
# NetSurf Project toolchain SDK build
#
# Required environment:
#
# GCCSDK_INSTALL_ENV      -- /path/to/install/prefix
# GCCSDK_INSTALL_CROSSBIN -- /path/to/toolchain/bin

# Supported triplets
KNOWN_TRIPLETS := \
	arm-riscos-gnueabi \
	arm-unknown-riscos \
	m68k-atari-mint \
	m5475-atari-mint \
	m68k-unknown-amigaos \
	ppc-amigaos \
	i686-w64-mingw32 \
	x86_64-w64-mingw32

# Upstream package versions

# Components for most (if not all) platforms

# Compression libraries

# https://github.com/google/brotli
VERSION_BROTLI := 1.2.1
# https://zlib.net/
VERSION_ZLIB := 1.3.2

# Cryptography

# https://openssl-library.org/
VERSION_OPENSSL := 4.0.3

# Document parsers

# https://github.com/libexpat/libexpat
VERSION_LIBEXPAT := 2.9.0

# Image formats

# https://libjpeg-turbo.org/
VERSION_LIBJPEG_TURBO := 3.2.0
# http://www.libpng.org/pub/png/libpng.html
VERSION_LIBPNG := 1.6.59
# https://developers.google.com/speed/webp/
VERSION_LIBWEBP := 1.6.0

# Network transports and name resolution

# https://c-ares.org/
# https://github.com/c-ares/c-ares/releases/download/
VERSION_LIBCARES := 1.34.9
# https://curl.se/
VERSION_LIBCURL := 8.23.0

# Text processing

# https://github.com/JuliaStrings/utf8proc
VERSION_UTF8PROC := 2.12.0

############################################################################

# Platforms with no native iconv (AmigaOS, Atari, Windows)

# https://www.gnu.org/software/libiconv/
VERSION_LIBICONV := 1.19

# Platforms that don't use libjpeg-turbo (AmigaOS 3, Atari, Windows)

# http://libjpeg.sourceforge.net/
VERSION_LIBJPEG := 8d

# AmigaOS only components

# http://mission-base.com/peter/source/
# 1.7.0: https://github.com/peterGraf/pbl/releases
VERSION_LIBPBL := 1.7.0
# https://github.com/laurikari/tre/
VERSION_LIBTRE := 0.9.0
#
VERSION_ROADSHOW := 7.64.0-amissl

# Atari only components

# https://www.freetype.org/
VERSION_FREETYPE := 2.15.0

# https://github.com/berkus/hermes-pixel-format/archive/refs/tags/v1.3.3.tar.gz
VERSION_HERMES := 1.3.3
#
VERSION_LDG := SVN-20150827
#
VERSION_LIBCF := Git-20221026
#
VERSION_WINDOM := 2.0.1

# Windows only components

#
VERSION_LIBGNURX := 2.5.1

############################################################################

# Enforce environmental requirements (unless we
# have been asked to build a target that does not
# require a toolchain)
ifeq ($(findstring source-archives,$(MAKECMDGOALS)),)
  ifeq ($(findstring check-updates,$(MAKECMDGOALS)),)
    ifeq ($(findstring process-updates,$(MAKECMDGOALS)),)
      ifeq ($(GCCSDK_INSTALL_ENV),)
        $(error GCCSDK_INSTALL_ENV not set)
      endif

      ifeq ($(GCCSDK_INSTALL_CROSSBIN),)
        $(error GCCSDK_INSTALL_CROSSBIN not set)
      endif
    endif
  endif
endif

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
ifneq ($(findstring $(MAKECMDGOALS),source-archives check-updates process-updates),)
  # source-archives or *-updates: force a placeholder TARGET
  # (important note: the SDK_ITEMS sanity test further down relies
  # on this not being a KNOWN_TRIPLET)
  TARGET := $(MAKECMDGOALS)
else
  # If not foo.real (i.e. the internal job for "foo"), derive target
  ifeq ($(findstring .real,$(MAKECMDGOALS)),)
    TARGET := $(shell $(cc__) -dumpmachine)
  endif
endif
# Assert we have a target
ifeq ($(TARGET),)
  $(error TARGET not set for task $(MAKECMDGOALS))
endif

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
CHECK_UPDATE := $(CURDIR)/../tools/check-update
PROCESS_UPDATES := $(CURDIR)/../tools/process-updates

# Compute the SDK components
SDK_ITEMS :=

COMMON_SDK_ITEMS := zlib.d openssl.d libpng.d libcares.d libcurl.d libexpat.d utf8proc.d
ALL_SDK_ITEMS := $(addprefix $(BUILDSTEPS)/,$(sort $(COMMON_SDK_ITEMS) libbrotli.d libjpeg-turbo.d libwebp.d libiconv.d libjpeg.d libcf.d ldg.d windom.d hermes.d freetype.d roadshow.d tre.d pbl.d libgnurx.d))

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
  EXTRAARGS_LIBCURL := --enable-nonblocking --enable-ares $(LIBCURL_DISABLES)
  EXTRAARGS_LIBEXPAT := --with-dev-urandom
  EXTRAARGS_OPENSSL := no-asm
  ENV_OPENSSL := CPPFLAGS="-DOPENSSL_NO_UNIX_SOCK"
endif

ifeq ($(TARGET),arm-unknown-riscos)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libbrotli.d $(COMMON_SDK_ITEMS) libjpeg-turbo.d libwebp.d)
  LIBBROTLI_ENV := CFLAGS="$(cflags__) -std=c99"
  EXTRAARGS_LIBCARES := --disable-shared 'ac_cv_have_decl_getservbyport_r=no'
  LIBCURL_ENV := LIBS="-lbrotlidec -lbrotlicommon"
  EXTRAARGS_LIBCURL := --enable-nonblocking --enable-ares $(LIBCURL_DISABLES)
  # Disable the expat testsuite as it requires C++11, which GCC4.7 does not support
  EXTRAARGS_LIBEXPAT := --with-tests=no --with-dev-urandom
  EXTRAARGS_OPENSSL := no-asm
  ENV_OPENSSL := CPPFLAGS="-DOPENSSL_NO_UNIX_SOCK" CFLAGS="$(cflags__) -std=c99"
endif

ifeq ($(TARGET),m68k-atari-mint)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d $(COMMON_SDK_ITEMS) libjpeg.d libcf.d ldg.d windom.d hermes.d freetype.d)
  EXTRAARGS_LIBCARES := --disable-shared 'ac_cv_have_decl_getservbyport_r=no'
  EXTRAARGS_LIBCURL := --enable-optimize --enable-nonblocking --enable-ares --disable-manual --disable-curldebug --disable-thread --disable-threaded-resolver 
  # actually not urandom: we've patched it to use libcrypto
  EXTRAARGS_LIBEXPAT := --with-dev-urandom
  LIBEXPAT_ENV := LDFLAGS="$(ldflags__) -lcrypto"
  EXTRAARGS_LIBICONV := --enable-extra-encodings
  LIBICONV_ENV := CFLAGS="$(cflags__) -DPATH_MAX=1024"
  EXTRAARGS_OPENSSL := no-tests no-ui-console
  ENV_OPENSSL := CPPFLAGS="-DOPENSSL_NO_UNIX_SOCK -DOPENSSL_USE_USLEEP" CFLAGS="$(cflags__) -std=c99"
endif

ifeq ($(TARGET),m5475-atari-mint)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d $(COMMON_SDK_ITEMS) libjpeg.d libcf.d ldg.d windom.d hermes.d freetype.d)
  EXTRAARGS_LIBCARES := --disable-shared 'ac_cv_have_decl_getservbyport_r=no'
  EXTRAARGS_LIBCURL := --enable-optimize --enable-nonblocking --enable-ares --disable-manual --disable-curldebug --disable-thread --disable-threaded-resolver
  # actually not urandom: we've patched it to use libcrypto
  EXTRAARGS_LIBEXPAT := --with-dev-urandom
  LIBEXPAT_ENV := LDFLAGS="$(ldflags__) -lcrypto"
  EXTRAARGS_LIBICONV := --enable-extra-encodings
  LIBICONV_ENV := CFLAGS="$(cflags__) -DPATH_MAX=1024"
  EXTRAARGS_OPENSSL := no-asm no-tests no-ui-console
  ENV_OPENSSL := CPPFLAGS="-DOPENSSL_NO_UNIX_SOCK -DOPENSSL_USE_USLEEP" CFLAGS="$(cflags__) -std=c99"
endif

ifeq ($(TARGET),ppc-amigaos)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d tre.d pbl.d $(COMMON_SDK_ITEMS) libjpeg-turbo.d)
  LIBCURL_SSL := amissl
  EXTRAARGS_LIBCARES := --disable-shared
  EXTRAARGS_LIBCURL := --disable-ntlm-wb --enable-threaded-resolver --disable-ipv6 LIBS=-lpthread
  ifeq ($(LIBCURL_SSL),openssl)
    # OpenSSL
    LIBEXPAT_CRYPTO_LIB := -lcrypto
  else
    # AmiSSL
    LIBEXPAT_CRYPTO_LIB := -lamisslauto
  endif
  LIBEXPAT_ENV := CPPFLAGS="$(cppflags__) -D__USE_INLINE__" LDFLAGS="$(ldflags__) $(LIBEXPAT_CRYPTO_LIB)"
  # actually not urandom: we've patched it to use libcrypto/amissl
  EXTRAARGS_LIBEXPAT := --with-dev-urandom
  ENV_OPENSSL := CPPFLAGS="-DOPENSSL_NO_UNIX_SOCK"
endif

ifeq ($(TARGET),m68k-unknown-amigaos)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, roadshow.d libiconv.d tre.d pbl.d $(COMMON_SDK_ITEMS) libjpeg.d)
  EXTRAARGS_LIBCARES := --disable-shared --without-random
  LIBCARES_ENV := LIBS="-lm -lnet -lunix"
  LIBCURL_SSL := amissl
  EXTRAARGS_LIBCURL := --disable-ntlm-wb --disable-threaded-resolver --with-zlib --disable-rt --disable-ipv6
  LIBICONV_ENV := CFLAGS="$(cflags__) -DPATH_MAX=1024 -DRLIMIT_NOFILE=6"
  LIBJPEG_ENV := LDFLAGS="$(ldflags__) -lm"
  LIBJPEG_TURBO_ENV := LDFLAGS="$(ldflags__) -lm"
  # XXX: build openssl against bsdsocket.library?
  # ENV_OPENSSL := CFLAGS="-I$(GCCSDK_INSTALL_ENV)/netinclude" CPPFLAGS="-D__NO_NET_API"
  ENV_OPENSSL := CPPFLAGS="-DOPENSSL_NO_UNIX_SOCK"
  ifeq ($(LIBCURL_SSL),openssl)
    LIBCURL_ENV := LIBS="-lm -lnet"
  else
    LIBCURL_ENV := CFLAGS="-I$(GCCSDK_INSTALL_ENV)/netinclude -D__NO_NET_API -DHAVE_IOCTLSOCKET_CAMEL_FIONBIO" LIBS="-lm" ac_cv_func_realpath=no
  endif
  ifeq ($(LIBCURL_SSL),openssl)
    # OpenSSL
    LIBEXPAT_CRYPTO_LIB := -lcrypto
  else
    # AmiSSL
    LIBEXPAT_CRYPTO_LIB := -lamisslauto
  endif
  # XXX: libgcc is meant to be implied, but for some reason that
  # results in ld evaluating it after libm and thus failing to find symbols.
  # Hack around it by forcing the link order here.
  LIBEXPAT_ENV := LDFLAGS="$(ldflags__) $(LIBEXPAT_CRYPTO_LIB) -lgcc -lm"
  # actually not urandom: we've patched it to use libcrypto/amissl
  EXTRAARGS_LIBEXPAT := --with-dev-urandom
endif

ifeq ($(TARGET),i686-w64-mingw32)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d libgnurx.d $(COMMON_SDK_ITEMS) libjpeg.d libwebp.d)
  EXTRAARGS_LIBCARES := --disable-shared
  EXTRAARGS_LIBCURL := --enable-nonblocking --enable-ares --disable-ldap
  LIBCURL_ENV := CFLAGS="$(cflags__) -DCARES_STATICLIB" LIBS="-lgdi32 -lcrypt32 -liphlpapi"
  TARGET_OPENSSL := mingw
  ENV_OPENSSL := CPPFLAGS="-DOPENSSL_NO_UNIX_SOCK" WINDRES="$(windres__)"
  EXTRAARGS_OPENSSL := no-asm
endif

ifeq ($(TARGET),x86_64-w64-mingw32)
  SDK_ITEMS := $(addprefix $(BUILDSTEPS)/, libiconv.d libgnurx.d $(COMMON_SDK_ITEMS) libjpeg.d libwebp.d)
  EXTRAARGS_LIBCARES := --disable-shared
  EXTRAARGS_LIBCURL := --enable-nonblocking --enable-ares --disable-ldap
  LIBCURL_ENV := CFLAGS="$(cflags__) -DCARES_STATICLIB" LIBS="-lgdi32 -lcrypt32 -liphlpapi"
  TARGET_OPENSSL := mingw64
  EXTRAARGS_OPENSSL := no-asm
  ENV_OPENSSL := CPPFLAGS="-DOPENSSL_NO_UNIX_SOCK" WINDRES="$(windres__)"
endif


ifneq ($(findstring $(TARGET),$(KNOWN_TRIPLETS)),)
  ifeq ($(SDK_ITEMS),)
    $(error Unable to compute SDK components for target $(TARGET))
  endif
endif

.PHONY: all clean distclean source-archives source-archives.real check-updates process-updates
all: $(SDK_ITEMS)

check-updates: $(foreach item,$(ALL_SDK_ITEMS),$(basename $(item))-chkupd.d)

process-updates:
	@make -C $(CURDIR) check-updates | $(PROCESS_UPDATES) -

clean:
	rm -fr $(BUILDDIR)

distclean: clean
	rm -fr $(SOURCEDIR)

define force_target
@TARGET=$(1) make --no-print-directory $(2)

endef

source-archives:
	$(foreach t,$(KNOWN_TRIPLETS),$(call force_target,$(t),$(MAKECMDGOALS).real))

source-archives.real: $(BUILDSTEPS)/sources.d

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

$(BUILDSTEPS)/zlib-chkupd.d:
	@$(CHECK_UPDATE) zlib $(VERSION_ZLIB) "https://api.github.com/repos/madler/zlib/releases" "zlib-(\d+\.\d+(\.\d+)?).tar.gz"


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

$(BUILDSTEPS)/libiconv-chkupd.d:
	@$(CHECK_UPDATE) libiconv $(VERSION_LIBICONV) "https://ftp.gnu.org/pub/gnu/libiconv/" "libiconv-(\d+\.\d+(\.\d+)?).tar.gz"


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

$(BUILDSTEPS)/utf8proc-chkupd.d:
	@$(CHECK_UPDATE) utf8proc $(VERSION_UTF8PROC) "https://api.github.com/repos/JuliaStrings/utf8proc/releases" "utf8proc-(\d+\.\d+(\.\d+)?).tar.gz"


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

# This component has not changed since 2007 and Sourceforge is awkward.
# Produce a pre-canned answer and assume it'll never change.
$(BUILDSTEPS)/libgnurx-chkupd.d:
	@echo '{"component": "libgnurx", "current": "$(VERSION_LIBGNURX)", "newest": "$(VERSION_LIBGNURX)", "url": null}'


###################
# atari cf library
###################

$(eval $(call patched_source_rule,libcf,$(VERSION_LIBCF),.tar.bz2,cflib))

$(BUILDSTEPS)/libcf.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_LIBCF)
	$(env) make -C $(BUILDDIR_LIBCF) CROSSPREFIX=${TARGET}- PREFIX=$(GCCSDK_INSTALL_ENV) CROSS=yes
	mkdir -p $(GCCSDK_INSTALL_ENV)/lib/mshort
	mkdir -p $(GCCSDK_INSTALL_ENV)/lib/m68020-60
	mkdir -p $(GCCSDK_INSTALL_ENV)/lib/m5475
	$(env) make -C $(BUILDDIR_LIBCF) CROSSPREFIX=${TARGET}- PREFIX=$(GCCSDK_INSTALL_ENV) CROSS=yes install
	touch $@

$(BUILDSTEPS)/libcf-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_LIBCF)) "https://ci.netsurf-browser.org/toolchain/sdk/libcf-$(VERSION_LIBCF).tar.bz2"
	touch $@

$(BUILDSTEPS)/libcf-chkupd.d:
	@$(CHECK_UPDATE) libcf $(VERSION_LIBCF) "http://vincent.riviere.free.fr/soft/m68k-atari-mint/" "cflib-([A-Za-z]+-\d+).tar.bz2"


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

# This component has not changed since 2006 and Sourceforge is awkward.
# Produce a pre-canned answer and assume it'll never change.
$(BUILDSTEPS)/windom-chkupd.d:
	@echo '{"component": "windom", "current": "$(VERSION_WINDOM)", "newest": "$(VERSION_WINDOM)", "url": null}'


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

$(BUILDSTEPS)/ldg-chkupd.d:
	@$(CHECK_UPDATE) ldg $(VERSION_LDG) "http://vincent.riviere.free.fr/soft/m68k-atari-mint/" "ldg-([A-Za-z]+-\d+).tar.bz2"


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

# No release artefacts, so look for the auto-generated tag sources instead
$(BUILDSTEPS)/hermes-chkupd.d:
	@$(CHECK_UPDATE) hermes $(VERSION_HERMES) "https://github.com/berkus/hermes-pixel-format/tags" "v(\d+\.\d+(\.\d+)?).tar.gz"


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

$(BUILDSTEPS)/freetype-chkupd.d:
	@$(CHECK_UPDATE) freetype $(VERSION_FREETYPE) "https://download.savannah.gnu.org/releases/freetype/" "freetype-(\d+\.\d+(\.\d+)?).tar.gz"


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

# A Chris special: assume he will track this as automation is hard.
$(BUILDSTEPS)/roadshow-chkupd.d:
	@echo '{"component": "roadshow", "current": "$(VERSION_ROADSHOW)", "newest": "$(VERSION_ROADSHOW)", "url": null}'


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

$(BUILDSTEPS)/tre-chkupd.d:
	@$(CHECK_UPDATE) tre $(VERSION_LIBTRE) "https://api.github.com/repos/laurikari/tre/releases" "tre-(\d+\.\d+(\.\d+)?).tar.gz"


########
# libpbl
########

$(eval $(call patched_source_rule,pbl,$(VERSION_LIBPBL)))

$(BUILDSTEPS)/pbl.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEP_SRC_PBL)
	cd $(BUILDDIR_PBL) && $(env) make install PREFIX=$(GCCSDK_INSTALL_ENV)
	touch $@

$(BUILDSTEPS)/pbl-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_PBL)) "https://www.mission-base.com/peter/source/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_PBL))"
	touch $@

# No release artefacts, so look for the auto-generated tag sources instead
$(BUILDSTEPS)/pbl-chkupd.d:
	@$(CHECK_UPDATE) pbl $(subst _,.,$(VERSION_LIBPBL)) "https://github.com/peterGraf/pbl/tags" "v(\d+\.\d+(\.\d+)?).tar.gz"


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

$(BUILDSTEPS)/openssl-chkupd.d:
	@$(CHECK_UPDATE) openssl $(VERSION_OPENSSL) "https://api.github.com/repos/openssl/openssl/releases" "openssl-(\d+\.\d+(\.\d+)?).tar.gz"


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

$(BUILDSTEPS)/libpng-chkupd.d:
	@$(CHECK_UPDATE) libpng $(VERSION_LIBPNG) "https://libpng.org/pub/png/libpng.html" "libpng-(\d+\.\d+(\.\d+)?).tar.gz"


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

# Pin v8 as we require that A{B,P}I
$(BUILDSTEPS)/libjpeg-chkupd.d:
	@$(CHECK_UPDATE) libjpeg $(VERSION_LIBJPEG) "https://ijg.org/files/" "jpegsrc.v(8[a-z]?).tar.gz"


###############
# libjpeg-turbo
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

$(BUILDSTEPS)/libjpeg-turbo-chkupd.d:
	@$(CHECK_UPDATE) libjpegturbo $(VERSION_LIBJPEG_TURBO) "https://api.github.com/repos/libjpeg-turbo/libjpeg-turbo/releases" "libjpeg-turbo-(\d+\.\d+(\.\d+)?).tar.gz"


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

# No source release artefact, so look for the auto-generated tag sources instead
$(BUILDSTEPS)/libbrotli-chkupd.d:
	@$(CHECK_UPDATE) brotli $(VERSION_BROTLI) "https://github.com/google/brotli/tags" "v(\d+\.\d+(\.\d+)?).tar.gz"


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

$(BUILDSTEPS)/libwebp-chkupd.d:
	@$(CHECK_UPDATE) libwebp $(VERSION_LIBWEBP) "https://storage.googleapis.com/downloads.webmproject.org/releases/webp/index.html" "libwebp-(\d+\.\d+(\.\d+)?).tar.gz"


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

$(BUILDSTEPS)/libcares-chkupd.d:
	@$(CHECK_UPDATE) libcares $(VERSION_LIBCARES) "https://api.github.com/repos/c-ares/c-ares/releases" "c-ares-(\d+\.\d+(\.\d+)?).tar.gz"


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

$(BUILDSTEPS)/libcurl-chkupd.d:
	@$(CHECK_UPDATE) libcurl $(VERSION_LIBCURL) "https://api.github.com/repos/curl/curl/releases" "curl-(\d+\.\d+(\.\d+)?).tar.gz"


###########
# libexpat
###########

$(eval $(call patched_source_rule,libexpat,$(VERSION_LIBEXPAT),.tar.gz,expat))

$(BUILDSTEPS)/libexpat.d: $(BUILDSTEPS)/builddir.d $(BUILDSTEPS)/openssl.d $(BUILDSTEPS)/libexpat-src2.d
	cd $(BUILDDIR_LIBEXPAT) && $(env) $(LIBEXPAT_ENV) ./configure --prefix=$(GCCSDK_INSTALL_ENV) --target=$(TARGET) --host=$(TARGET) $(NS_SDK_AUTOTOOLS_DOC_SUPPRESS) --disable-shared $(EXTRAARGS_LIBEXPAT)
	cd $(BUILDDIR_LIBEXPAT) && $(env) $(LIBEXPAT_ENV) make install
	touch $@

$(BUILDSTEPS)/libexpat-src2.d: $(BUILDSTEP_SRC_LIBEXPAT)
ifeq ($(LIBCURL_SSL),amissl)
ifneq ($(realpath $(RECIPES)/patches/libexpat/$(TARGET)),)
	for p in `ls $(RECIPES)/patches/libexpat/$(TARGET)/*.p.amissl` ; do $(PATCH) -d $(BUILDDIR_LIBEXPAT) -p0 <$$p ; done
endif
endif
	touch $@

$(BUILDSTEPS)/libexpat-gensrc.d: $(BUILDSTEPS)/builddir.d
	wget -q -O $(BUILDDIR)/$(subst $(SOURCEDIR)/,,$(SOURCETARBALL_LIBEXPAT)) "https://github.com/libexpat/libexpat/releases/download/R_$(subst .,_,$(VERSION_LIBEXPAT))/expat-$(VERSION_LIBEXPAT).tar.gz"
	touch $@

$(BUILDSTEPS)/libexpat-chkupd.d:
	@$(CHECK_UPDATE) libexpat $(VERSION_LIBEXPAT) "https://api.github.com/repos/libexpat/libexpat/releases" "expat-(\d+\.\d+(\.\d+)?).tar.gz"
