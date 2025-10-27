From 063184c10762d5b798671bb49a55cf2308b3c134 Mon Sep 17 00:00:00 2001
From: Georgios Sokianos <walkero@gmail.com>
Date: Fri, 29 Nov 2024 19:58:52 +0000
Subject: [PATCH] Changes for compiling for AmigaOS 4 using clib4

---
 bfd/Makefile.am                       |    4 +-
 bfd/Makefile.in                       |    5 +-
 bfd/bfd-in2.h                         |    6 +
 bfd/config.bfd                        |    4 +
 bfd/configure                         |    3 +-
 bfd/configure.ac                      |    1 +
 bfd/{cpu-bfin.c => elf-amigaos.c}     |   44 +-
 bfd/{elf-nacl.h => elf-amigaos.h}     |   11 +-
 bfd/elf-bfd.h                         |    3 +-
 bfd/elf32-ppc.c                       |  247 +++++-
 bfd/elflink.c                         |    2 +
 bfd/libbfd.h                          |    4 +
 bfd/po/SRC-POTFILES.in                |    2 +
 bfd/reloc.c                           |   10 +-
 bfd/targets.c                         |    2 +
 binutils/elfcomm.c                    |   12 +
 binutils/objcopy.c                    |   79 +-
 binutils/readelf.c                    |    4 +
 binutils/version.c                    |   13 +
 config.sub                            |   10 +-
 elfcpp/powerpc.h                      |    6 +
 gas/Makefile.am                       |    1 +
 gas/Makefile.in                       |    1 +
 gas/config/tc-ppc.c                   |   26 +-
 gas/config/te-amigaos.h               |   14 +
 gas/configure.tgt                     |    1 +
 gas/doc/.dirstamp                     |    0
 gas/po/POTFILES.in                    |    1 +
 include/elf/amigaos.h                 |   27 +
 include/elf/ppc.h                     |    6 +
 include/filenames.h                   |   18 +
 include/libiberty.h                   |    5 +
 ld/Makefile.am                        |    2 +
 ld/Makefile.in                        |    2 +
 ld/configure.tgt                      |    8 +
 ld/emulparams/amigaos.sh              |   31 +
 ld/ldctor.c                           |   23 +-
 ld/ldlang.c                           |    7 +
 ld/ldmain.c                           |    7 +-
 ld/po/BLD-POTFILES.in                 |    2 +
 ld/scripttempl/{elf.sc => amigaos.sc} |    4 +-
 libiberty/Makefile.in                 |   14 +-
 libiberty/configure                   |    1 +
 libiberty/configure.ac                |    1 +
 libiberty/fnmatch.c                   |    6 +-
 libiberty/lbasename.c                 |   20 +
 libiberty/make-temp-file.c            |    2 +
 libiberty/pex-amigaos.c               |  325 +++++++
 82 files changed, 2536 insertions(+), 89 deletions(-)
 copy bfd/{cpu-bfin.c => elf-amigaos.c} (51%)
 copy bfd/{elf-nacl.h => elf-amigaos.h} (73%)
 create mode 100644 gas/config/te-amigaos.h
 create mode 100644 include/elf/amigaos.h
 create mode 100644 ld/emulparams/amigaos.sh
 copy ld/scripttempl/{elf.sc => amigaos.sc} (99%)
 create mode 100644 libiberty/pex-amigaos.c

diff --git a/bfd/Makefile.am b/bfd/Makefile.am
index e1692e7f8aa8475c8d9fb41f5b35a342752c3987..a884108ff983528b8d3f13f440b3b1e04cb9e260 100644
--- bfd/Makefile.am
+++ bfd/Makefile.am
@@ -289,12 +289,13 @@ BFD32_BACKENDS = \
 	elf-sframe.lo \
 	elf-ifunc.lo \
 	elf-m10200.lo \
 	elf-m10300.lo \
 	elf-nacl.lo \
 	elf-strtab.lo \
+	elf-amigaos.lo \
 	elf-vxworks.lo \
 	elf.lo \
 	elf32-am33lin.lo \
 	elf32-arc.lo \
 	elf32-arm.lo \
 	elf32-avr.lo \
@@ -424,12 +425,13 @@ BFD32_BACKENDS_CFILES = \
 	elf-sframe.c \
 	elf-ifunc.c \
 	elf-m10200.c \
 	elf-m10300.c \
 	elf-nacl.c \
 	elf-strtab.c \
+	elf-amigaos.c \
 	elf-vxworks.c \
 	elf.c \
 	elf32-am33lin.c \
 	elf32-arc.c \
 	elf32-arm.c \
 	elf32-avr.c \
@@ -703,13 +705,13 @@ SOURCE_HFILES = \
 	elf32-dlx.h elf32-hppa.h elf32-m68hc1x.h elf32-m68k.h \
 	elf32-metag.h elf32-nds32.h elf32-nios2.h elf32-ppc.h \
 	elf32-rx.h elf32-score.h elf32-sh-relocs.h elf32-spu.h \
 	elf32-tic6x.h elf32-tilegx.h elf32-tilepro.h elf32-v850.h \
 	elf64-hppa.h elf64-ppc.h elf64-tilegx.h \
 	elf-bfd.h elfcode.h elfcore.h elf-hppa.h elf-linker-x86.h \
-	elf-linux-core.h elf-nacl.h elf-s390.h elf-vxworks.h \
+	elf-linux-core.h elf-nacl.h elf-s390.h elf-amigaos.h elf-vxworks.h \
 	elfxx-aarch64.h elfxx-ia64.h elfxx-mips.h elfxx-riscv.h \
 	elfxx-sparc.h elfxx-tilegx.h elfxx-x86.h elfxx-loongarch.h \
 	genlink.h go32stub.h \
 	libaout.h libbfd.h libcoff.h libecoff.h libhppa.h \
 	libpei.h libxcoff.h \
 	mach-o.h \
diff --git a/bfd/Makefile.in b/bfd/Makefile.in
index 80aed6576435a4ce295db37c155928349fa8a9a7..ff4d61b7a3a096611d24edcc8ce2d8712703bdb0 100644
--- bfd/Makefile.in
+++ bfd/Makefile.in
@@ -758,12 +758,13 @@ BFD32_BACKENDS = \
 	elf-sframe.lo \
 	elf-ifunc.lo \
 	elf-m10200.lo \
 	elf-m10300.lo \
 	elf-nacl.lo \
 	elf-strtab.lo \
+	elf-amigaos.lo \
 	elf-vxworks.lo \
 	elf.lo \
 	elf32-am33lin.lo \
 	elf32-arc.lo \
 	elf32-arm.lo \
 	elf32-avr.lo \
@@ -893,12 +894,13 @@ BFD32_BACKENDS_CFILES = \
 	elf-sframe.c \
 	elf-ifunc.c \
 	elf-m10200.c \
 	elf-m10300.c \
 	elf-nacl.c \
 	elf-strtab.c \
+	elf-amigaos.c \
 	elf-vxworks.c \
 	elf.c \
 	elf32-am33lin.c \
 	elf32-arc.c \
 	elf32-arm.c \
 	elf32-avr.c \
@@ -1169,13 +1171,13 @@ SOURCE_HFILES = \
 	elf32-dlx.h elf32-hppa.h elf32-m68hc1x.h elf32-m68k.h \
 	elf32-metag.h elf32-nds32.h elf32-nios2.h elf32-ppc.h \
 	elf32-rx.h elf32-score.h elf32-sh-relocs.h elf32-spu.h \
 	elf32-tic6x.h elf32-tilegx.h elf32-tilepro.h elf32-v850.h \
 	elf64-hppa.h elf64-ppc.h elf64-tilegx.h \
 	elf-bfd.h elfcode.h elfcore.h elf-hppa.h elf-linker-x86.h \
-	elf-linux-core.h elf-nacl.h elf-s390.h elf-vxworks.h \
+	elf-linux-core.h elf-nacl.h elf-s390.h elf-amigaos.h  elf-vxworks.h \
 	elfxx-aarch64.h elfxx-ia64.h elfxx-mips.h elfxx-riscv.h \
 	elfxx-sparc.h elfxx-tilegx.h elfxx-x86.h elfxx-loongarch.h \
 	genlink.h go32stub.h \
 	libaout.h libbfd.h libcoff.h libecoff.h libhppa.h \
 	libpei.h libxcoff.h \
 	mach-o.h \
@@ -1577,12 +1579,13 @@ distclean-compile:
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/elf-m10200.Plo@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/elf-m10300.Plo@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/elf-nacl.Plo@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/elf-properties.Plo@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/elf-sframe.Plo@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/elf-strtab.Plo@am__quote@
+@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/elf-amigaos.Plo@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/elf-vxworks.Plo@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/elf.Plo@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/elf32-aarch64.Plo@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/elf32-am33lin.Plo@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/elf32-arc.Plo@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/elf32-arm.Plo@am__quote@
diff --git a/bfd/bfd-in2.h b/bfd/bfd-in2.h
index eddfb31b6d51c42c970919534af0f05b65f67604..984865277d6acabc543af5693a6afb31a2560cc5 100644
--- bfd/bfd-in2.h
+++ bfd/bfd-in2.h
@@ -2974,12 +2974,18 @@ instruction.  */
   BFD_RELOC_PPC64_GOT_TLSGD_PCREL34,
   BFD_RELOC_PPC64_GOT_TLSLD_PCREL34,
   BFD_RELOC_PPC64_GOT_TPREL_PCREL34,
   BFD_RELOC_PPC64_GOT_DTPREL_PCREL34,
   BFD_RELOC_PPC64_TLS_PCREL,
 
+/* AmigaOS4 specific relocations */
+  BFD_RELOC_PPC_AMIGAOS_BREL,
+  BFD_RELOC_PPC_AMIGAOS_BREL_LO,
+  BFD_RELOC_PPC_AMIGAOS_BREL_HI,
+  BFD_RELOC_PPC_AMIGAOS_BREL_HA,
+  
 /* IBM 370/390 relocations  */
   BFD_RELOC_I370_D12,
 
 /* The type of reloc used to build a constructor table - at the moment
 probably a 32 bit wide absolute relocation, but the target can choose.
 It generally does map to one of the other relocation types.  */
diff --git a/bfd/config.bfd b/bfd/config.bfd
index 1b0111fd410dcef529bc4d94e2c314678cdd4a2b..8b191314d4653996823189fade3d30a9ecca009a 100644
--- bfd/config.bfd
+++ bfd/config.bfd
@@ -1124,12 +1124,16 @@ case "${targ}" in
 	*-*-aix4.[3456789]* | *-*-aix[56789]*)
 	want64=true;;
 	*)
 	targ_cflags=-DSMALL_ARCHIVE;;
     esac
     ;;
+  powerpc-*-amiga*)
+    targ_defvec=powerpc_elf32_amigaos_vec
+    targ_selvecs="powerpc_elf32_vec"
+    ;;	
 #ifdef BFD64
   powerpc64-*-aix*)
     targ_defvec=rs6000_xcoff64_vec
     targ_selvecs=rs6000_xcoff_vec
     want64=true
     ;;
diff --git a/bfd/configure b/bfd/configure
index e5d464378f872ab643679197f18a2209b7bc4631..a5754abfb0dfe3f238da79d6421c0ab5a1ee5c8e 100755
--- bfd/configure
+++ bfd/configure
@@ -13777,13 +13777,14 @@ do
     pdp11_aout_vec)		 tb="$tb pdp11.lo" ;;
     pef_vec)			 tb="$tb pef.lo" ;;
     pef_xlib_vec)		 tb="$tb pef.lo" ;;
     pj_elf32_vec)		 tb="$tb elf32-pj.lo elf32.lo $elf" ;;
     pj_elf32_le_vec)		 tb="$tb elf32-pj.lo elf32.lo $elf" ;;
     powerpc_boot_vec)		 tb="$tb ppcboot.lo" ;;
-    powerpc_elf32_vec)		 tb="$tb elf32-ppc.lo elf-vxworks.lo elf32.lo $elf" ;;
+    powerpc_elf32_amigaos_vec)	tb="$tb elf32-ppc.lo elf-amigaos.lo elf32.lo $elf" ;;
+	powerpc_elf32_vec)		 tb="$tb elf32-ppc.lo elf-vxworks.lo elf32.lo $elf" ;;
     powerpc_elf32_le_vec)	 tb="$tb elf32-ppc.lo elf-vxworks.lo elf32.lo $elf" ;;
     powerpc_elf32_fbsd_vec)	 tb="$tb elf32-ppc.lo elf-vxworks.lo elf32.lo $elf" ;;
     powerpc_elf32_vxworks_vec)	 tb="$tb elf32-ppc.lo elf-vxworks.lo elf32.lo $elf" ;;
     powerpc_elf64_vec)		 tb="$tb elf64-ppc.lo elf64-gen.lo elf64.lo $elf"; target_size=64 ;;
     powerpc_elf64_le_vec)	 tb="$tb elf64-ppc.lo elf64-gen.lo elf64.lo $elf" target_size=64 ;;
     powerpc_elf64_fbsd_vec)	 tb="$tb elf64-ppc.lo elf64-gen.lo elf64.lo $elf" target_size=64 ;;
diff --git a/bfd/configure.ac b/bfd/configure.ac
index 015fd01189321b862b5c2b7777344fb20ead67fe..352be794f65cbc75ce625c2a3d9de4cad302e124 100644
--- bfd/configure.ac
+++ bfd/configure.ac
@@ -567,12 +567,13 @@ do
     pdp11_aout_vec)		 tb="$tb pdp11.lo" ;;
     pef_vec)			 tb="$tb pef.lo" ;;
     pef_xlib_vec)		 tb="$tb pef.lo" ;;
     pj_elf32_vec)		 tb="$tb elf32-pj.lo elf32.lo $elf" ;;
     pj_elf32_le_vec)		 tb="$tb elf32-pj.lo elf32.lo $elf" ;;
     powerpc_boot_vec)		 tb="$tb ppcboot.lo" ;;
