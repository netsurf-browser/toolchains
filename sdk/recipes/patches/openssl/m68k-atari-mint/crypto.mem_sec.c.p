--- crypto/mem_sec.c.orig	2018-06-03 03:52:26.000000000 +0200
+++ crypto/mem_sec.c	2018-06-03 03:53:35.000000000 +0200
@@ -24,7 +24,7 @@
 
 #include <string.h>
 
-#if ! defined(OPENSSL_SYS_RISCOS) && ! defined(OPENSSL_SYS_AMIGAOS3) && ! defined(OPENSSL_SYS_AMIGAOS4) && (defined(OPENSSL_SYS_LINUX) || defined(OPENSSL_SYS_UNIX))
+#if ! defined(OPENSSL_SYS_RISCOS) && ! defined(OPENSSL_SYS_AMIGAOS3) && ! defined(OPENSSL_SYS_AMIGAOS4) && !defined(OPENSSL_SYS_MINT) && (defined(OPENSSL_SYS_LINUX) || defined(OPENSSL_SYS_UNIX))
 # define IMPLEMENTED
 # include <stdlib.h>
 # include <assert.h>
