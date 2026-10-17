--- src/src/pbl.h.orig	2026-10-13 19:28:49.223523996 +0100
+++ src/src/pbl.h	2026-10-13 19:29:18.255831479 +0100
@@ -48,7 +48,7 @@
     #include <io.h>
 	#include <string.h>
 #else
-#ifndef __CYGWIN__
+#if !defined(__CYGWIN__) && !defined(__AMIGA__)
     #include <sys/dir.h>
 #endif
 #endif