+    powerpc_elf32_amigaos_vec)	tb="$tb elf32-ppc.lo elf-amigaos.lo elf32.lo $elf" ;;
     powerpc_elf32_vec)		 tb="$tb elf32-ppc.lo elf-vxworks.lo elf32.lo $elf" ;;
     powerpc_elf32_le_vec)	 tb="$tb elf32-ppc.lo elf-vxworks.lo elf32.lo $elf" ;;
     powerpc_elf32_fbsd_vec)	 tb="$tb elf32-ppc.lo elf-vxworks.lo elf32.lo $elf" ;;
     powerpc_elf32_vxworks_vec)	 tb="$tb elf32-ppc.lo elf-vxworks.lo elf32.lo $elf" ;;
     powerpc_elf64_vec)		 tb="$tb elf64-ppc.lo elf64-gen.lo elf64.lo $elf"; target_size=64 ;;
     powerpc_elf64_le_vec)	 tb="$tb elf64-ppc.lo elf64-gen.lo elf64.lo $elf" target_size=64 ;;
diff --git a/bfd/cpu-bfin.c b/bfd/elf-amigaos.c
similarity index 51%
copy from bfd/cpu-bfin.c
copy to bfd/elf-amigaos.c
index 4d3c1804536147ab0365a6f1167c5a87cc8edac8..06771ecc3121b1f96f0a96437dd7dd2bc7e649f9 100644
--- bfd/cpu-bfin.c
+++ bfd/elf-amigaos.c
@@ -1,8 +1,7 @@
-/* BFD Support for the ADI Blackfin processor.
-
+/* VxWorks support for ELF
    Copyright (C) 2005-2023 Free Software Foundation, Inc.
 
    This file is part of BFD, the Binary File Descriptor library.
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
@@ -12,31 +11,32 @@
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
-   along with this program; if not, write to the Free Software
-   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
-   MA 02110-1301, USA.  */
+   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */
+
+/* This file provides routines used by all VxWorks targets.  */
 
 #include "sysdep.h"
 #include "bfd.h"
 #include "libbfd.h"
+#include "elf-bfd.h"
+#include "elf-amigaos.h"
+#include "elf/amigaos.h"
+
+/* Add dynamic tags
+   AmigaOS: Flag it as a version 2 dynamic binary */  
+
+bool
+_bfd_elf_amigaos_add_dynamic_tags (struct bfd_link_info *info)
+{
+#ifdef DEBUG
+		printf ("Target amigaos-pcc needs addtional marker symbol DT_AMIGAOS_DYNVERSION to mark version used.\n"); 
+#endif
 
-const bfd_arch_info_type bfd_bfin_arch =
-  {
-    16,			/* Bits in a word.  */
-    32,			/* Bits in an address.  */
-    8,			/* Bits in a byte.  */
-    bfd_arch_bfin,
-    0,			/* Only one machine.  */
-    "bfin",		/* Arch name.  */
-    "bfin",		/* Arch printable name.  */
-    4,			/* Section align power.  */
-    true,		/* The one and only.  */
-    bfd_default_compatible,
-    bfd_default_scan,
-    bfd_arch_default_fill,
-    NULL,
-    0 /* Maximum offset of a reloc from the start of an insn.  */
-  };
+	struct elf_link_hash_table *htab = elf_hash_table (info);
+	
+	return  htab->target_os != is_amigaos || _bfd_elf_add_dynamic_entry (info,DT_AMIGAOS_DYNVERSION, 2);
+}
+ 
\ No newline at end of file
diff --git a/bfd/elf-nacl.h b/bfd/elf-amigaos.h
similarity index 73%
copy from bfd/elf-nacl.h
copy to bfd/elf-amigaos.h
index e921c64589c111bfebe31f61c0d0a982e6479ba6..f7770b1d669e39f2b9fc139471fb6dfac1c1beb5 100644
--- bfd/elf-nacl.h
+++ bfd/elf-amigaos.h
@@ -1,8 +1,8 @@
-/* Native Client support for ELF
-   Copyright (C) 2012-2023 Free Software Foundation, Inc.
+/* VxWorks support for ELF
+   Copyright (C) 2005-2023 Free Software Foundation, Inc.
 
    This file is part of BFD, the Binary File Descriptor library.
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
@@ -13,9 +13,10 @@
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.  */
 
-bool nacl_modify_segment_map (bfd *, struct bfd_link_info *);
-bool nacl_modify_headers (bfd *, struct bfd_link_info *);
-bool nacl_final_write_processing (bfd *);
+#include "elf/common.h"
+#include "elf/internal.h"
+
+bool _bfd_elf_amigaos_add_dynamic_tags(struct bfd_link_info *);
diff --git a/bfd/elf-bfd.h b/bfd/elf-bfd.h
index 2b7c574f540b55bbc8cccb22f5b60b96f730307d..6ee3ba456a209933b317f6ab726171da3926f7ad 100644
--- bfd/elf-bfd.h
+++ bfd/elf-bfd.h
@@ -588,13 +588,14 @@ struct bfd_link_needed_list
 
 enum elf_target_os
 {
   is_normal,
   is_solaris,	/* Solaris.  */
   is_vxworks,	/* VxWorks.  */
-  is_nacl	/* Native Client.  */
+  is_nacl, 	/* Native Client.  */
+  is_amigaos    /* AmigaOS */
 };
 
 /* Used by bfd_sym_from_r_symndx to cache a small number of local
    symbols.  */
 #define LOCAL_SYM_CACHE_SIZE 32
 struct sym_cache
diff --git a/bfd/elf32-ppc.c b/bfd/elf32-ppc.c
index a8234f27a8a437b2503cd55c971d5d3acbbf6222..2077aaab94e68bb29bce78598ec82c7e38dba2fe 100644
--- bfd/elf32-ppc.c
+++ bfd/elf32-ppc.c
@@ -19,27 +19,31 @@
    Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
    Boston, MA 02110-1301, USA.  */
 
 /* The assembler should generate a full set of section symbols even
    when they appear unused.  The linux kernel build tool recordmcount
    needs them.  */
+// ML: TODO: Keep for Amiga?
 #define TARGET_KEEP_UNUSED_SECTION_SYMBOLS true
 
 #include "sysdep.h"
 #include <stdarg.h>
 #include "bfd.h"
 #include "bfdlink.h"
 #include "libbfd.h"
 #include "elf-bfd.h"
 #include "elf/ppc.h"
+#include "elf/amigaos.h"
 #include "elf32-ppc.h"
+#include "elf-amigaos.h"
 #include "elf-vxworks.h"
 #include "dwarf2.h"
 #include "opcode/ppc.h"
 
 /* All users of this file have bfd_octets_per_byte (abfd, sec) == 1.  */
