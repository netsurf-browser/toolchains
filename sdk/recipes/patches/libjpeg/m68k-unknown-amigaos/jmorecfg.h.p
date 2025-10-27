--- ./builddir-m68k-unknown-amigaos/libjpeg/jpeg-8d/jmorecfg.h	2025-09-05 11:25:10.036000000 +0100
+++ ./jmorecfg.h	2025-09-05 11:28:12.836000000 +0100
@@ -317,9 +317,9 @@ typedef int boolean;
  *    can't use color quantization if you change that value.
  */
 
-#define RGB_RED		0	/* Offset of Red in an RGB scanline element */
+#define RGB_RED		2	/* Offset of Red in an RGB scanline element */
 #define RGB_GREEN	1	/* Offset of Green */
-#define RGB_BLUE	2	/* Offset of Blue */
+#define RGB_BLUE	0	/* Offset of Blue */
 #define RGB_PIXELSIZE	4	/* JSAMPLEs per RGB scanline element */
 
 
