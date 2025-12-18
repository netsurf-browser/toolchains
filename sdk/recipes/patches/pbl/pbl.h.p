--- src/pbl.h.orig	2012-08-04 20:24:02.000000000 +0100
+++ src/pbl.h	2012-08-04 20:24:50.000000000 +0100
@@ -79,7 +79,7 @@
     #include <io.h>
 	#include <string.h>
 #else
-#ifndef __CYGWIN__
+#if !defined(__CYGWIN__) && !defined(__AMIGA__)
     #include <sys/dir.h>
 #endif
 #endif

