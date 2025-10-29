--- apps/lib/opt.c.orig	2017-10-10 15:19:45.611896396 +0100
+++ apps/lib/opt.c	2017-10-17 15:42:25.304921197 +0100
@@ -562,7 +562,7 @@
 
 #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L && \
     defined(INTMAX_MAX) && defined(UINTMAX_MAX) && \
-    !defined(OPENSSL_NO_INTTYPES_H)
+    !defined(OPENSSL_NO_INTTYPES_H) && !defined(OPENSSL_SYS_AMIGAOS3) && !defined(OPENSSL_SYS_AMIGAOS4)
 
 /* Parse an intmax_t, put it into *result; return 0 on failure, else 1. */
 int opt_intmax(const char *value, ossl_intmax_t *result)
--- test/params_conversion_test.c.orig	2017-10-10 15:19:45.611896396 +0100
+++ test/params_conversion_test.c	2017-10-17 15:42:25.304921197 +0100
@@ -15,7 +15,7 @@
 /* On machines that dont support <inttypes.h> just disable the tests */
 #if !defined(OPENSSL_NO_INTTYPES_H)
 
-# ifdef OPENSSL_SYS_VMS
+# if defined(OPENSSL_SYS_VMS) || defined(OPENSSL_SYS_AMIGAOS3) || defined(OPENSSL_SYS_AMIGAOS4)
 #  define strtoumax strtoull
 #  define strtoimax strtoll
 # endif
