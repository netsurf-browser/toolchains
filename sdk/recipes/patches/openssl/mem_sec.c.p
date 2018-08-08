--- crypto/mem_sec.c.orig	2017-10-17 16:46:24.923285526 +0100
+++ crypto/mem_sec.c	2017-10-17 16:48:00.358325267 +0100
@@ -24,7 +24,7 @@
 
 #include <string.h>
 
-#if defined(OPENSSL_SYS_LINUX) || defined(OPENSSL_SYS_UNIX)
+#if ! defined(OPENSSL_SYS_RISCOS) && ! defined(OPENSSL_SYS_AMIGAOS3) && ! defined(OPENSSL_SYS_AMIGAOS4) && (defined(OPENSSL_SYS_LINUX) || defined(OPENSSL_SYS_UNIX))
 # define IMPLEMENTED
 # include <stdlib.h>
 # include <assert.h>
