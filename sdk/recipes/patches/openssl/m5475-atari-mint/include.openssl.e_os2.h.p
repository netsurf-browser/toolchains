--- include/openssl/e_os2.h.orig	2018-06-03 03:39:16.000000000 +0200
+++ include/openssl/e_os2.h	2018-06-03 03:43:34.000000000 +0200
@@ -212,6 +212,11 @@
 #  endif
 # endif
 
+# ifdef __MINT__
+#  define ossl_ssize_t int
+#  define OSSL_SSIZE_MAX INT_MAX
+# endif
+
 # if defined(OPENSSL_SYS_UEFI) && !defined(ossl_ssize_t)
 #  define ossl_ssize_t INTN
 #  define OSSL_SSIZE_MAX MAX_INTN
