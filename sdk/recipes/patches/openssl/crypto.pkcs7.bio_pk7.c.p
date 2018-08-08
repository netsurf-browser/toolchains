--- crypto/pkcs7/bio_pk7.c.orig	2017-11-22 07:58:17.501346816 +0000
+++ crypto/pkcs7/bio_pk7.c	2017-11-22 08:00:09.130073671 +0000
@@ -12,7 +12,11 @@
 #include <openssl/bio.h>
 
 #if !defined(OPENSSL_SYS_VXWORKS)
-# include <memory.h>
+# if !defined(OPENSSL_SYS_AMIGAOS3) && !defined(OPENSSL_SYS_AMIGAOS4)
+#  include <memory.h>
+# else
+#  include <string.h>
+# endif
 #endif
 #include <stdio.h>
 
