--- crypto/rand/rand_unix.c.orig	2017-11-22 11:25:03.096636507 +0000
+++ crypto/rand/rand_unix.c	2017-11-22 11:25:52.649406308 +0000
@@ -15,7 +15,7 @@
 #include <openssl/rand.h>
 #include "rand_lcl.h"
 
-#if !(defined(OPENSSL_SYS_WINDOWS) || defined(OPENSSL_SYS_WIN32) || defined(OPENSSL_SYS_VMS) || defined(OPENSSL_SYS_VXWORKS) || defined(OPENSSL_SYS_UEFI))
+#if !(defined(OPENSSL_SYS_WINDOWS) || defined(OPENSSL_SYS_WIN32) || defined(OPENSSL_SYS_VMS) || defined(OPENSSL_SYS_VXWORKS) || defined(OPENSSL_SYS_UEFI) || defined(OPENSSL_SYS_AMIGAOS3) || defined(OPENSSL_SYS_AMIGAOS4))
 
 # include <sys/types.h>
 # include <sys/time.h>
