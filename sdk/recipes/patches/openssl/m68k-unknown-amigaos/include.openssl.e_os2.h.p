--- include/openssl/e_os2.h.orig	2017-11-21 23:09:39.684814614 +0000
+++ include/openssl/e_os2.h	2017-11-21 23:10:32.501369938 +0000
@@ -223,6 +223,8 @@
 #   define OSSL_SSIZE_MAX SSIZE_MAX
 #  elif defined(_POSIX_SSIZE_MAX)
 #   define OSSL_SSIZE_MAX _POSIX_SSIZE_MAX
+#  else
+#   define OSSL_SSIZE_MAX INT_MAX
 #  endif
 # endif
 
