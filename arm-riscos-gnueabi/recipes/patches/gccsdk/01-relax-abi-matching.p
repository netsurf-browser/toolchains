Index: gcc/bfd.config.bfd.pp
===================================================================
--- gcc/bfd.config.bfd.pp	(revision 7698)
+++ gcc/bfd.config.bfd.pp	(working copy)
@@ -4,7 +4,7 @@
      targ_defvec=arm_elf32_le_vec
      targ_selvecs=arm_elf32_be_vec
      ;;
-+  arm-riscos-gnueabihf)
++  arm-riscos-gnueabi*)
 +    targ_defvec=arm_elf32_le_vec
 +    targ_selvecs=arm_elf32_be_vec
 +    targ_cflags=-D__RISCOS_TARGET__
Index: gcc/configure.ac.p
===================================================================
--- gcc/configure.ac.p	(revision 7698)
+++ gcc/configure.ac.p	(working copy)
@@ -13,7 +13,7 @@
  esac
  
 +case "${target}" in
-+  arm*-*-riscos | *riscos-gnueabihf)
++  arm*-*-riscos | *riscos-gnueabi*)
 +    ;;
 +  *)
 +    noconfigdirs="$noconfigdirs target-libunixlib"
Index: gcc/gas.configure.tgt.pp
===================================================================
--- gcc/gas.configure.tgt.pp	(revision 7698)
+++ gcc/gas.configure.tgt.pp	(working copy)
@@ -4,7 +4,7 @@
    arm-*-coff)				fmt=coff ;;
    arm-*-phoenix*)			fmt=elf ;;
    arm-*-elf)				fmt=elf ;;
-+  arm-riscos-gnueabihf)			fmt=elf em=riscos ;;
++  arm-riscos-gnueabi*)			fmt=elf em=riscos ;;
    arm-*-eabi* | arm-*-rtems*)		fmt=elf em=armeabi ;;
    arm-*-symbianelf*)			fmt=elf em=symbian ;;
    arm-*-kaos*)				fmt=elf ;;
Index: gcc/gcc.config.gcc.p
===================================================================
--- gcc/gcc.config.gcc.p	(revision 7698)
+++ gcc/gcc.config.gcc.p	(working copy)
@@ -13,7 +13,7 @@
  	esac
  	tm_file="${tm_file} arm/aout.h arm/arm.h"
  	;;
-+arm*-riscos-gnueabihf)
++arm*-riscos-gnueabi*)
 +	gas=yes
 +	gnu_ld=yes
 +	tm_file="dbxelf.h elfos.h glibc-stdint.h arm/elf.h arm/aout.h arm/arm.h arm/riscos-gnueabihf.h"
Index: gcc/ld.configure.tgt.pp
===================================================================
--- gcc/ld.configure.tgt.pp	(revision 7698)
+++ gcc/ld.configure.tgt.pp	(working copy)
@@ -4,7 +4,7 @@
  arm-*-phoenix*)		targ_emul=armelf ;;
  armeb-*-elf | armeb-*-eabi*)
  			targ_emul=armelfb ;;
-+arm-riscos-gnueabihf)	targ_emul=armelf_riscos_eabi ;;
++arm-riscos-gnueabi*)	targ_emul=armelf_riscos_eabi ;;
  arm-*-elf | arm*-*-eabi* | arm-*-rtems*)
  			targ_emul=armelf ;;
  arm*-*-symbianelf*)	targ_emul=armsymbian;;
Index: gcc/libgcc.config.host.p
===================================================================
--- gcc/libgcc.config.host.p	(revision 7698)
+++ gcc/libgcc.config.host.p	(working copy)
@@ -4,7 +4,7 @@
    tmake_file="$tmake_file t-crtstuff-pic t-libgcc-pic t-eh-dw2-dip t-slibgcc t-slibgcc-fuchsia"
    extra_parts="crtbegin.o crtend.o"
    ;;
-+arm*-riscos-gnueabihf)
++arm*-riscos-gnueabi*)
 +  tmake_file="$tmake_file t-crtstuff-pic t-libgcc-pic t-eh-dw2-dip t-slibgcc t-slibgcc-gld t-slibgcc-elf-ver t-linux"
 +  extra_parts="crtbegin.o crtbeginS.o crtbeginT.o crtend.o crtendS.o crti.o crtn.o"
 +  if test x$enable_vtable_verify = xyes; then
@@ -18,7 +18,7 @@
  	    ;;
  	esac
  	;;
-+arm*-riscos-gnueabihf)
++arm*-riscos-gnueabi*)
 +	tmake_file="${tmake_file} arm/t-arm t-fixedpoint-gnu-prefix t-crtfm"
 +	tmake_file="${tmake_file} arm/t-elf arm/t-bpabi arm/t-linux-eabi t-slibgcc-libgcc t-slibgcc-nolc-override"
 +	tm_file="$tm_file arm/bpabi-lib.h"
Index: gcc/libstdc++-v3.configure.host.p
===================================================================
--- gcc/libstdc++-v3.configure.host.p	(revision 7698)
+++ gcc/libstdc++-v3.configure.host.p	(working copy)
@@ -4,7 +4,7 @@
    linux-musl*)
      os_include_dir="os/generic"
      ;;
-+  gnueabihf)
++  gnueabi*)
 +    os_include_dir="os/riscos"
 +    ;;
    gnu* | linux* | kfreebsd*-gnu | uclinux*)
