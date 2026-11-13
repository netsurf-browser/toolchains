--- GNUmakefile.68k	2021-05-13 23:38:42.000000000 +0100
+++ GNUmakefile.68k	2026-10-10 23:26:07.648870441 +0100
@@ -24,9 +24,9 @@ MAKE = $(MAKE_COMMAND) -f GNUmakefile.68
 
 # override certain things for non-native builds
 ifneq ($(HOST), AmigaOS)
-   CC = m68k-amigaos-gcc
-   AR = m68k-amigaos-ar -q
-   RANLIB = m68k-amigaos-ranlib
+   CC = m68k-unknown-amigaos-gcc
+   AR = m68k-unknown-amigaos-ar -q
+   RANLIB = m68k-unknown-amigaos-ranlib
    COPY = cp
    DELETE = rm -rf
    MAKEDIR = mkdir -p
@@ -423,6 +423,7 @@ C_LIB = \
 	string_strncat.o \
 	string_strncmp.o \
 	string_strncpy.o \
+	string_strnlen.o \
 	string_strpbrk.o \
 	string_strrchr.o \
 	string_strspn.o \
@@ -539,6 +540,7 @@ UNIX_LIB = \
 	stdlib_realloc.o \
 	stdlib_resetmemstats.o \
 	stdlib_system.o \
+	stubs.o \
 	systeminfo_sysinfo.o \
 	termios_cfgetispeed.o \
 	termios_cfgetospeed.o \
@@ -988,10 +990,10 @@ all: \
 	lib/n32bcrt0.o \
 	lib/n32rcrt0.o \
 	lib/libm020/libm.a \
-	lib/libm.a \
-	lib/libb/libm.a \
-	lib/libb/libm020/libm.a \
-	lib/libb32/libm020/libm.a
+	lib/libm.a
+#	lib/libb/libm.a \
+#	lib/libb/libm020/libm.a \
+#	lib/libb32/libm020/libm.a
 
 ##############################################################################
 
