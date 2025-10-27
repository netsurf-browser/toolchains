Index: Source/Makefile
===================================================================
--- Source/Makefile	(revision 477)
+++ Source/Makefile	(working copy)
@@ -3,8 +3,8 @@
 # already be built upfront.
 # Written by OSLib Development Team.
 
-# Build type: CROSSGCC_AOF, CROSSGCC_ELF (default), [NORCROFT_AOF: not
-# supported yet]
+# Build type: CROSSGCC_AOF, CROSSGCC_ELF (default), CROSSGCC_ELFEABI,
+# [NORCROFT_AOF: not supported yet]
 export BUILDTYPE ?= CROSSGCC_ELF
 # Possible values: "SOFTFPU" (e.g. for UnixLib usage), "HARDFPU" (e.g. for
 # SharedCLibrary usage) and "MODULE" (latter also meaning HARDFPU but the
@@ -12,9 +12,10 @@
 export ELFOBJECTTYPE ?= SOFTFPU
 
 ifeq ($(BUILDTYPE),CROSSGCC_ELF)
+else ifeq ($(BUILDTYPE),CROSSGCC_ELFEABI)
 else ifeq ($(BUILDTYPE),CROSSGCC_AOF)
 else
-$(error Unknown BUILDTYPE value, possible values are CROSSGCC_AOF and CROSSGCC_ELF)
+$(error Unknown BUILDTYPE value, possible values are CROSSGCC_AOF, CROSSGCC_ELF, and CROSSGCC_ELFEABI)
 endif
 include Makefile.setup.$(BUILDTYPE)
 
Index: Source/Makefile.inc
===================================================================
--- Source/Makefile.inc	(revision 477)
+++ Source/Makefile.inc	(working copy)
@@ -65,6 +65,9 @@
 ifeq ($(BUILDTYPE),CROSSGCC_ELF)
 .ELFHdr.Hdr:
 		if [ -s "$<" ]; then $(CP) $< $@; fi
+else ifeq ($(BUILDTYPE),CROSSGCC_ELFEABI)
+.ELFHdr.Hdr:
+		if [ -s "$<" ]; then $(CP) $< $@; fi
 else
 .AOFHdr.Hdr:
 		if [ -s "$<" ]; then $(CP) $< $@; fi
@@ -98,6 +101,9 @@
 ifeq ($(BUILDTYPE),CROSSGCC_ELF)
 .gas.o:
 		$(AS) -o $@ $<
+else ifeq ($(BUILDTYPE),CROSSGCC_ELFEABI)
+.gas.o:
+		$(AS) -o $@ $<
 else
 .asm.o:
 		$(AS) -o $@ $<
