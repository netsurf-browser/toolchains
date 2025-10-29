--- c/tools/brotli.c.orig	2024-11-06 18:14:10.818290440 +0000
+++ c/tools/brotli.c	2024-11-06 18:14:43.910412091 +0000
@@ -82,6 +82,7 @@
 #else
 #define HAVE_UTIMENSAT 0
 #endif
+#define HAVE_UTIMENSAT 0
 
 #if HAVE_UTIMENSAT
 #if defined(__APPLE__)
