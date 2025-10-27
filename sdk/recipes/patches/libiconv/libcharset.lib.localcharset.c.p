--- libcharset/lib/localcharset.c.old	2010-12-29 19:12:21.000000000 +0000
+++ libcharset/lib/localcharset.c	2010-12-29 19:12:49.000000000 +0000
@@ -824,9 +824,6 @@
    If the canonical name cannot be determined, the result is a non-canonical
    name.  */
 
-#ifdef STATIC
-STATIC
-#endif
 const char *
 locale_charset (void)
 {