+// ML: TODO: Keep for Amiga? Fist for amigao
 #define OCTETS_PER_BYTE(ABFD, SEC) 1
 
 typedef enum split16_format_type
 {
   split16a_type = 0,
   split16d_type
@@ -571,12 +575,29 @@ static reloc_howto_type ppc_elf_howto_raw[] = {
   /* Relocation not handled: R_PPC_EMB_RELSEC16 */
   /* Relocation not handled: R_PPC_EMB_RELST_LO */
   /* Relocation not handled: R_PPC_EMB_RELST_HI */
   /* Relocation not handled: R_PPC_EMB_RELST_HA */
   /* Relocation not handled: R_PPC_EMB_BIT_FLD */
 
+  /* A standard 32 bit base relative relocation.  */
+  HOW (R_PPC_AMIGAOS_BREL, 2, 32, 0xffffffff, 0, false, bitfield,
+  	   bfd_elf_generic_reloc),
+
+  /* A 16 bit base relative relocation without overflow.  */
+  HOW (R_PPC_AMIGAOS_BREL_LO, 1, 16, 0xffff, 0, false, dont,
+  	   bfd_elf_generic_reloc),
+
+  /* The high order 16 bits of a base relative address.  */
+  HOW (R_PPC_AMIGAOS_BREL_HI, 1, 16, 0xffff, 0, false, dont,
+  	   bfd_elf_generic_reloc),
+
+  /* The high order 16 bits of a base relative address, plus 1 if the contents
+     of the low 16 bits, treated as a signed number, is negative.  */
+  HOW (R_PPC_AMIGAOS_BREL_HA, 1, 16, 0xffff, 16, false, dont,
+  	   bfd_elf_generic_reloc),
+
   /* PC relative relocation against either _SDA_BASE_ or _SDA2_BASE_, filling
      in the 16 bit signed offset from the appropriate base, and filling in the
      register field with the appropriate register (0, 2, or 13).  */
   HOW (R_PPC_EMB_RELSDA, 2, 16, 0xffff, 0, false, signed,
        ppc_elf_unhandled_reloc),
 
@@ -819,13 +840,17 @@ ppc_elf_reloc_type_lookup (bfd *abfd ATTRIBUTE_UNUSED,
     case BFD_RELOC_PPC_EMB_RELSEC16:	r = R_PPC_EMB_RELSEC16;		break;
     case BFD_RELOC_PPC_EMB_RELST_LO:	r = R_PPC_EMB_RELST_LO;		break;
     case BFD_RELOC_PPC_EMB_RELST_HI:	r = R_PPC_EMB_RELST_HI;		break;
     case BFD_RELOC_PPC_EMB_RELST_HA:	r = R_PPC_EMB_RELST_HA;		break;
     case BFD_RELOC_PPC_EMB_BIT_FLD:	r = R_PPC_EMB_BIT_FLD;		break;
     case BFD_RELOC_PPC_EMB_RELSDA:	r = R_PPC_EMB_RELSDA;		break;
-    case BFD_RELOC_PPC_VLE_REL8:	r = R_PPC_VLE_REL8;		break;
+	case BFD_RELOC_PPC_AMIGAOS_BREL:	r = R_PPC_AMIGAOS_BREL;		break;
+    case BFD_RELOC_PPC_AMIGAOS_BREL_LO:	r = R_PPC_AMIGAOS_BREL_LO;	break;
+    case BFD_RELOC_PPC_AMIGAOS_BREL_HI:	r = R_PPC_AMIGAOS_BREL_HI;	break;
+    case BFD_RELOC_PPC_AMIGAOS_BREL_HA:	r = R_PPC_AMIGAOS_BREL_HA;	break;
+	case BFD_RELOC_PPC_VLE_REL8:	r = R_PPC_VLE_REL8;		break;
     case BFD_RELOC_PPC_VLE_REL15:	r = R_PPC_VLE_REL15;		break;
     case BFD_RELOC_PPC_VLE_REL24:	r = R_PPC_VLE_REL24;		break;
     case BFD_RELOC_PPC_VLE_LO16A:	r = R_PPC_VLE_LO16A;		break;
     case BFD_RELOC_PPC_VLE_LO16D:	r = R_PPC_VLE_LO16D;		break;
     case BFD_RELOC_PPC_VLE_HI16A:	r = R_PPC_VLE_HI16A;		break;
     case BFD_RELOC_PPC_VLE_HI16D:	r = R_PPC_VLE_HI16D;		break;
@@ -2332,13 +2357,22 @@ ppc_elf_create_got (bfd *abfd, struct bfd_link_info *info)
   struct ppc_elf_link_hash_table *htab;
 
   if (!_bfd_elf_create_got_section (abfd, info))
     return false;
 
   htab = ppc_elf_hash_table (info);
-  if (htab->elf.target_os != is_vxworks)
+  if (htab->elf.target_os == is_amigaos )
+	{
+	  /* The powerpc .got has a blrl instruction in it.  Mark it
+	 executable.  */
+      flagword flags = (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS
+			| SEC_IN_MEMORY | SEC_LINKER_CREATED);
+      if (!bfd_set_section_flags (htab->elf.sgot, flags))
+	return false;
+	}
+  else if (htab->elf.target_os != is_vxworks)
     {
       /* The powerpc .got has a blrl instruction in it.  Mark it
 	 executable.  */
       flagword flags = (SEC_ALLOC | SEC_LOAD | SEC_CODE | SEC_HAS_CONTENTS
 			| SEC_IN_MEMORY | SEC_LINKER_CREATED);
       if (!bfd_set_section_flags (htab->elf.sgot, flags))
@@ -2498,12 +2532,14 @@ ppc_elf_create_dynamic_sections (bfd *abfd, struct bfd_link_info *info)
   if (htab->elf.target_os == is_vxworks
       && !elf_vxworks_create_dynamic_sections (abfd, info, &htab->srelplt2))
     return false;
 
   s = htab->elf.splt;
   flags = SEC_ALLOC | SEC_CODE | SEC_LINKER_CREATED;
+  if (htab->elf.target_os == is_amigaos )
+     flags |= SEC_READONLY;
   if (htab->plt_type == PLT_VXWORKS)
     /* The VxWorks PLT is a loaded section with contents.  */
     flags |= SEC_HAS_CONTENTS | SEC_LOAD | SEC_READONLY;
   return bfd_set_section_flags (s, flags);
 }
 
@@ -3153,12 +3189,19 @@ ppc_elf_check_relocs (bfd *abfd,
 	    {
 	      ppc_elf_hash_entry (h)->has_sda_refs = true;
 	      h->non_got_ref = true;
 	    }
 	  break;
 
+	  /* These don't work with a GOT */
+	case R_PPC_AMIGAOS_BREL:
+	case R_PPC_AMIGAOS_BREL_HI:
+	case R_PPC_AMIGAOS_BREL_LO:
+	case R_PPC_AMIGAOS_BREL_HA:
+	  break;
+
 	case R_PPC_VLE_REL8:
 	case R_PPC_VLE_REL15:
 	case R_PPC_VLE_REL24:
 	case R_PPC_VLE_LO16A:
 	case R_PPC_VLE_LO16D:
 	case R_PPC_VLE_HI16A:
@@ -4907,12 +4950,13 @@ ppc_elf_adjust_dynamic_symbol (struct bfd_link_info *info,
       doesn't work on VxWorks, where we can not have dynamic
       relocations (other than copy and jump slot relocations) in an
       executable.  */
   if (ELIMINATE_COPY_RELOCS
       && !ppc_elf_hash_entry (h)->has_sda_refs
       && htab->elf.target_os != is_vxworks
+	  && htab->elf.target_os != is_amigaos
       && !h->def_regular
       && !alias_readonly_dynrelocs (h))
     return true;
 
   /* We must allocate the symbol in our .dynbss section, which will
      become part of the .bss section of the executable.  There will be
@@ -5894,12 +5938,15 @@ ppc_elf_size_dynamic_sections (bfd *output_bfd,
   _bfd_elf_add_dynamic_entry (info, TAG, VAL)
 
       if (!_bfd_elf_maybe_vxworks_add_dynamic_tags (output_bfd, info,
 						    relocs))
 	return false;
 
+      if (!_bfd_elf_amigaos_add_dynamic_tags (info))
+		return false;
+
       if (htab->plt_type == PLT_NEW
 	  && htab->glink != NULL
 	  && htab->glink->size != 0)
 	{
 	  if (!add_dynamic_entry (DT_PPC_GOT, 0))
 	    return false;
@@ -6977,12 +7024,13 @@ ppc_elf_relocate_section (bfd *output_bfd,
   struct ppc_elf_link_hash_table *htab;
   Elf_Internal_Rela *rel;
   Elf_Internal_Rela *wrel;
   Elf_Internal_Rela *relend;
   Elf_Internal_Rela outrel;
   asection *got2;
+  asection *data_section = NULL;
   bfd_vma *local_got_offsets;
   bool ret = true;
   bfd_vma d_offset = (bfd_big_endian (input_bfd) ? 2 : 0);
   bool is_vxworks_tls;
   unsigned int picfixup_size = 0;
   struct ppc_elf_relax_info *relax_info = NULL;
@@ -8048,12 +8096,57 @@ ppc_elf_relocate_section (bfd *output_bfd,
 	case R_PPC_ADDR16_HI:
 	case R_PPC_ADDR16_HA:
 	case R_PPC_UADDR32:
 	case R_PPC_UADDR16:
 	  goto dodyn;
 
+	case R_PPC_AMIGAOS_BREL:
+	case R_PPC_AMIGAOS_BREL_HI:
+	case R_PPC_AMIGAOS_BREL_LO:
+	case R_PPC_AMIGAOS_BREL_HA:
+	{
+		if (data_section == NULL)
+			data_section = bfd_get_section_by_name (output_bfd, ".data");
+		if (data_section != NULL)
+		{
+			if (sec)
+			{
+				const char *name = bfd_section_name (sec->output_section);
+				if (strcmp (name, ".sdata") != 0
+					&& strcmp (name, ".sbss") != 0
+					&& strcmp (name, ".data") != 0
+					&& strcmp (name, ".bss") != 0
+					&& strncmp (name, ".ctors", 6) != 0
+					&& strncmp (name, ".dtors", 6) != 0)
+				{
+					_bfd_error_handler
+						/* xgettext:c-format */
+						(_("%pB: the target (%s) of a %s relocation is in the wrong output section (%s)"),
+							input_bfd,
+							sym_name,
+							howto->name,
+							name);
+				}			
+			}
+
+			addend -= data_section->output_section->vma;
+
+			if (r_type == R_PPC_AMIGAOS_BREL_HA)
+			addend += ((relocation + addend) & 0x8000) << 1;
+		}
+		else 
+		{
+			_bfd_error_handler
+				/* xgettext:c-format */
+				(_("%pB: the target (%s) has not '.data' section"),
+					input_bfd,
+					sym_name);
+		}
+	}
+	break;
+
 	case R_PPC_VLE_REL8:
 	case R_PPC_VLE_REL15:
 	case R_PPC_VLE_REL24:
 	case R_PPC_REL24:
 	case R_PPC_REL14:
 	case R_PPC_REL14_BRTAKEN:
@@ -10433,12 +10526,162 @@ ppc_elf_finish_dynamic_sections (bfd *output_bfd,
 #define elf_backend_action_discarded		ppc_elf_action_discarded
 #define elf_backend_init_index_section		_bfd_elf_init_1_index_section
 #define elf_backend_lookup_section_flags_hook	ppc_elf_lookup_section_flags
 
 #include "elf32-target.h"
 
+/* ML: TODO: AmigaOS Target */
+
+#undef  TARGET_LITTLE_SYM
+#undef  TARGET_LITTLE_NAME
+
+#undef  TARGET_BIG_SYM
+#define TARGET_BIG_SYM  powerpc_elf32_amigaos_vec
+#undef  TARGET_BIG_NAME
+#define TARGET_BIG_NAME "elf32-powerpc-amigaos"
+
+#undef ELF_TARGET_OS
+#define ELF_TARGET_OS		is_amigaos
+
+/* The name of the readonly data section.  */
+#define RDATA_SECTION_NAME ".rodata"
+
+/* If we have .rodata section we need to bump the
+programm headers, so that it is in it own segment. */ 
+
+
+static int
+ppc_elf_amigaos_additional_program_headers (
+	bfd *abfd,
+	struct bfd_link_info *info ATTRIBUTE_UNUSED)
+{
+	int ret = ppc_elf_additional_program_headers(abfd,info);
+
+	/* See if we need a RDATA_SECTION_NAME segment.  */
+	if (bfd_get_section_by_name (abfd, RDATA_SECTION_NAME))
+	{
+#ifdef DEBUG
+		printf ("Target amigaos-pcc needs addtional programm header, because .rodata section is present, thus we add 1 to %d\n",ret); 
+#endif
+		++ret;
+	}
+
+	return ret;
+}
+
+static bool
+ppc_elf_amigaos_modify_segment_map (
+	bfd *abfd,
+	struct bfd_link_info *info ATTRIBUTE_UNUSED)
+{
+	/* If there is a .rodata section, we need a own segment for it.  */
+	asection *roSection = bfd_get_section_by_name (abfd, RDATA_SECTION_NAME);
+	if( roSection != NULL ) 
+	{
+#ifdef DEBUG
+		printf ("Target amigaos-pcc needs .rodata section in aseparate segment from .text and .plt\n"); 
+#endif
+		for( struct elf_segment_map *segment = elf_seg_map (abfd);segment != NULL;segment = segment->next ) 
+		{
+			if( segment->p_type == PT_LOAD && segment->count > 1 )
+			{
+				for( unsigned int index = 0;index < segment->count;index++ )
+				{
+					if( segment->sections[index] == roSection ) 
+					{
+#ifdef DEBUG
+						printf ("Segment found for .rodata at index %d of %d sections\n",index,segment->count); 
+#endif
+				
+						if( index + 1 < segment->count )
+						{
+							struct elf_segment_map *nextSegment = bfd_zalloc (abfd,sizeof (struct elf_segment_map) + ( (segment->count - ( index + 2 )) * sizeof ( segment->sections[0]) ) );
+							if( nextSegment == NULL ) 
+								return false;
+						
+							nextSegment->count = segment->count - (index + 1);
+							memcpy (nextSegment->sections, segment->sections + index + 1,nextSegment->count * sizeof (segment->sections[0]));
+							nextSegment->p_type = PT_LOAD;
+							nextSegment->p_flags = PF_R;
+							nextSegment->next = segment->next;
+							segment->next = nextSegment;
+						}
+						
+						segment->count = 1;
+
+						if( index != 0 )
+						{
+							segment->count = index;
+							struct elf_segment_map *nextSegment = bfd_zalloc (abfd,sizeof (struct elf_segment_map));
+							if( nextSegment == NULL )
+								return false;
+
+							nextSegment->p_type = PT_LOAD;
+							nextSegment->p_flags = PF_R;
+							nextSegment->count = 1;
+							nextSegment->sections[0] = roSection;
+							nextSegment->next = segment->next;
+							segment->next = nextSegment;
+						}
+
+						break;
+					} 
+				}		
+			}
+		}
+	}
+
+	return ppc_elf_modify_segment_map( abfd,info );
+}
+
+static bool
+ppc_elf_amigaos_finish_dynamic_symbol(
+	bfd *output_bfd,
+	struct bfd_link_info *info,
+	struct elf_link_hash_entry *hashEntry,
+	Elf_Internal_Sym *sym)
+{
+#ifdef DEBUG
+	printf ("Target amigaos-pcc needs reloc R_PPC_JMP_SLOT/... having none zero value\n"); 
+#endif
+ 
+	if( ! hashEntry->def_regular || ( hashEntry->type == STT_GNU_IFUNC && !bfd_link_pic( info ) ) )
+	{
+		for( struct plt_entry *pltEntry = hashEntry->plt.plist;pltEntry != NULL;pltEntry = pltEntry->next )
+		{
+			if( pltEntry->plt.offset != (bfd_vma)-1 ) 
+			{	
+				if( ! hashEntry->def_regular && ! hashEntry->pointer_equality_needed )
+	    		{
+					/* THF: This is peculiar. The compiler generates a R_PPC_JMP_SLOT for externally referenced
+					 * symbols imported from libc.so. Relocation in elf.library requires the symbol to have it's .plt
+					 * stub value, but the linker specifically clears the value to 0, resulting in run-time
+					 * errors when the binary tries to call libc functions.
+					 */	
+					hashEntry->pointer_equality_needed = 1;
+				}
+			}
+		}
+	}
+
+  return ppc_elf_finish_dynamic_symbol( output_bfd,info,hashEntry,sym );
+}
+#undef elf_backend_additional_program_headers
+#define elf_backend_additional_program_headers	ppc_elf_amigaos_additional_program_headers
+
+#undef elf_backend_modify_segment_map
+#define elf_backend_modify_segment_map			ppc_elf_amigaos_modify_segment_map
+
+#undef elf_backend_finish_dynamic_symbol
+#define elf_backend_finish_dynamic_symbol	ppc_elf_amigaos_finish_dynamic_symbol
+
+#undef elf32_bed
+#define elf32_bed	elf32_powerpc_amigaos_bed
+
+#include "elf32-target.h"
+
 /* FreeBSD Target */
 
 #undef  TARGET_LITTLE_SYM
 #undef  TARGET_LITTLE_NAME
 
 #undef  TARGET_BIG_SYM
diff --git a/bfd/elflink.c b/bfd/elflink.c
index 7bf337c7d449b8e1f690b5adccf42d590e15b807..370f4173b22857ab84d06b8ec85311c9b1a5225f 100644
--- bfd/elflink.c
+++ bfd/elflink.c
@@ -10530,12 +10530,14 @@ elf_link_output_extsym (struct bfd_hash_entry *bh, void *data)
   sym.st_other = h->other;
   switch (h->root.type)
     {
     default:
     case bfd_link_hash_new:
     case bfd_link_hash_warning:
+	  // ML: TODO: really needed, or just  cosmetic?
+	  (*_bfd_error_handler)(_("Unexpected type (%d) of symbol %s"), h->root.type, h->root.root.string);
       abort ();
       return false;
 
     case bfd_link_hash_undefined:
     case bfd_link_hash_undefweak:
       input_sec = bfd_und_section_ptr;
diff --git a/bfd/libbfd.h b/bfd/libbfd.h
index e75935133acd6e9e5000177bff0eaba3e2f17d78..a406a9dc00d31449ceef3de062655720846d2b06 100644
--- bfd/libbfd.h
+++ bfd/libbfd.h
@@ -1648,12 +1648,16 @@ static const char *const bfd_reloc_code_real_names[] = { "@@uninitialized@@",
   "BFD_RELOC_PPC64_DTPREL34",
   "BFD_RELOC_PPC64_GOT_TLSGD_PCREL34",
   "BFD_RELOC_PPC64_GOT_TLSLD_PCREL34",
   "BFD_RELOC_PPC64_GOT_TPREL_PCREL34",
   "BFD_RELOC_PPC64_GOT_DTPREL_PCREL34",
   "BFD_RELOC_PPC64_TLS_PCREL",
+  "BFD_RELOC_PPC_AMIGAOS_BREL",
+  "BFD_RELOC_PPC_AMIGAOS_BREL_LO",
+  "BFD_RELOC_PPC_AMIGAOS_BREL_HI",
+  "BFD_RELOC_PPC_AMIGAOS_BREL_HA",
   "BFD_RELOC_I370_D12",
   "BFD_RELOC_CTOR",
   "BFD_RELOC_ARM_PCREL_BRANCH",
   "BFD_RELOC_ARM_PCREL_BLX",
   "BFD_RELOC_THUMB_PCREL_BLX",
   "BFD_RELOC_ARM_PCREL_CALL",
diff --git a/bfd/po/SRC-POTFILES.in b/bfd/po/SRC-POTFILES.in
index 08e2901d3baf74b92a48cbcc705c4b13701fec63..c4ff7a793ecbebc8fd5e0124c1eb75e4a6a67678 100644
--- bfd/po/SRC-POTFILES.in
+++ bfd/po/SRC-POTFILES.in
@@ -145,12 +145,14 @@ elf-m10300.c
 elf-nacl.c
 elf-nacl.h
 elf-properties.c
 elf-s390.h
 elf-sframe.c
 elf-strtab.c
+elf-amigaos.c
+elf-amigaos.h
 elf-vxworks.c
 elf-vxworks.h
 elf.c
 elf32-am33lin.c
 elf32-arc.c
 elf32-arm.c
diff --git a/bfd/reloc.c b/bfd/reloc.c
index db4f30d36d0065a670df387215d1329fcbaa9039..0f4faa0c5446c295143360e09d023e6d0c05d253 100644
--- bfd/reloc.c
+++ bfd/reloc.c
@@ -3048,15 +3048,21 @@ ENUMX
 ENUMX
   BFD_RELOC_PPC64_TLS_PCREL
 ENUMDOC
   PowerPC and PowerPC64 thread-local storage relocations.
 
 ENUM
-  BFD_RELOC_I370_D12
+  BFD_RELOC_PPC_AMIGAOS_BREL
+ENUMX  
+  BFD_RELOC_PPC_AMIGAOS_BREL_LO
+ENUMX  
+  BFD_RELOC_PPC_AMIGAOS_BREL_HI
+ENUMX
+  BFD_RELOC_PPC_AMIGAOS_BREL_HA
 ENUMDOC
-  IBM 370/390 relocations
+  AmigaOS PowerPC r2 base relative addressing into data section.
 
 ENUM
   BFD_RELOC_CTOR
 ENUMDOC
   The type of reloc used to build a constructor table - at the moment
   probably a 32 bit wide absolute relocation, but the target can choose.
diff --git a/bfd/targets.c b/bfd/targets.c
index 41294ea4d141c44ff58bac3fb1ce3316c8cde63f..f09f2c337a7dc62f5145e14c01182cd97a737ba2 100644
--- bfd/targets.c
+++ bfd/targets.c
@@ -847,12 +847,13 @@ extern const bfd_target pdp11_aout_vec;
 extern const bfd_target pef_vec;
 extern const bfd_target pef_xlib_vec;
 extern const bfd_target pj_elf32_vec;
 extern const bfd_target pj_elf32_le_vec;
 extern const bfd_target plugin_vec;
 extern const bfd_target powerpc_boot_vec;
+extern const bfd_target powerpc_elf32_amigaos_vec;
 extern const bfd_target powerpc_elf32_vec;
 extern const bfd_target powerpc_elf32_le_vec;
 extern const bfd_target powerpc_elf32_fbsd_vec;
 extern const bfd_target powerpc_elf32_vxworks_vec;
 extern const bfd_target powerpc_elf64_vec;
 extern const bfd_target powerpc_elf64_le_vec;
@@ -1233,12 +1234,13 @@ static const bfd_target * const _bfd_target_vector[] =
 	&pef_xlib_vec,
 
 	&pj_elf32_vec,
 	&pj_elf32_le_vec,
 
 	&powerpc_boot_vec,
+	&powerpc_elf32_amigaos_vec,
 	&powerpc_elf32_vec,
 	&powerpc_elf32_le_vec,
 	&powerpc_elf32_fbsd_vec,
 	&powerpc_elf32_vxworks_vec,
 #ifdef BFD64
 	&powerpc_elf64_vec,
diff --git a/binutils/elfcomm.c b/binutils/elfcomm.c
index 71b595a68a9296fdfbe3fb794f4baa8324ea2a4a..b8c0afa8e0d1914a390ef5d0539549663da0fc26 100644
--- binutils/elfcomm.c
+++ binutils/elfcomm.c
@@ -32,12 +32,24 @@
 #include "aout/ar.h"
 #include "elfcomm.h"
 #include <assert.h>
 
 extern char *program_name;
 
+/* Restore commit 546cb2d85eddba4f56dfbcb0288db68243e3a0fd for AmigaOS4 with clib clib4 */
+#if defined(__amigaos4__) && defined(__CLIB4__)
+/* FIXME:  This definition really ought to be in ansidecl.h.  */
+#ifndef ATTRIBUTE_WEAK
+#define ATTRIBUTE_WEAK __attribute__((weak))
+#endif
+
+/* Allow the following two functions to be overridden if desired.  */
+void error (const char *, ...) ATTRIBUTE_WEAK;
+void warn (const char *, ...) ATTRIBUTE_WEAK;
+#endif
+
 void
 error (const char *message, ...)
 {
   va_list args;
 
   /* Try to keep error messages in sync with the program's normal output.  */
diff --git a/binutils/objcopy.c b/binutils/objcopy.c
index a6182b48b6c0ebc293ef57f306fa71231f3efbfa..4e85ed7966a56fb86a306c535a051d19166019bb 100644
--- binutils/objcopy.c
+++ binutils/objcopy.c
@@ -119,12 +119,15 @@ enum strip_action
   STRIP_ALL		/* Strip all symbols.  */
 };
 
 /* Which symbols to remove.  */
 static enum strip_action strip_symbols = STRIP_UNDEF;
 
+/* Shall we strip unneeded relative relocs? */
+static int strip_unneeded_rel_relocs;
+
 enum locals_action
 {
   LOCALS_UNDEF,
   LOCALS_START_L,	/* Discard locals starting with L.  */
   LOCALS_ALL		/* Discard all locals.  */
 };
@@ -363,12 +366,13 @@ enum command_line_switch
   OPTION_SET_SECTION_ALIGNMENT,
   OPTION_SET_START,
   OPTION_SREC_FORCES3,
   OPTION_SREC_LEN,
   OPTION_STACK,
   OPTION_STRIP_DWO,
+  OPTION_STRIP_UNNEEED_REL_RELOCS,
   OPTION_STRIP_SYMBOLS,
   OPTION_STRIP_UNNEEDED,
   OPTION_STRIP_UNNEEDED_SYMBOL,
   OPTION_STRIP_UNNEEDED_SYMBOLS,
   OPTION_SUBSYSTEM,
   OPTION_UPDATE_SECTION,
@@ -406,12 +410,13 @@ static struct option strip_options[] =
   {"remove-relocations", required_argument, 0, OPTION_REMOVE_RELOCS},
   {"strip-all", no_argument, 0, 's'},
   {"strip-debug", no_argument, 0, 'S'},
   {"strip-dwo", no_argument, 0, OPTION_STRIP_DWO},
   {"strip-symbol", required_argument, 0, 'N'},
   {"strip-unneeded", no_argument, 0, OPTION_STRIP_UNNEEDED},
+  {"strip-unneeded-rel-relocs", no_argument, 0, OPTION_STRIP_UNNEEED_REL_RELOCS},
   {"target", required_argument, 0, 'F'},
   {"verbose", no_argument, 0, 'v'},
   {"version", no_argument, 0, 'V'},
   {"wildcard", no_argument, 0, 'w'},
   {0, no_argument, 0, 0}
 };
@@ -1559,12 +1564,17 @@ filter_symbols (bfd *abfd, bfd *obfd, asymbol **osyms,
       bool undefined;
       bool rem_leading_char;
       bool add_leading_char;
 
       undefined = bfd_is_und_section (bfd_asymbol_section (sym));
 
+	  if (strip_symbols == STRIP_ALL && undefined)
+        {
+          add_specific_symbol(name, keep_specific_htab);
+        }
+
       if (add_sym_list)
 	{
 	  struct addsym_node *ptr;
 
 	  if (need_sym_before (&ptr, name))
 	    to[dst_count++] = create_new_symbol (ptr, obfd);
@@ -1642,16 +1652,29 @@ filter_symbols (bfd *abfd, bfd *obfd, asymbol **osyms,
 	    }
 
 	  strcpy (ptr, name);
 	  bfd_set_asymbol_name (sym, n);
 	  name = n;
 	}
-
-      if (strip_symbols == STRIP_ALL)
-	keep = false;
-      else if ((flags & BSF_KEEP) != 0		/* Used in relocation.  */
+      if(strip_symbols == STRIP_ALL) 
+	  { 		
+		  keep = false;
+		 /* Never, ever, strip everthing on the Amiga, keep global symbols, needed by OS 
+		 	_start: 		Entry point of executable, isn't fixed on ppc-amigaos, so OS needs to knwo where to enter
+			__amigaos4__: 	Maker symbol to identify that ELF file is for ppc-amigaos, because no offcial value has been assigned to ELF heder field OS/ABI for ppc-amigaos
+			_SDA_BASE_:		If small data model ist used, the symbol is needed ....?????
+		 */
+     	 if( (bfd_get_flavour(obfd) == bfd_target_elf_flavour && get_elf_backend_data(obfd)->target_os == is_amigaos))
+		 {
+			//  ML: TODO: _SDA_BASE_ onyl needs to be kept if small data section are present ????
+			if (strcmp(name, "_start") == 0 || strcmp(name, "__amigaos4__") == 0 || strcmp(name, "_SDA_BASE_") == 0) {
+			  keep = true;
+			}
+		 }
+	  }
+	  else if ((flags & BSF_KEEP) != 0		/* Used in relocation.  */
 	       || ((flags & BSF_SECTION_SYM) != 0
 		   && ((*bfd_asymbol_section (sym)->symbol_ptr_ptr)->flags
 		       & BSF_KEEP) != 0))
 	{
 	  keep = true;
 	  used_in_reloc = true;
@@ -1706,13 +1729,23 @@ filter_symbols (bfd *abfd, bfd *obfd, asymbol **osyms,
       if (!keep
 	  && ((keep_file_symbols && (flags & BSF_FILE))
 	      || is_specified_symbol (name, keep_specific_htab)))
 	keep = true;
 
       if (keep && is_strip_section (abfd, bfd_asymbol_section (sym)))
-	keep = false;
+	{
+          /* If the symbol refers to a stripped section, we still want to
+           * keep it, e.g., _SDA_BASE_ TODO: We should perhaps output a
+           * warning or add another option to trigger this behaviour.
+           * FIXME: The section to which symbol refers must be adjusted
+           * as well */
+          if (!is_specified_symbol (name, keep_specific_htab))
+            {
+              keep = false;
+            }
+	}
 
       if (keep)
 	{
 	  if (((flags & (BSF_GLOBAL | BSF_GNU_UNIQUE))
 	       || undefined)
 	      && (weaken || is_specified_symbol (name, weaken_specific_htab)))
@@ -3288,12 +3321,13 @@ copy_object (bfd *ibfd, bfd *obfd, const bfd_arch_info_type *input_arch)
 
 	 Note we iterate over the input sections examining their
 	 relocations since the relocations for the output sections
 	 haven't been set yet.  mark_symbols_used_in_relocations will
 	 ignore input sections which have no corresponding output
 	 section.  */
+	 // ML: TODO: Really needef for maiga to remove the floowing  fi???
       if (strip_symbols != STRIP_ALL)
 	{
 	  bfd_set_error (bfd_error_no_error);
 	  bfd_map_over_sections (ibfd,
 				 mark_symbols_used_in_relocations,
 				 isympp);
@@ -4371,27 +4405,40 @@ copy_relocations_in_section (bfd *ibfd, sec_ptr isection, void *obfdarg)
 	      bfd_nonfatal_message (NULL, ibfd, isection,
 				    _("relocation count is negative"));
 	      return;
 	    }
 	}
 
-      if (strip_symbols == STRIP_ALL)
-	{
+    if (strip_symbols == STRIP_ALL )
+  {
 	  /* Remove relocations which are not in
 	     keep_strip_specific_list.  */
 	  arelent **w_relpp;
 	  long i;
 
 	  for (w_relpp = relpp, i = 0; i < relcount; i++)
 	    /* PR 17512: file: 9e907e0c.  */
 	    if (relpp[i]->sym_ptr_ptr
 		/* PR 20096 */
-		&& *relpp[i]->sym_ptr_ptr
-		&& is_specified_symbol (bfd_asymbol_name (*relpp[i]->sym_ptr_ptr),
+		&& *relpp[i]->sym_ptr_ptr ) {
+		asection *sec = (*(relpp[i]->sym_ptr_ptr))->section;
+
+		if( is_specified_symbol (bfd_asymbol_name (*relpp[i]->sym_ptr_ptr),
 					keep_specific_htab))
 	      *w_relpp++ = relpp[i];
+		/* Never, ever, strip all? reloc data on the Amiga! */
+		else if( bfd_get_flavour(obfd) == bfd_target_elf_flavour && get_elf_backend_data(obfd)->target_os == is_amigaos)
+		{
+			if (!strip_unneeded_rel_relocs || !relpp [i]->howto->pc_relative || sec->index != osection->index)
+			{
+				relpp[i]->addend = bfd_asymbol_value(*relpp [i]->sym_ptr_ptr) - sec->vma + relpp[i]->addend;
+				relpp[i]->sym_ptr_ptr = sec->symbol_ptr_ptr;
+				*w_relpp++ = relpp[i];
+			}
+		}  
+		}
 	  relcount = w_relpp - relpp;
 	  *w_relpp = 0;
 	}
 
       bfd_set_reloc (obfd, osection, relcount == 0 ? NULL : relpp, relcount);
     }
@@ -4747,12 +4794,15 @@ strip_main (int argc, char *argv[])
 	case OPTION_STRIP_DWO:
 	  strip_symbols = STRIP_DWO;
 	  break;
 	case OPTION_STRIP_UNNEEDED:
 	  strip_symbols = STRIP_UNNEEDED;
 	  break;
+	case OPTION_STRIP_UNNEEED_REL_RELOCS:
+	  strip_unneeded_rel_relocs = 1;
+	  break;	  
 	case 'K':
 	  add_specific_symbol (optarg, keep_specific_htab);
 	  break;
 	case 'M':
 	  merge_notes = true;
 	  merge_notes_set = true;
@@ -4832,12 +4882,17 @@ strip_main (int argc, char *argv[])
 
   if (show_version)
     print_version ("strip");
 
   default_deterministic ();
 
+  // ML: TODO: For amiga
+  add_specific_symbol("__amigaos4__", keep_specific_htab);
+  add_specific_symbol("_start", keep_specific_htab);
+  add_specific_symbol("_SDA_BASE_", keep_specific_htab);
+
   /* Default is to strip all symbols.  */
   if (strip_symbols == STRIP_UNDEF
       && discard_locals == LOCALS_UNDEF
       && htab_elements (strip_specific_htab) == 0)
     strip_symbols = STRIP_ALL;
 
@@ -5906,12 +5961,18 @@ copy_main (int argc, char *argv[])
   if (show_version)
     print_version ("objcopy");
 
   if (interleave && copy_byte == -1)
     fatal (_("interleave start byte must be set with --byte"));
 
+  // ML: TOOD: For akigaSO
+  add_specific_symbol("__amigappc__", keep_specific_htab);
+  add_specific_symbol("__amigaos4__", keep_specific_htab);
+  add_specific_symbol("_start", keep_specific_htab);
+  add_specific_symbol("_SDA_BASE_", keep_specific_htab);
+
   if (copy_byte >= interleave)
     fatal (_("byte number must be less than interleave"));
 
   if (copy_width > interleave - copy_byte)
     fatal (_("interleave width must be less than or equal to interleave - byte`"));
 
diff --git a/binutils/readelf.c b/binutils/readelf.c
index 3da3db159ccd2ca8bcc22e9b644447c106b7c110..0439e7af16bd78c35d57187d4af414df3e3404ce 100644
--- binutils/readelf.c
+++ binutils/readelf.c
@@ -94,12 +94,13 @@
 
 #define RELOC_MACROS_GEN_FUNC
 
 #include "elf/aarch64.h"
 #include "elf/alpha.h"
 #include "elf/amdgpu.h"
+#include "elf/amigaos.h"
 #include "elf/arc.h"
 #include "elf/arm.h"
 #include "elf/avr.h"
 #include "elf/bfin.h"
 #include "elf/cr16.h"
 #include "elf/cris.h"
@@ -2220,12 +2221,13 @@ static const char *
 get_ppc_dynamic_type (unsigned long type)
 {
   switch (type)
     {
     case DT_PPC_GOT:    return "PPC_GOT";
     case DT_PPC_OPT:    return "PPC_OPT";
+	case DT_AMIGAOS_DYNVERSION: return "AMIGAOS_DYNVERSION";
     default:
       return NULL;
     }
 }
 
 static const char *
@@ -2527,12 +2529,14 @@ get_dynamic_type (Filedata * filedata, unsigned long type)
     case DT_GNU_CONFLICTSZ: return "GNU_CONFLICTSZ";
     case DT_GNU_LIBLIST: return "GNU_LIBLIST";
     case DT_GNU_LIBLISTSZ: return "GNU_LIBLISTSZ";
     case DT_GNU_HASH:	return "GNU_HASH";
     case DT_GNU_FLAGS_1: return "GNU_FLAGS_1";
 
+	case DT_AMIGAOS_DYNVERSION: return get_ppc_dynamic_type (type);
+
     default:
       if ((type >= DT_LOPROC) && (type <= DT_HIPROC))
 	{
 	  const char * result;
 
 	  switch (filedata->file_header.e_machine)
diff --git a/binutils/version.c b/binutils/version.c
index 08035359ad7fca65322249a37bbf20992eaa7ec3..5752fb4cb0a31c6448c7e5d57d039ec3f00330cf 100644
--- binutils/version.c
+++ binutils/version.c
@@ -33,8 +33,21 @@ print_version (const char *name)
   printf ("GNU %s %s\n", name, BFD_VERSION_STRING);
   printf (_("Copyright (C) 2023 Free Software Foundation, Inc.\n"));
   printf (_("\
 This program is free software; you may redistribute it under the terms of\n\
 the GNU General Public License version 3 or (at your option) any later version.\n\
 This program has absolutely no warranty.\n"));
+
+#if defined(__amigaos4__)
+# if defined( __NEWLIB__)
+  printf (_("AmigaOS native (ppc-amigaos,newlib).\n"));
+# elif defined( __CLIB4__)
+  printf (_("AmigaOS native (ppc-amigaos,clib4).\n"));
+# elif defined( __CLIB2__ )
+  printf (_("AmigaOS native (ppc-amigaos,clib2).\n"));
+# else
+ printf (_("AmigaOS native (ppc-amigaos,unknown).\n"));
+# endif
+#endif
+
   exit (0);
 }
diff --git a/config.sub b/config.sub
index dba16e84c77c7d25871d80c24deff717faf4c094..3913e5de7c182cd77942ba69027b37f774f104fc 100755
--- config.sub
+++ config.sub
@@ -9,13 +9,13 @@ timestamp='2022-01-03'
 # This file is free software; you can redistribute it and/or modify it
 # under the terms of the GNU General Public License as published by
 # the Free Software Foundation, either version 3 of the License, or
 # (at your option) any later version.
 #
 # This program is distributed in the hope that it will be useful, but
-# WITHOUT ANY WARRANTY; without even the implied warranty of
+# WITHOUT ANY WARRANTY; without even the implied warranty amigaunix
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 # General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License
 # along with this program; if not, see <https://www.gnu.org/licenses/>.
 #
@@ -228,18 +228,14 @@ case $1 in
 				basic_os=bsd
 				;;
 			amdahl)
 				basic_machine=580-amdahl
 				basic_os=sysv
 				;;
-			amiga)
-				basic_machine=m68k-unknown
-				basic_os=
-				;;
-			amigaos | amigados)
-				basic_machine=m68k-unknown
+			amigaos | amigados | amiga)
+				basic_machine=powerpc-unknown
 				basic_os=amigaos
 				;;
 			amigaunix | amix)
 				basic_machine=m68k-unknown
 				basic_os=sysv4
 				;;
diff --git a/elfcpp/powerpc.h b/elfcpp/powerpc.h
index 9322061daed24fd9e8942129322804ad3282e77b..375468d984fbcc86d2246b2e569564b89cc34f10 100644
--- elfcpp/powerpc.h
+++ elfcpp/powerpc.h
@@ -208,12 +208,18 @@ enum
   R_PPC64_DTPREL34 = 147,
   R_PPC64_GOT_TLSGD_PCREL34 = 148,
   R_PPC64_GOT_TLSLD_PCREL34 = 149,
   R_PPC64_GOT_TPREL_PCREL34 = 150,
   R_PPC64_GOT_DTPREL_PCREL34 = 151,
 
+  /* ML: TODO: AmigaOS ELF base relative addressing data secion via r2 relocation (compile option -mbaserel)*/
+  R_PPC_AMIGAOS_BREL = 210,
+  R_PPC_AMIGAOS_BREL_LO = 211,
+  R_PPC_AMIGAOS_BREL_HI = 212,
+  R_PPC_AMIGAOS_BREL_HA = 213,
+  
   R_PPC_VLE_REL8 = 216,
   R_PPC_VLE_REL15 = 217,
   R_PPC_VLE_REL24 = 218,
   R_PPC_VLE_LO16A = 219,
   R_PPC_VLE_LO16D = 220,
   R_PPC_VLE_HI16A = 221,
diff --git a/gas/Makefile.am b/gas/Makefile.am
index ba2896581b75ea89f9a53200d636cdbb9910b751..b6fbe64a10c26c229afc65cad6ae48b84c7f37e5 100644
--- gas/Makefile.am
+++ gas/Makefile.am
@@ -314,12 +314,13 @@ OBJ_FORMAT_HFILES = \
 
 # Emulation header files in config
 
 TARG_ENV_HFILES = \
 	config/te-386bsd.h \
 	config/te-aix5.h \
+	config/te-amigaos.h \
 	config/te-armeabi.h \
 	config/te-armfbsdeabi.h \
 	config/te-armfbsdvfp.h \
 	config/te-armlinuxeabi.h \
 	config/te-csky_abiv1.h \
 	config/te-csky_abiv1_linux.h \
diff --git a/gas/Makefile.in b/gas/Makefile.in
index 8319181b47278770c2449c23b5dfac043a4ead5d..743c546418da854330df420ed153a67b5d3f8e7c 100644
--- gas/Makefile.in
+++ gas/Makefile.in
@@ -801,12 +801,13 @@ OBJ_FORMAT_HFILES = \
 
 
 # Emulation header files in config
 TARG_ENV_HFILES = \
 	config/te-386bsd.h \
 	config/te-aix5.h \
+	config/te-amigaos.h \
 	config/te-armeabi.h \
 	config/te-armfbsdeabi.h \
 	config/te-armfbsdvfp.h \
 	config/te-armlinuxeabi.h \
 	config/te-csky_abiv1.h \
 	config/te-csky_abiv1_linux.h \
diff --git a/gas/config/tc-ppc.c b/gas/config/tc-ppc.c
index 9450fa74de1b61542c9a18babf8c8f621ef904fb..cb5da74ab478d0da04fa562f4a96a351e862c35a 100644
--- gas/config/tc-ppc.c
+++ gas/config/tc-ppc.c
@@ -1544,20 +1544,24 @@ ppc_target_format (void)
 #  else
   return (ppc_obj64 ? "aixcoff64-rs6000" : "aixcoff-rs6000");
 #  endif
 #endif
 #endif
 #ifdef OBJ_ELF
-# ifdef TE_FreeBSD
+# if TE_AMIGAOS
+  return "elf32-powerpc-amigaos";
+# else
+#  ifdef TE_FreeBSD
   return (ppc_obj64 ? "elf64-powerpc-freebsd" : "elf32-powerpc-freebsd");
-# elif defined (TE_VXWORKS)
+#  elif defined (TE_VXWORKS)
   return "elf32-powerpc-vxworks";
-# else
+#  else
   return (target_big_endian
 	  ? (ppc_obj64 ? "elf64-powerpc" : "elf32-powerpc")
 	  : (ppc_obj64 ? "elf64-powerpcle" : "elf32-powerpcle"));
+#  endif
 # endif
 #endif
 }
 
 /* Validate one entry in powerpc_opcodes[] or vle_opcodes[].
    Return TRUE if there's a problem, otherwise FALSE.  */
@@ -2108,12 +2112,16 @@ ppc_elf_suffix (char **str_p, expressionS *exp_p)
 
 #define MAP(str, reloc)   { str, sizeof (str) - 1, 1, 1, reloc }
 #define MAP32(str, reloc) { str, sizeof (str) - 1, 1, 0, reloc }
 #define MAP64(str, reloc) { str, sizeof (str) - 1, 0, 1, reloc }
 
   static const struct map_bfd mapping[] = {
+    MAP ("brel",		BFD_RELOC_PPC_AMIGAOS_BREL),
+    MAP ("brel@l",		BFD_RELOC_PPC_AMIGAOS_BREL_LO),
+    MAP ("brel@h",		BFD_RELOC_PPC_AMIGAOS_BREL_HI),
+    MAP ("brel@ha",		BFD_RELOC_PPC_AMIGAOS_BREL_HA),	
     MAP ("l",			BFD_RELOC_LO16),
     MAP ("h",			BFD_RELOC_HI16),
     MAP ("ha",			BFD_RELOC_HI16_S),
     MAP ("brtaken",		BFD_RELOC_PPC_B16_BRTAKEN),
     MAP ("brntaken",		BFD_RELOC_PPC_B16_BRNTAKEN),
     MAP ("got",			BFD_RELOC_16_GOTOFF),
@@ -3108,12 +3116,15 @@ fixup_size (bfd_reloc_code_real_type reloc, bool *pc_relative)
     case BFD_RELOC_PPC_TOC16_HI:
     case BFD_RELOC_PPC_TOC16_LO:
     case BFD_RELOC_PPC_TPREL16:
     case BFD_RELOC_PPC_TPREL16_HA:
     case BFD_RELOC_PPC_TPREL16_HI:
     case BFD_RELOC_PPC_TPREL16_LO:
+	case BFD_RELOC_PPC_AMIGAOS_BREL_LO:
+	case BFD_RELOC_PPC_AMIGAOS_BREL_HI:
+	case BFD_RELOC_PPC_AMIGAOS_BREL_HA:
       size = 2;
       break;
 
     case BFD_RELOC_16_PCREL:
     case BFD_RELOC_HI16_PCREL:
     case BFD_RELOC_HI16_S_PCREL:
@@ -3172,12 +3183,13 @@ fixup_size (bfd_reloc_code_real_type reloc, bool *pc_relative)
     case BFD_RELOC_PPC_VLE_SDAREL_HI16A:
     case BFD_RELOC_PPC_VLE_SDAREL_HI16D:
     case BFD_RELOC_PPC_VLE_SDAREL_LO16A:
     case BFD_RELOC_PPC_VLE_SDAREL_LO16D:
     case BFD_RELOC_PPC64_TLS_PCREL:
     case BFD_RELOC_RVA:
+	case BFD_RELOC_PPC_AMIGAOS_BREL:
       size = 4;
       break;
 
     case BFD_RELOC_24_PLT_PCREL:
     case BFD_RELOC_32_PCREL:
     case BFD_RELOC_32_PLT_PCREL:
@@ -7504,12 +7516,20 @@ md_apply_fix (fixS *fixP, valueT *valP, segT seg)
 
 	case BFD_RELOC_VTABLE_ENTRY:
 	  fixP->fx_done = 0;
 	  break;
 
 #ifdef OBJ_ELF
+	case BFD_RELOC_PPC_AMIGAOS_BREL:
+	case BFD_RELOC_PPC_AMIGAOS_BREL_HI:
+	case BFD_RELOC_PPC_AMIGAOS_BREL_LO:
+	case BFD_RELOC_PPC_AMIGAOS_BREL_HA:
+	  md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
+			      value, 2);
+	  break;
+
 	  /* These can appear with @l etc. in data.  */
 	case BFD_RELOC_LO16:
 	case BFD_RELOC_LO16_PCREL:
 	case BFD_RELOC_HI16:
 	case BFD_RELOC_HI16_PCREL:
 	case BFD_RELOC_HI16_S:
diff --git a/gas/config/te-amigaos.h b/gas/config/te-amigaos.h
new file mode 100644
index 0000000000000000000000000000000000000000..56f213ddd38b0475d86b0d5333aad1060f7f98f3
--- /dev/null
+++ gas/config/te-amigaos.h
@@ -0,0 +1,14 @@
+/*
+ * te-amigaos.h -- Amiga target environment declarations.
+ */
+
+#define TE_AMIGAOS 1
+
+#define LOCAL_LABELS_DOLLAR 1
+#define LOCAL_LABELS_FB 1
+
+#ifdef OBJ_HEADER
+#include OBJ_HEADER
+#else
+#include "obj-format.h"
+#endif
\ No newline at end of file
diff --git a/gas/configure.tgt b/gas/configure.tgt
index 765ba73633df54b51eec560002cc234ce98205c3..6b3d33c96e1eab00caab1a78d0e43798f5a64c31 100644
--- gas/configure.tgt
+++ gas/configure.tgt
@@ -358,12 +358,13 @@ case ${generic_target} in
   ppc-*-aix5.[01])			fmt=coff em=aix5 ;;
   ppc-*-aix[5-9].*)			fmt=coff em=aix5 ;;
   ppc-*-aix*)				fmt=coff em=aix ;;
   ppc-*-beos*)				fmt=coff ;;
   ppc-*-*n*bsd* | ppc-*-elf*)		fmt=elf ;;
   ppc-*-eabi* | ppc-*-sysv4*)		fmt=elf ;;
