--- src/Dither.h.orig	2013-10-07 23:00:38.000000000 +0200
+++ src/Dither.h	2013-10-07 21:53:56.000000000 +0200
@@ -20,7 +20,7 @@
 extern char8 DitherTab_b332_44[4][4][256];
 
 
-void Dither_SetupMatrices();
+void Dither_SetupMatrices(void);
 
 
 
