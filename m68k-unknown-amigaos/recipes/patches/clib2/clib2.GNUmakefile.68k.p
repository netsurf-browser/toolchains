--- GNUmakefile.68k	2016-11-27 14:53:40.000000000 +0000
+++ GNUmakefile.68k	2016-12-05 18:33:20.094019332 +0000
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
@@ -123,9 +123,9 @@ WARNINGS = \
 
 INCLUDES = -Iinclude -I. -Inetinclude
 #OPTIONS = -fno-builtin -fno-common -DDEBUG
- OPTIONS = -fno-builtin -fno-common -DNDEBUG
+#OPTIONS = -fno-builtin -fno-common -DNDEBUG
 #OPTIONS = -fno-builtin -fno-common -DNDEBUG -D__THREAD_SAFE
-#OPTIONS = -fno-builtin -fno-common -D__MEM_DEBUG
+OPTIONS = -fno-builtin -fno-common -D__MEM_DEBUG
 #OPTIONS = -fno-builtin -fno-common -DDEBUG -D__MEM_DEBUG -DNO_INLINE_STDARG
 #OPTIMIZE = -O3 -fomit-frame-pointer -funroll-loops    -finline-functions
 OPTIMIZE = -O  -fomit-frame-pointer -fstrength-reduce -finline-functions
@@ -537,6 +537,7 @@ UNIX_LIB = \
 	stdlib_realloc.o \
 	stdlib_resetmemstats.o \
 	stdlib_system.o \
+	stubs.o \
 	systeminfo_sysinfo.o \
 	termios_cfgetispeed.o \
 	termios_cfgetospeed.o \
@@ -986,10 +987,10 @@ all: \
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
 