+  ppc-*-amigaos*)         	fmt=elf em=amigaos;;
   ppc-*-haiku*)				fmt=elf em=haiku ;;
   ppc-*-linux-*)			fmt=elf em=linux ;;
   ppc-*-solaris*)			fmt=elf em=solaris ;;
   ppc-*-macos*)				fmt=coff em=macos ;;
   ppc-*-nto*)				fmt=elf ;;
   ppc-*-kaos*)				fmt=elf ;;
diff --git a/gas/po/POTFILES.in b/gas/po/POTFILES.in
index 8f2efdb8d556578d94a98c68b5f4ddfc36bea36c..b0d625ffa80e23f05a8ae3f6d6064c8ab4d7aa17 100644
--- gas/po/POTFILES.in
+++ gas/po/POTFILES.in
@@ -182,12 +182,13 @@ config/tc-xtensa.h
 config/tc-z80.c
 config/tc-z80.h
 config/tc-z8k.c
 config/tc-z8k.h
 config/te-386bsd.h
 config/te-aix5.h
+config/te-amigaos.h
 config/te-armeabi.h
 config/te-armfbsdeabi.h
 config/te-armfbsdvfp.h
 config/te-armlinuxeabi.h
 config/te-csky_abiv1.h
 config/te-csky_abiv1_linux.h