Index: Source/Makefile.setup.CROSSGCC_ELFEABI
===================================================================
--- Source/Makefile.setup.CROSSGCC_ELFEABI	(nonexistent)
+++ Source/Makefile.setup.CROSSGCC_ELFEABI	(working copy)
@@ -0,0 +1,62 @@
+# Settings for ELF building
+
+ifndef GCCSDK_INSTALL_CROSSBIN
+$(error Environment variable GCCSDK_INSTALL_CROSSBIN is not defined)
+endif
+ifndef GCCSDK_INSTALL_ENV
+$(error Environment variable GCCSDK_INSTALL_ENV is not defined)
+endif
+
+ifeq ($(ELFOBJECTTYPE),SOFTFPU)
+CCFLAGS = -mfloat-abi=soft
+DEFMODFLAGS = -float-abi soft
+else ifeq ($(ELFOBJECTTYPE),HARDFPU)
+CCFLAGS = -mlibscl
+DEFMODFLAGS = -float-abi hard
+else ifeq ($(ELFOBJECTTYPE),MODULE)
+CCFLAGS = -mmodule
+DEFMODFLAGS = -float-abi hard
+else
+$(error Unknown ELFOBJECTTYPE value, possible values are SOFTFPU, HARDFPU and MODULE)
+endif
+
+export SOURCEDIR := $(abspath .)
+export BINDIR := $(SOURCEDIR)/../Bin
+export BUILDDIR := $(SOURCEDIR)/../Build
+export RELEASEDIR := $(SOURCEDIR)/../Release
+export RELEASEBINDIR := $(SOURCEDIR)/../Release/$(BUILDTYPE)/$(ELFOBJECTTYPE)
+export EXAMPLESDIR := $(SOURCEDIR)/../examples
+export DOCSDIR := $(SOURCEDIR)/../docs
+
+export TOPHEADERDIR := $(BUILDDIR)/Header
+export HEADERDIR := $(TOPHEADERDIR)/oslib
+export CSTRONGDIR := $(BUILDDIR)/CStrong
+export VAPIDIR := $(BUILDDIR)/\!Vala/vapi
+export VAPIHDIR := $(BUILDDIR)/vapih
+export OBJDIR := $(BUILDDIR)/Objs
+export OBJEXCEPTDIR := $(BUILDDIR)/Objs/Exceptions
+export OSLIBSUPPORTDIR := $(BUILDDIR)/OSLibSupport
+
+export BYTEWIDE := $(SOURCEDIR)/ByteWide
+export DEFMOD := $(BINDIR)/defmod -asmtype gccelfeabi $(DEFMODFLAGS)
+export REDUCEAOF := @echo Not needed for ELF build: reduceaof
+export BINDHELP := $(BINDIR)/bindhelp
+
+export PATH := $(GCCSDK_INSTALL_CROSSBIN):$(PATH)
+
+export CC := arm-riscos-gnueabi-gcc $(CCFLAGS)
+export CXX := arm-riscos-gnueabi-g++ $(CCFLAGS)
+export AS := arm-riscos-gnueabi-gcc -c -xassembler-with-cpp $(CCFLAGS) -I.. -I$(SOURCEDIR)/Types
+export LIBFILE := arm-riscos-gnueabi-ar rs
+export MKDIR := mkdir -p
+export RM := rm -rf
+export CP := cp -r
+export INSTALL := $(GCCSDK_INSTALL_ENV)/ro-install
+
+export ASMEXT := .gas
+export EXEEXT := ,e1f
+export LIBSUFFIX := a
+
+export OSLIBLIB := $(BUILDDIR)/libOSLib32.$(LIBSUFFIX)
+export OSLIBSUPPORTLIB := $(OSLIBSUPPORTDIR)/libOSLibSupport32.$(LIBSUFFIX)
+export OSLIBCSTRONG := $(BUILDDIR)/OSLib,3d6
Index: Makefile
===================================================================
--- Makefile	(revision 477)
+++ Makefile	(working copy)
@@ -29,6 +29,10 @@
 install-elf: oslib-elf
 	$(MAKE) -C Source install BUILDTYPE=CROSSGCC_ELF
 
+# Installs the ELF headers & library in GCCSDK environment.
+install-elfeabi: oslib-elfeabi
+	$(MAKE) -C Source install BUILDTYPE=CROSSGCC_ELFEABI
+
 # Installs the AOF headers & library in GCCSDK environment.
 install-aof: oslib-aof
 	$(MAKE) -C Source install BUILDTYPE=CROSSGCC_AOF
@@ -49,6 +53,10 @@
 	$(MAKE) -C Source oslib BUILDTYPE=CROSSGCC_ELF
 	$(MAKE) -C Source oslibsupport BUILDTYPE=CROSSGCC_ELF
 
+oslib-elfeabi: tools
+	$(MAKE) -C Source oslib BUILDTYPE=CROSSGCC_ELFEABI
+	$(MAKE) -C Source oslibsupport BUILDTYPE=CROSSGCC_ELFEABI
+
 vapis: tools
 	$(MAKE) -C Source vapis BUILDTYPE=CROSSGCC_ELF
 
