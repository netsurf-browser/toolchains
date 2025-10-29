Index: libunixlib/Makefile.am
===================================================================
--- libunixlib/Makefile.am	(revision 7698)
+++ libunixlib/Makefile.am	(working copy)
@@ -1002,18 +1002,11 @@
 	string/strtok.c \
 	string/swab.c \
 	string/_strcpy.s
-if ARM_EABI
 string_src += \
-	string/_memcpymove-v7l.s \
-	string/_memset-v7l.s \
-	string/_strlen-v7l.s
-else
-string_src += \
 	string/_memcpy.s \
 	string/_memset.s \
 	string/_strlen.s
 endif
-endif
 string_src += \
 	string/memccpy.c \
 	string/memchr.c \
@@ -1029,10 +1022,8 @@
 	string/strtok_r.c \
 	string/_ffs.s \
 	string/_ffsll.s
-if !ARM_EABI
 string_src += \
 	string/mempcpy.c
-endif
 
 if UNIXLIB_BUILDING_SCL
 sys_src =