diff --git a/include/elf/amigaos.h b/include/elf/amigaos.h
new file mode 100644
index 0000000000000000000000000000000000000000..2f12286430f85a5487808af69d3d38881de573c3
--- /dev/null
+++ include/elf/amigaos.h
@@ -0,0 +1,27 @@
+/* AmigaOS ELF support for BFD.
+   Copyright 2001 Free Software Foundation, Inc.
+
+This file is part of BFD, the Binary File Descriptor library.
+
+This program is free software; you can redistribute it and/or modify
+it under the terms of the GNU General Public License as published by
+the Free Software Foundation; either version 2 of the License, or
+(at your option) any later version.
+
+This program is distributed in the hope that it will be useful,
+but WITHOUT ANY WARRANTY; without even the implied warranty of
+MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+GNU General Public License for more details.
+
+You should have received a copy of the GNU General Public License
+along with this program; if not, write to the Free Software Foundation, Inc.,
+51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */
+
+#ifndef _ELF_AMIGAOS_H
+#define _ELF_AMIGAOS_H
+
+#include "elf/common.h"
+
+#define DT_AMIGAOS_DYNVERSION	(DT_LOOS + 1)
+
+#endif /* _ELF_AMIGAOS_H */
\ No newline at end of file
diff --git a/include/elf/ppc.h b/include/elf/ppc.h
index e0c54a66292c99dbc87d5fdf754421c63e158311..8af55a10a85f9017146c89a590a37aee8602439d 100644
--- include/elf/ppc.h
+++ include/elf/ppc.h
@@ -135,12 +135,18 @@ START_RELOC_NUMBERS (elf_ppc_reloc_type)
   RELOC_NUMBER (R_PPC_EMB_RELSDA,	116)
 
 /* Marker reloc for inline plt call insns.  */
   RELOC_NUMBER (R_PPC_PLTSEQ,		119)
   RELOC_NUMBER (R_PPC_PLTCALL,		120)
 
