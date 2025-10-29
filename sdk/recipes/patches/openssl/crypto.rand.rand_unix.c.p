--- providers/implementations/rands/seeding/rand_unix.c.orig	2017-11-22 11:25:03.096636507 +0000
+++ providers/implementations/rands/seeding/rand_unix.c	2017-11-22 11:25:52.649406308 +0000
@@ -109,7 +109,8 @@
 
 #if !(defined(OPENSSL_SYS_WINDOWS) || defined(OPENSSL_SYS_WIN32) \
     || defined(OPENSSL_SYS_VMS) || defined(OPENSSL_SYS_VXWORKS) \
-    || defined(OPENSSL_SYS_UEFI))
+    || defined(OPENSSL_SYS_UEFI) || defined(OPENSSL_SYS_AMIGAOS3) \
+    || defined(OPENSSL_SYS_AMIGAOS4))
 
 # if defined(OPENSSL_SYS_VOS)
 
