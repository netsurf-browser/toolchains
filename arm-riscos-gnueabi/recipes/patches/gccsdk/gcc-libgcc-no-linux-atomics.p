Index: gcc/libgcc.config.arm.t-riscos-gnueabihf
===================================================================
--- gcc/libgcc.config.arm.t-riscos-gnueabihf	(revision 7698)
+++ gcc/libgcc.config.arm.t-riscos-gnueabihf	(working copy)
@@ -1,36 +1,2 @@
-# Copyright (C) 2011-2012 Free Software Foundation, Inc.
-# Contributed by John Tytgat (John.Tytgat@aaug.net).
-#
-# This file is part of GCC.
-#
-# GCC is free software; you can redistribute it and/or modify
-# it under the terms of the GNU General Public License as published by
-# the Free Software Foundation; either version 3, or (at your option)
-# any later version.
-#
-# GCC is distributed in the hope that it will be useful,
-# but WITHOUT ANY WARRANTY; without even the implied warranty of
-# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-# GNU General Public License for more details.
-#
-# You should have received a copy of the GNU General Public License
-# along with GCC; see the file COPYING3.  If not see
-# <http://www.gnu.org/licenses/>.
-
-# vvv Part of config/arm/t-bpabi
-LIB2ADD_ST += $(srcdir)/config/arm/fp16.c
-
-LIB2ADDEH = $(srcdir)/config/arm/unwind-arm.c \
-  $(srcdir)/config/arm/libunwind.S \
-  $(srcdir)/config/arm/pr-support.c $(srcdir)/unwind-c.c
-# ^^^
-
-ifeq (,$(findstring mlibscl,$(CFLAGS))$(findstring mmodule,$(CFLAGS)))
-# For shared libgcc building:
-EXTRA_PARTS += crtbeginS.o crtendS.o
-gcc_s_compile += -fPIC
-SHLIB_LDFLAGS=-fPIC
-include $(srcdir)/config/t-slibgcc
-include $(srcdir)/config/t-slibgcc-nolc-override
-endif
-
+# Use a version of div0 which raises SIGFPE, and a special __clear_cache.
+LIB1ASMFUNCS := $(filter-out _dvmd_tls,$(LIB1ASMFUNCS)) _dvmd_lnx _clear_cache
Index: gcc/libgcc.config.host.p
===================================================================
--- gcc/libgcc.config.host.p	(revision 7698)
+++ gcc/libgcc.config.host.p	(working copy)
@@ -20,7 +20,7 @@
  	;;
 +arm*-riscos-gnueabi*)
 +	tmake_file="${tmake_file} arm/t-arm t-fixedpoint-gnu-prefix t-crtfm"
-+	tmake_file="${tmake_file} arm/t-elf arm/t-bpabi arm/t-linux-eabi t-slibgcc-libgcc t-slibgcc-nolc-override"
++	tmake_file="${tmake_file} arm/t-elf arm/t-bpabi arm/t-riscos-gnueabihf t-slibgcc-libgcc t-slibgcc-nolc-override"
 +	tm_file="$tm_file arm/bpabi-lib.h"
 +	unwind_header=config/arm/unwind-arm.h
 +	tmake_file="$tmake_file t-softfp-sfdf t-softfp-excl arm/t-softfp t-softfp"