+/* AmigaOS4 relocs */
+  RELOC_NUMBER (R_PPC_AMIGAOS_BREL,	210)
+  RELOC_NUMBER (R_PPC_AMIGAOS_BREL_LO,	211)
+  RELOC_NUMBER (R_PPC_AMIGAOS_BREL_HI,  212)
+  RELOC_NUMBER (R_PPC_AMIGAOS_BREL_HA,  213)
+  
 /* PowerPC VLE relocations.  */
   RELOC_NUMBER (R_PPC_VLE_REL8,		216)
   RELOC_NUMBER (R_PPC_VLE_REL15,	217)
   RELOC_NUMBER (R_PPC_VLE_REL24,	218)
   RELOC_NUMBER (R_PPC_VLE_LO16A,	219)
   RELOC_NUMBER (R_PPC_VLE_LO16D,	220)
diff --git a/include/filenames.h b/include/filenames.h
index 444c5cc411cfc0518488193f378299ea13bc5584..3a12e2d025dc651e0faf16f9682f16976bdfd5a6 100644
--- include/filenames.h
+++ include/filenames.h
@@ -40,12 +40,22 @@ extern "C" {
 #  ifndef HAVE_CASE_INSENSITIVE_FILE_SYSTEM
 #    define HAVE_CASE_INSENSITIVE_FILE_SYSTEM 1
 #  endif
 #  define HAS_DRIVE_SPEC(f) HAS_DOS_DRIVE_SPEC (f)
 #  define IS_DIR_SEPARATOR(c) IS_DOS_DIR_SEPARATOR (c)
 #  define IS_ABSOLUTE_PATH(f) IS_DOS_ABSOLUTE_PATH (f)
+#elif defined(__amigaos4__)
+#  ifndef HAVE_AMIGA_BASED_FILE_SYSTEM
+#    define HAVE_AMIGA_BASED_FILE_SYSTEM 1
+#  endif
+#  ifndef HAVE_CASE_INSENSITIVE_FILE_SYSTEM
+#    define HAVE_CASE_INSENSITIVE_FILE_SYSTEM 1
+#  endif
+#  define HAS_DRIVE_SPEC(f) HAS_AMIGOS_DRIVE_SPEC (f)
+#  define IS_DIR_SEPARATOR(c) IS_AMIGOS_DIR_SEPARATOR (c)
+#  define IS_ABSOLUTE_PATH(f) IS_AMIGOS_ABSOLUTE_PATH (f)
 #else /* not DOSish */
 #  if defined(__APPLE__)
 #    ifndef HAVE_CASE_INSENSITIVE_FILE_SYSTEM
 #      define HAVE_CASE_INSENSITIVE_FILE_SYSTEM 1
 #    endif
 #  endif /* __APPLE__ */
@@ -60,18 +70,26 @@ extern "C" {
 
 #define HAS_DRIVE_SPEC_1(dos_based, f)			\
   ((f)[0] && ((f)[1] == ':') && (dos_based))
 
 /* Remove the drive spec from F, assuming HAS_DRIVE_SPEC (f).
    The result is a pointer to the remainder of F.  */
+#if defined(__amigaos4__)
+#define STRIP_DRIVE_SPEC(f)	(index( &(f)[0],':') + 1 )
+#else
 #define STRIP_DRIVE_SPEC(f)	((f) + 2)
+#endif
 
 #define IS_DOS_DIR_SEPARATOR(c) IS_DIR_SEPARATOR_1 (1, c)
 #define IS_DOS_ABSOLUTE_PATH(f) IS_ABSOLUTE_PATH_1 (1, f)
 #define HAS_DOS_DRIVE_SPEC(f) HAS_DRIVE_SPEC_1 (1, f)
 
+#define IS_AMIGOS_DIR_SEPARATOR(c) ( ((c) == '/') || ((c) == ':') )
+#define IS_AMIGOS_ABSOLUTE_PATH(f) HAS_AMIGOS_DRIVE_SPEC(f)
+#define HAS_AMIGOS_DRIVE_SPEC(f) (index (&(f)[0], ':') != NULL ) 
+
 #define IS_UNIX_DIR_SEPARATOR(c) IS_DIR_SEPARATOR_1 (0, c)
 #define IS_UNIX_ABSOLUTE_PATH(f) IS_ABSOLUTE_PATH_1 (0, f)
 
 /* Note that when DOS_BASED is true, IS_ABSOLUTE_PATH accepts d:foo as
    well, although it is only semi-absolute.  This is because the users
    of IS_ABSOLUTE_PATH want to know whether to prepend the current
diff --git a/include/libiberty.h b/include/libiberty.h
index 1d5c779fcff358a9c96ea0111f7306a9e6ffa6d0..e9d103d797572248720ffa8ff993ea61082fb991 100644
--- include/libiberty.h
+++ include/libiberty.h
@@ -124,12 +124,17 @@ extern const char *lbasename (const char *) ATTRIBUTE_RETURNS_NONNULL ATTRIBUTE_
 
 /* Same, but assumes DOS semantics (drive name, backslash is also a
    dir separator) regardless of host.  */
 
 extern const char *dos_lbasename (const char *) ATTRIBUTE_RETURNS_NONNULL ATTRIBUTE_NONNULL(1);
 
+/* Same, but assumes AMIGA semantics (drive name, slash is also a
+   dir separator) regardless of host.  */
+
+extern const char *amiga_lbasename (const char *) ATTRIBUTE_RETURNS_NONNULL ATTRIBUTE_NONNULL(1);
+
 /* Same, but assumes Unix semantics (absolute paths always start with
    a slash, only forward slash is accepted as dir separator)
    regardless of host.  */
 
 extern const char *unix_lbasename (const char *) ATTRIBUTE_RETURNS_NONNULL ATTRIBUTE_NONNULL(1);
 
diff --git a/ld/Makefile.am b/ld/Makefile.am
index 12b2c3c453fdbdb1fcac74bf5047bc4d0001483f..fe85a5adb17fe595aedd7fa05d4d7428721e9a16 100644
--- ld/Makefile.am
+++ ld/Makefile.am
@@ -155,12 +155,13 @@ ALL_EMULATION_SOURCES = \
 	eaix5ppc.c \
 	eaix5rs6.c \
 	eaixppc.c \
 	eaixrs6.c \
 	ealpha.c \
 	ealphavms.c \
+	eamigaos.c \
 	earcelf.c \
 	earclinux.c \
 	earclinux_nps.c \
 	earcv2elf.c \
 	earcv2elfx.c \
 	earm_wince_pe.c \
@@ -650,12 +651,13 @@ $(ALL_EMULATION_SOURCES) $(ALL_64_EMULATION_SOURCES): $(GEN_DEPENDS)
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/eaix5ppc.Pc@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/eaix5rs6.Pc@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/eaixppc.Pc@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/eaixrs6.Pc@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/ealpha.Pc@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/ealphavms.Pc@am__quote@
+@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/eamigaos.Pc@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earcelf.Pc@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earclinux.Pc@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earclinux_nps.Pc@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earcv2elf.Pc@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earcv2elfx.Pc@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earm_wince_pe.Pc@am__quote@
diff --git a/ld/Makefile.in b/ld/Makefile.in
index 3d5685d6bae1ce6258f053190da8d84f9eaba463..63ad373bbd053b1cbbbcce79e4cd0fd44dd780cb 100644
--- ld/Makefile.in
+++ ld/Makefile.in
@@ -656,12 +656,13 @@ ALL_EMULATION_SOURCES = \
 	eaix5ppc.c \
 	eaix5rs6.c \
 	eaixppc.c \
 	eaixrs6.c \
 	ealpha.c \
 	ealphavms.c \
+	eamigaos.c \
 	earcelf.c \
 	earclinux.c \
 	earclinux_nps.c \
 	earcv2elf.c \
 	earcv2elfx.c \
 	earm_wince_pe.c \
@@ -1269,12 +1270,13 @@ distclean-compile:
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/eaix5ppc.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/eaix5rs6.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/eaixppc.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/eaixrs6.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/ealpha.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/ealphavms.Po@am__quote@
+@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/eamigaos.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earcelf.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earclinux.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earclinux_nps.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earcv2elf.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earcv2elfx.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earm_wince_pe.Po@am__quote@
diff --git a/ld/configure.tgt b/ld/configure.tgt
index de04a44b8125f08095b792285be5ddbaf41e95f2..a2b3a54a94452c6ec882df7c41e63bd00b905b81 100644
--- ld/configure.tgt
+++ ld/configure.tgt
@@ -674,12 +674,16 @@ pdp11-*-*)		targ_emul=pdp11
 pjl*-*-*)		targ_emul=pjlelf
 			targ_extra_emuls="elf_i386 elf_iamcu"
 			;;
 pj*-*-*)		targ_emul=pjelf
 			targ_extra_ofiles=ldelfgen.o
 			;;
+powerpc-*-amigaos*)
+			targ_emul=amigaos
+			targ_extra_emuls=elf32ppc
+			;;			
 powerpc-*-freebsd* | powerpc-*-kfreebsd*-gnu)
 			targ_emul=elf32ppc_fbsd
 			targ_extra_emuls="elf32ppc elf32ppcsim"
 			targ_extra_libpath=elf32ppc;
 			tdir_elf32ppcsim=`echo ${targ_alias} | sed -e 's/ppc/ppcsim/'`
 			;;
@@ -1141,12 +1145,16 @@ i[03-9x]86-*-cygwin* | x86_64-*-cygwin*)
 *-*-linux*)
   ;;
 
 *-*-netbsd*)
   ;;
 
+powerpc-*-amigaos*)
+  NATIVE_LIB_DIRS='/gcc/local/lib /gcc/lib'
+  ;;
+
 alpha*-*-*)
   NATIVE_LIB_DIRS='/usr/local/lib /usr/ccs/lib /lib /usr/lib'
   ;;
 
 esac
 
diff --git a/ld/emulparams/amigaos.sh b/ld/emulparams/amigaos.sh
new file mode 100644
index 0000000000000000000000000000000000000000..c9e25b1cbb5060faac798aa9480dae874751ef5d
--- /dev/null
+++ ld/emulparams/amigaos.sh
@@ -0,0 +1,31 @@
+. ${srcdir}/emulparams/elf32ppccommon.sh
+. ${srcdir}/emulparams/plt_unwind.sh
+
+TEMPLATE_NAME=elf
+EXTRA_EM_FILE=ppc32elf
+SCRIPT_NAME=amigaos
+OUTPUT_FORMAT="elf32-powerpc-amigaos"
+MAXPAGESIZE="CONSTANT (MAXPAGESIZE)"
+COMMONPAGESIZE="CONSTANT (COMMONPAGESIZE)"
+DATA_SEGMENT_ALIGN="ALIGN(${SEGMENT_SIZE})"
+ALIGNMENT=16
+ARCH=powerpc
+MACHINE=
+GENERATE_SHLIB_SCRIPT=yes
+TEXT_START_ADDR=0x01000000
+SHLIB_TEXT_START_ADDR=0x10000000
+unset WRITABLE_RODATA
+DATA_START_SYMBOLS="_DATA_BASE_ = .;"
+SDATA_START_SYMBOLS="_SDA_BASE_ = . + 0x8000;"
+DATA_GOT=
+SDATA_GOT=
+TEXT_PLT=yes
+SEPARATE_GOTPLT=0
+unset BSS_PLT
+unset DATA_PLT
+GOT=".got          ${RELOCATING-0} : SPECIAL { *(.got) }"
+PLT=".plt          ${RELOCATING-0} :  { *(.plt) }"
+# GOTPLT="${PLT}"
+OTHER_TEXT_SECTIONS="*(.glink)"
+ENABLE_INITFINI_ARRAY=no
+DYNAMIC_LINK=false
\ No newline at end of file
diff --git a/ld/ldctor.c b/ld/ldctor.c
index 2f80aa02df699a0b1f3301412b02b2da030bac01..6bdbba10074d8052e83ca858cc7e6cec21c0d772 100644
--- ld/ldctor.c
+++ ld/ldctor.c
@@ -29,12 +29,13 @@
 #include "ldexp.h"
 #include "ldlang.h"
 #include "ldmisc.h"
 #include <ldgram.h>
 #include "ldmain.h"
 #include "ldctor.h"
