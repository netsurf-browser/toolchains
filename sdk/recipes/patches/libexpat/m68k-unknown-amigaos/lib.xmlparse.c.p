--- lib/xmlparse.c.orig	2026-10-14 00:09:14.307546858 +0100
+++ lib/xmlparse.c	2026-10-14 00:11:45.033405229 +0100
@@ -109,7 +109,9 @@
 #include <stdint.h> /* SIZE_MAX, UINT64_MAX, uint64_t, uintptr_t */
 #include <math.h>   /* isnan */
 #include <errno.h>
-#include <wchar.h> /* wcsncmp */
+#if defined(XML_UNICODE) && defined(XML_UNICODE_WCHAR_T)
+#  include <wchar.h> /* wcsncmp */
+#endif
 
 #ifdef _WIN32
 #  define getpid GetCurrentProcessId
