--- srclib/fseterr.c.orig	2026-10-13 17:58:07.646125749 +0100
+++ srclib/fseterr.c	2026-10-13 18:07:36.863754557 +0100
@@ -53,6 +53,8 @@
 #elif defined EPLAN9                /* Plan9 */
   if (fp->state != 0 /* CLOSED */)
     fp->state = 5 /* ERR */;
+#elif defined __amigaos__           /* AmigaOS */
+  fp->flags |= __FILE_ERROR;
 #elif 0                             /* unknown  */
   /* Portable fallback, based on an idea by Rich Felker.
      Wow! 6 system calls for something that is just a bit operation!