+#include "elf-bfd.h"
 
 /* The list of statements needed to handle constructors.  These are
    invoked by the command CONSTRUCTORS in the linker script.  */
 lang_statement_list_type constructor_list;
 
 /* Whether the constructors should be sorted.  Note that this is
@@ -253,14 +254,19 @@ ldctor_build_sets (void)
       reloc_howto_type *howto;
       int reloc_size, size;
 
       /* If the symbol is defined, we may have been invoked from
 	 collect, and the sets may already have been built, so we do
 	 not do anything.  */
-      if (p->h->type == bfd_link_hash_defined
-	  || p->h->type == bfd_link_hash_defweak)
+	 /* dgv -- libnix v1.1 uses absolute sets that are also explicitly
+	 defined in the library so that the sets need to be build even
+	 if the symbol is defined */
+	  if (!(bfd_get_flavour (link_info.output_bfd) == bfd_target_elf_flavour &&
+		get_elf_backend_data (link_info.output_bfd)->target_os == is_amigaos) &&
+        (p->h->type == bfd_link_hash_defined
+	    || p->h->type == bfd_link_hash_defweak))
 	continue;
 
       /* For each set we build:
 	   set:
 	     .long number_of_elements
 	     .long element0
@@ -353,21 +359,28 @@ ldctor_build_sets (void)
 		  len = 0;
 		}
 	      print_spaces (20 - len);
 
 	      if (e->name != NULL)
 		minfo ("%pT\n", e->name);
-	      else
-		minfo ("%G\n", e->section->owner, e->section, e->value);
+		  else if (e->section->owner)
+	    minfo ("%G\n", e->section->owner, e->section, e->value);
+		  else
+		minfo ("%s\n", "** ABS **");
 	    }
 
 	  /* Need SEC_KEEP for --gc-sections.  */
 	  if (!bfd_is_abs_section (e->section))
 	    e->section->flags |= SEC_KEEP;
 
-	  if (bfd_link_relocatable (&link_info))
+	  /* dgv -- on the amiga, we want the constructors to be relocateable
+	     objects. However, this should be arranged somewhere else (FIXME) */
+	  if (bfd_link_relocatable (&link_info) ||
+		  (bfd_get_flavour (link_info.output_bfd) == bfd_target_elf_flavour &&
+           get_elf_backend_data (link_info.output_bfd)->target_os == is_amigaos &&
+	       e->section != bfd_abs_section_ptr))
 	    lang_add_reloc (p->reloc, howto, e->section, e->name,
 			    exp_intop (e->value));
 	  else
 	    lang_add_data (size, exp_relop (e->section, e->value));
 	}
 
diff --git a/ld/ldlang.c b/ld/ldlang.c
index b66d8c6bc1dd8c9bcb8f142c7111307513098588..2f546239ab0503fe4ba4a9318d9670e454db7349 100644
--- ld/ldlang.c
+++ ld/ldlang.c
@@ -3837,12 +3837,19 @@ typedef struct bfd_sym_chain ldlang_undef_chain_list_type;
 
 #define ldlang_undef_chain_list_head entry_symbol.next
 
 void
 ldlang_add_undef (const char *const name, bool cmdline ATTRIBUTE_UNUSED)
 {
+#if 1
+  /* This is a quick ugly hak of getting around the problem
+   * with -use-dynld being passed to the linker
+   */
+  if (strcmp(name, "se-dynld") == 0)
+    return;
+#endif	
   ldlang_undef_chain_list_type *new_undef;
 
   new_undef = stat_alloc (sizeof (*new_undef));
   new_undef->next = ldlang_undef_chain_list_head;
   ldlang_undef_chain_list_head = new_undef;
 
diff --git a/ld/ldmain.c b/ld/ldmain.c
index 9290a189b0d26fbd7bbcc97624ee45fbcd188584..308928445feced047f44a99c463f6d89174f9776 100644
--- ld/ldmain.c
+++ ld/ldmain.c
@@ -493,18 +493,23 @@ main (int argc, char **argv)
 	}
       link_info.has_map_file = true;
     }
 
   lang_process ();
 
-  /* Print error messages for any missing symbols, for any warning
+    /* Print error messages for any missing symbols, for any warning
      symbols, and possibly multiple definitions.  */
+#ifdef __amigaos4__
+  /* Make all files executable, even relocatable files */
+    link_info.output_bfd->flags |= EXEC_P;
+#else
   if (bfd_link_relocatable (&link_info))
     link_info.output_bfd->flags &= ~EXEC_P;
   else
     link_info.output_bfd->flags |= EXEC_P;
+#endif
 
   flagword flags = 0;
   switch (config.compress_debug)
     {
     case COMPRESS_DEBUG_GNU_ZLIB:
       flags = BFD_COMPRESS;
diff --git a/ld/po/BLD-POTFILES.in b/ld/po/BLD-POTFILES.in
index ff820172b9845f40be4fdae1df56137d3e2d65ac..018ac5d6dc7299a8dd57167b72d8851a07e075af 100644
--- ld/po/BLD-POTFILES.in
+++ ld/po/BLD-POTFILES.in
@@ -265,12 +265,14 @@ ends32elf16m.c
 ends32elf_linux.c
 enios2elf.c
 enios2linux.c
 ens32knbsd.c
 epc532macha.c
 epdp11.c
+eppcamiga.c
+eppcamiga_bss.c
 epjelf.c
 epjlelf.c
 eppcmacos.c
 epruelf.c
 escore3_elf.c
 escore7_elf.c
diff --git a/ld/scripttempl/elf.sc b/ld/scripttempl/amigaos.sc
similarity index 99%
copy from ld/scripttempl/elf.sc
copy to ld/scripttempl/amigaos.sc
index 5d3b0d31b1bbb5903730a785c32937939931d1cf..4b2b8a1e66ed188f672ca09baba622e40eff6502 100644
--- ld/scripttempl/elf.sc
+++ ld/scripttempl/amigaos.sc
@@ -535,13 +535,12 @@ cat <<EOF
   {
     ${RELOCATING+${INIT_START}}
     KEEP (*(SORT_NONE(.init)))
     ${RELOCATING+${INIT_END}}
   } ${FILL}
 
-  ${TEXT_PLT+${PLT_NEXT_DATA-${PLT} ${OTHER_PLT_SECTIONS}}}
   ${TINY_READONLY_SECTION}
   .text         ${RELOCATING-0} :
   {
     ${RELOCATING+${TEXT_START_SYMBOLS}}
     ${RELOCATING+*(.text.unlikely .text.*_unlikely .text.unlikely.*)}
     ${RELOCATING+*(.text.exit .text.exit.*)}
@@ -550,12 +549,15 @@ cat <<EOF
     ${RELOCATING+*(SORT(.text.sorted.*))}
     *(.text .stub${RELOCATING+ .text.* .gnu.linkonce.t.*})
     /* .gnu.warning sections are handled specially by elf.em.  */
     *(.gnu.warning)
     ${RELOCATING+${OTHER_TEXT_SECTIONS}}
   } ${FILL}
+  . = ALIGN(4096);
+  ${TEXT_PLT+${PLT_NEXT_DATA-${PLT} ${OTHER_PLT_SECTIONS}}}
+  . = ALIGN(4096);
   .fini         ${RELOCATING-0}${RELOCATING+${FINI_ADDR}} :
   {
     ${RELOCATING+${FINI_START}}
     KEEP (*(SORT_NONE(.fini)))
     ${RELOCATING+${FINI_END}}
   } ${FILL}
diff --git a/libiberty/Makefile.in b/libiberty/Makefile.in
index f9fbba23e2c81793bb7271f069b663b52813e812..8bbad59fcc5b137a4cfeb74995a0a05e2f5e8a7b 100644
--- libiberty/Makefile.in
+++ libiberty/Makefile.in
@@ -140,13 +140,13 @@ CFILES = alloca.c argv.c asprintf.c atexit.c				\
 	make-relative-prefix.c						\
 	make-temp-file.c md5.c memchr.c memcmp.c memcpy.c memmem.c	\
 	 memmove.c mempcpy.c memset.c mkstemps.c			\
 	objalloc.c obstack.c						\
 	partition.c pexecute.c						\
 	 pex-common.c pex-djgpp.c pex-msdos.c pex-one.c			\
-	 pex-unix.c pex-win32.c						\
+	 pex-unix.c pex-win32.c	pex-amigaos.c			\
          physmem.c putenv.c						\
 	random.c regex.c rename.c rindex.c				\
 	rust-demangle.c							\
 	safe-ctype.c setenv.c setproctitle.c sha1.c sigsetmask.c        \
 	 simple-object.c simple-object-coff.c simple-object-elf.c	\
 	 simple-object-mach-o.c simple-object-xcoff.c			\
@@ -213,13 +213,13 @@ CONFIGURED_OFILES = ./asprintf.$(objext) ./atexit.$(objext)		\
 	 ./gettimeofday.$(objext)					\
 	./index.$(objext) ./insque.$(objext)				\
 	./memchr.$(objext) ./memcmp.$(objext) ./memcpy.$(objext) 	\
 	./memmem.$(objext) ./memmove.$(objext)				\
 	 ./mempcpy.$(objext) ./memset.$(objext) ./mkstemps.$(objext)	\
 	./pex-djgpp.$(objext) ./pex-msdos.$(objext)			\
-	 ./pex-unix.$(objext) ./pex-win32.$(objext)			\
+	 ./pex-unix.$(objext) ./pex-win32.$(objext)	./pex-amigaos.$(objext)		\
 	 ./putenv.$(objext)						\
 	./random.$(objext) ./rename.$(objext) ./rindex.$(objext)	\
 	./setenv.$(objext) 						\
 	 ./setproctitle.$(objext)					\
 	 ./sigsetmask.$(objext) ./snprintf.$(objext)			\
 	 ./stpcpy.$(objext) ./stpncpy.$(objext) ./strcasecmp.$(objext)	\
@@ -1162,12 +1162,22 @@ $(CONFIGURED_OFILES): stamp-picdir stamp-noasandir
 	else true; fi
 	if [ x"$(NOASANFLAG)" != x ]; then \
 	  $(COMPILE.c) $(PICFLAG) $(NOASANFLAG) $(srcdir)/pex-unix.c -o noasan/$@; \
 	else true; fi
 	$(COMPILE.c) $(srcdir)/pex-unix.c $(OUTPUT_OPTION)
 
+./pex-amigaos.$(objext): $(srcdir)/pex-amigaos.c config.h $(INCDIR)/ansidecl.h \
+	$(INCDIR)/libiberty.h $(srcdir)/pex-common.h
+	if [ x"$(PICFLAG)" != x ]; then \
+	  $(COMPILE.c) $(PICFLAG) $(srcdir)/pex-amigaos.c -o pic/$@; \
+	else true; fi
+	if [ x"$(NOASANFLAG)" != x ]; then \
+	  $(COMPILE.c) $(PICFLAG) $(NOASANFLAG) $(srcdir)/pex-amigaos.c -o noasan/$@; \
+	else true; fi
+	$(COMPILE.c) $(srcdir)/pex-amigaos.c $(OUTPUT_OPTION)
+
 ./pex-win32.$(objext): $(srcdir)/pex-win32.c config.h $(INCDIR)/ansidecl.h \
 	$(INCDIR)/libiberty.h $(srcdir)/pex-common.h
 	if [ x"$(PICFLAG)" != x ]; then \
 	  $(COMPILE.c) $(PICFLAG) $(srcdir)/pex-win32.c -o pic/$@; \
 	else true; fi
 	if [ x"$(NOASANFLAG)" != x ]; then \
diff --git a/libiberty/configure b/libiberty/configure
index 1ccfac9fb11f334926f253d284b03a8335d94b2e..e5433eda5cf3bbe9527f12bae015535938e4995f 100755
--- libiberty/configure
+++ libiberty/configure
@@ -7537,12 +7537,13 @@ fi
 
 # Figure out which version of pexecute to use.
 case "${host}" in
      *-*-mingw* | *-*-winnt*)	pexecute=pex-win32  ;;
      *-*-msdosdjgpp*)		pexecute=pex-djgpp  ;;
      *-*-msdos*)		pexecute=pex-msdos  ;;
+	 powerpc-*-amigaos*)	pexecute=pex-amigaos ;;
      *)				pexecute=pex-unix   ;;
 esac
 
 
 
 
diff --git a/libiberty/configure.ac b/libiberty/configure.ac
index 6c1ff9c60933a95a150334652061cd649457eebc..e4aa76595652047ea04754425d5d890abb11cf64 100644
--- libiberty/configure.ac
+++ libiberty/configure.ac
@@ -733,12 +733,13 @@ fi
 
 # Figure out which version of pexecute to use.
 case "${host}" in
      *-*-mingw* | *-*-winnt*)	pexecute=pex-win32  ;;
      *-*-msdosdjgpp*)		pexecute=pex-djgpp  ;;
      *-*-msdos*)		pexecute=pex-msdos  ;;
