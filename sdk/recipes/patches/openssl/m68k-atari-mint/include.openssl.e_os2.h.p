--- include/openssl/e_os2.h.orig	2018-06-03 03:39:16.000000000 +0200
+++ include/openssl/e_os2.h	2018-06-03 03:43:34.000000000 +0200
@@ -230,6 +230,12 @@
 #endif
 #endif
 
+#ifdef __MINT__
+#include <sys/types.h>
+typedef int ossl_ssize_t;
+#define OSSL_SSIZE_MAX INT_MAX
+#endif
+
 #ifndef OSSL_SSIZE_MAX
 #include <sys/types.h>
 typedef ssize_t ossl_ssize_t;
