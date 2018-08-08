--- crypto/bio/bio_lcl.h.orig	2017-10-17 16:19:36.908150231 +0100
+++ crypto/bio/bio_lcl.h	2017-10-17 16:19:52.056631064 +0100
@@ -36,7 +36,7 @@
 /*
  * Undefine AF_UNIX on systems that define it but don't support it.
  */
-# if defined(OPENSSL_SYS_WINDOWS) || defined(OPENSSL_SYS_VMS)
+# if defined(OPENSSL_SYS_WINDOWS) || defined(OPENSSL_SYS_VMS) || defined(OPENSSL_SYS_RISCOS) || defined(OPENSSL_SYS_AMIGAOS3)  || defined(OPENSSL_SYS_AMIGAOS4)
 #  undef AF_UNIX
 # endif
 