+	 powerpc-*-amigaos*)	pexecute=pex-amigaos ;;
      *)				pexecute=pex-unix   ;;
 esac
 AC_SUBST(pexecute)
 
 libiberty_AC_FUNC_STRNCMP
 
diff --git a/libiberty/fnmatch.c b/libiberty/fnmatch.c
index 3602059ce8a5897d82fc8be62c40b37344713f0d..4b3e030d040bf102bd1f342c40f2f203338c924d 100644
--- libiberty/fnmatch.c
+++ libiberty/fnmatch.c
@@ -44,20 +44,23 @@ Boston, MA 02110-1301, USA.  */
 #endif
 
 #include <errno.h>
 #include <fnmatch.h>
 #include <safe-ctype.h>
 
+/* Ignore all this code if targeting AmigaOS4 with clib clib4, because
+   the clib4 clib provided it own fnmatch implementation */
+#if !(defined(__amigaos4__) && defined(__CLIB4__))
+
 /* Comment out all this code if we are using the GNU C Library, and are not
    actually compiling the library itself.  This code is part of the GNU C
    Library, but also included in many other GNU distributions.  Compiling
    and linking in this code is a waste when using the GNU C library
    (especially if it is a shared library).  Rather than having every GNU
    program understand `configure --with-gnu-libc' and omit the object files,
    it is simpler to just do this in the source for each such file.  */
-
 #if defined (_LIBC) || !defined (__GNU_LIBRARY__)
 
 
 #if !defined(__GNU_LIBRARY__) && !defined(STDC_HEADERS)
 extern int errno;
 #endif
@@ -215,6 +218,7 @@ fnmatch (const char *pattern, const char *string, int flags)
     return 0;
 
   return FNM_NOMATCH;
 }
 
 #endif	/* _LIBC or not __GNU_LIBRARY__.  */
+#endif  /* not (AMIGAOS and CLIB4) $*/
diff --git a/libiberty/lbasename.c b/libiberty/lbasename.c
index a7864ab2b8c44c198b210b681efbadfcf8415fbb..d1f40818b2243b317e0d4fc65ee2978297ed890c 100644
--- libiberty/lbasename.c
+++ libiberty/lbasename.c
@@ -70,15 +70,35 @@ dos_lbasename (const char *name)
     if (IS_DOS_DIR_SEPARATOR (*name))
       base = name + 1;
 
   return base;
 }
 
+const char *
+amiga_lbasename (const char *name)
+{
+  const char *base = name;
+
+  /* Skip over a possible disk name.  */
+  if (HAS_AMIGOS_DRIVE_SPEC (name)) {
+    base = STRIP_DRIVE_SPEC (name);
+  };
+
+  /* Strip any directories */
+  for (; *name; name++)
+    if (IS_AMIGOS_DIR_SEPARATOR (*name))
+      base = name + 1;
+
+  return base;
+}
+
 const char *
 lbasename (const char *name)
 {
 #if defined (HAVE_DOS_BASED_FILE_SYSTEM)
   return dos_lbasename (name);
+#elif defined (HAVE_AMIGA_BASED_FILE_SYSTEM)
+  return amiga_lbasename (name);
 #else
   return unix_lbasename (name);
 #endif
 }
diff --git a/libiberty/make-temp-file.c b/libiberty/make-temp-file.c
index fae743f398539d5217bcaca5f1a3de7be9f9e547..5f66abacc55f0dde71d19e7543f7e1a6453496a9 100644
--- libiberty/make-temp-file.c
+++ libiberty/make-temp-file.c
@@ -153,12 +153,14 @@ choose_tmpdir (void)
       len = strlen (base);
       tmpdir = XNEWVEC (char, len + 2);
       strcpy (tmpdir, base);
       tmpdir[len] = DIR_SEPARATOR;
       tmpdir[len+1] = '\0';
       memoized_tmpdir = tmpdir;
+#elif __amigaos4__
+    memoized_tmpdir = xstrdup ("T:");
 #else /* defined(_WIN32) && !defined(__CYGWIN__) */
       DWORD len;
 
       /* Figure out how much space we need.  */
       len = GetTempPath(0, NULL);
       if (len)
diff --git a/libiberty/pex-amigaos.c b/libiberty/pex-amigaos.c
new file mode 100644
index 0000000000000000000000000000000000000000..0c61a108764c8501f8a2e9552c7c07499f402b1a
--- /dev/null
+++ libiberty/pex-amigaos.c
@@ -0,0 +1,325 @@
+/* Utilities to execute a program in a subprocess (possibly linked by pipes
+   with other subprocesses), and wait for it.  Generic AMIGAOS specialization.
+   Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2003, 2005
+   Free Software Foundation, Inc.
+
+This file is part of the libiberty library.
+Libiberty is free software; you can redistribute it and/or
+modify it under the terms of the GNU Library General Public
+License as published by the Free Software Foundation; either
+version 2 of the License, or (at your option) any later version.
+
+Libiberty is distributed in the hope that it will be useful,
+but WITHOUT ANY WARRANTY; without even the implied warranty of
+MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
+Library General Public License for more details.
+
+You should have received a copy of the GNU Library General Public
+License along with libiberty; see the file COPYING.LIB.  If not,
+write to the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
+Boston, MA 02110-1301, USA.  */
+
+#include "pex-common.h"
+
+#include <stdio.h>
+#include <errno.h>
+#ifdef NEED_DECLARATION_ERRNO
+extern int errno;
+#endif
+#ifdef HAVE_STDLIB_H
+#include <stdlib.h>
+#endif
+#include <string.h>
+#include <fcntl.h>
+#include <unistd.h>
+#include <sys/stat.h>
+#include <ctype.h>
+
+/* Use ECHILD if available, otherwise use EINVAL.  */
+#ifdef ECHILD
+#define PWAIT_ERROR ECHILD
+#else
+#define PWAIT_ERROR EINVAL
+#endif
+
+#if !defined(FD_CLOEXEC)
+#define FD_CLOEXEC 1
+#endif
+
+static int pex_amiga_open_read (struct pex_obj *, const char *, int);
+static int pex_amiga_open_write (struct pex_obj *, const char *, int);
+static pid_t pex_amiga_exec_child (struct pex_obj *, int, const char *,
+				 char * const *, char * const *,
+				 int, int, int, int,
+				 const char **, int *);
+static int pex_amiga_close (struct pex_obj *, int);
+static int pex_amiga_wait (struct pex_obj *, long, int *, struct pex_time *,
+			   int, const char **, int *);
+static FILE *pex_amiga_fdopenr (struct pex_obj *, int, int);
+static FILE *pex_amiga_fdopenw (struct pex_obj *, int, int);
+
+/* The list of functions we pass to the common routines.  */
+
+const struct pex_funcs funcs =
+{
+  pex_amiga_open_read,
+  pex_amiga_open_write,
+  pex_amiga_exec_child,
+  pex_amiga_close,
+  pex_amiga_wait,
+  NULL, /* pipe */
+  pex_amiga_fdopenr,
+  pex_amiga_fdopenw,
+  NULL, /* cleanup */
+};
+
+/* Return a newly initialized pex_obj structure.  */
+
+struct pex_obj *
+pex_init (int flags, const char *pname, const char *tempbase)
+{
+  /* AMIGAOS does not support pipes.  */
+  flags &= ~ PEX_USE_PIPES;
+  return pex_init_common (flags, pname, tempbase, &funcs);
+}
+
+/* Open a file for reading.  */
+
+static int
+pex_amiga_open_read (struct pex_obj *obj ATTRIBUTE_UNUSED, const char *name,
+		    int binary ATTRIBUTE_UNUSED)
+{
+  return open (name, O_RDONLY);
+}
+
+/* Open a file for writing.  */
+
+static int
+pex_amiga_open_write (struct pex_obj *obj ATTRIBUTE_UNUSED, const char *name,
+		     int binary ATTRIBUTE_UNUSED)
+{
+  /* Note that we can't use O_EXCL here because gcc may have already
+     created the temporary file via make_temp_file.  */
+  return open (name, O_WRONLY | O_CREAT | O_TRUNC);
+}
+
+/* Close a file.  */
+
+static int
+pex_amiga_close (struct pex_obj *obj ATTRIBUTE_UNUSED, int fd)
+{
+  return close (fd);
+}
+
+/* Execute a child.  */
+
+const unsigned char __shell_escape_character = '\\';
+
+static pid_t
+pex_amiga_exec_child (struct pex_obj *obj, int flags ATTRIBUTE_UNUSED, const char *executable ATTRIBUTE_UNUSED,
+		     char * const * argv, char * const * env ATTRIBUTE_UNUSED,
+                     int in ATTRIBUTE_UNUSED, int out ATTRIBUTE_UNUSED, int errdes ATTRIBUTE_UNUSED,
+		     int toclose ATTRIBUTE_UNUSED, const char **errmsg, int *err)
+{
+  int rc;
+  char *scmd,*s;
+  int i,j,c,len,arglen;
+  int need_quote;
+  int already_have_quote;
+  int escaped;
+  int *statuses;
+
+  len = 0;
+
+  for(i = 0 ; argv[i] != NULL ; i++)
+  {
+    arglen = strlen(argv[i]);
+
+    len += 1 + arglen;
+
+    need_quote = already_have_quote = 0;
+
+    /* Check if this parameter is already surrounded by double quotes.
+       What counts is that the first character is a double quote. We
+       hope that the last character is an unescaped double quote, but
+       don't check for it. */
+    if(argv[i][0] == '\"')
+    {
+      already_have_quote = 1;
+    }
+    else
+    {
+      /* Check if there's a blank space in the argument. If so, we will
+         need to add double quote characters. */
+      for (j = 0 ; j < arglen ; j++)
+      {
+        c = argv[i][j];
+
+        if (isspace(c))
+        {
+          need_quote = 1;
+          break;
+        }
+      }
+
+      /* Make room for the double quote characters that we will have to add. */
+      if(need_quote)
+        len += 2;
+    }
+
+    /* Check if there are " or * characters in the quoted string which
+       may have to be escaped. */
+    if (need_quote || already_have_quote)
+    {
+      for (j = 0 ; j < arglen ; j++)
+      {
+        c = argv[i][j];
+
+        /* We just might have to add an escape character in front of these two. */
+        if (c == '\"' || c == '*')
+	        len++;
+      }
+    }
+  }
+
+  s = scmd = (char *) xmalloc (len+1);
+
+  for(i = 0 ; argv[i] != NULL ; i++)
+  {
+    arglen = strlen(argv[i]);
+
+    need_quote = already_have_quote = 0;
+
+    if (argv[i][0] == '\"')
+    {
+      already_have_quote = 1;
+    }
+    else
+    {
+      for (j = 0 ; j < arglen ; j++)
+      {
+        c = argv[i][j];
+
+        if (isspace(c))
+        {
+          need_quote = 1;
+          break;
+        }
+      }
+    }
+
+    if(s != scmd)
+      (*s++) = ' ';
+
+    if(need_quote)
+      (*s++) = '\"';
+
+    escaped = 0;
+
+    for(j = 0 ; j < arglen ; j++)
+    {
+      c = argv[i][j];
+
+      /* If this is a " or * and the parameter is quoted, try to
+         add an escape character in front of it. */
+      if((c == '\"' || c == '*') && (need_quote || already_have_quote))
+	    {
+        /* Careful, don't escape the first double
+           quote character by mistake. */
+        if(!already_have_quote || j > 0)
+        {
+          /* Don't add an escape character here if the previous character
+             already was an escape character. */
+          if(!escaped)
+            (*s++) = '*';
+	      }
+      }
+
+      (*s++) = c;
+
+      /* Remember if the last character read was an escape character. */
+      if (escaped)
+        escaped = 0;
+      else
+        escaped = (c == __shell_escape_character && c != '*');
+    }
+
+    if(need_quote)
+      (*s++) = '\"';
+  }
+
+  (*s) = '\0';
+
+  rc = system (scmd);
+
+  free (scmd);
+
+  if (rc == -1)
+  {
+    *err = errno;
+    *errmsg = install_error_msg;
+    return -1;
+  }
+
+  /* Save the exit status for later.  When we are called, obj->count
+     is the number of children which have executed before this
+     one.  */
+  statuses = (int *) obj->sysdep;
+  statuses = XRESIZEVEC (int, statuses, obj->count + 1);
+  statuses[obj->count] = (rc << 8); /* Tuck the status away for pwait */
+  obj->sysdep = (void *) statuses;
+
+  return obj->count;
+}
+
+/* Create a pipe.  */
+/*
+static int
+pex_amiga_pipe (struct pex_obj *obj ATTRIBUTE_UNUSED, int *p,
+	       int binary ATTRIBUTE_UNUSED)
+{
+  return pipe (p);
+}
+*/
+
+/* Get a FILE pointer to read from a file descriptor.  */
+
+static FILE *
+pex_amiga_fdopenr (struct pex_obj *obj ATTRIBUTE_UNUSED, int fd,
+		  int binary ATTRIBUTE_UNUSED)
+{
+  return fdopen (fd, "r");
+}
+
+/* Get a FILE pointer to write to a file descriptor.  */
+
+static FILE *
+pex_amiga_fdopenw (struct pex_obj *obj ATTRIBUTE_UNUSED, int fd,
+		  int binary ATTRIBUTE_UNUSED)
+{
+  if (fcntl (fd, F_SETFD, FD_CLOEXEC) < 0)
+    return NULL;
+  return fdopen (fd, "w");
+}
+
+
+/* Wait for a child process to complete.  Actually the child process
+   has already completed, and we just need to return the exit
+   status.  */
+
+static int
+pex_amiga_wait (struct pex_obj *obj, long pid, int *status,
+		struct pex_time *time, int done ATTRIBUTE_UNUSED,
+		const char **errmsg ATTRIBUTE_UNUSED,
+		int *err ATTRIBUTE_UNUSED)
+{
+  int *statuses;
+
+  if (time != NULL)
+    memset (time, 0, sizeof (struct pex_time));
+
+  statuses = (int *) obj->sysdep;
+  *status = statuses[pid];
+
+  return 0;
+}

