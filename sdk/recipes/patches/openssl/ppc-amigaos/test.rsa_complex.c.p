--- test/rsa_complex.c.orig	2019-02-14 10:16:06.665734720 +0000
+++ test/rsa_complex.c	2019-02-14 10:16:18.581682332 +0000
@@ -16,11 +16,6 @@
  */
 
 #if !defined(__DJGPP__)
-# if defined(__STDC_VERSION__)
-#  if __STDC_VERSION__ >= 199901L
-#   include <complex.h>
-#  endif
-# endif
 # include <openssl/rsa.h>
 #endif
 #include <stdlib.h>
