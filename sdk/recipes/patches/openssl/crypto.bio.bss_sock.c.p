--- crypto/bio/bss_sock.c.orig	2026-10-13 15:36:12.358874893 +0100
+++ crypto/bio/bss_sock.c	2026-10-13 15:36:30.875007390 +0100
@@ -320,7 +320,7 @@
         || err == EAGAIN
         || err == ENOTCONN
         || err == EINTR
-#if !defined(__DJGPP__) && !defined(OPENSSL_SYS_TANDEM)
+#if !defined(__DJGPP__) && !defined(OPENSSL_SYS_TANDEM) && !defined(OPENSSL_SYS_RISCOS)
         || err == EPROTO
 #endif
 #ifdef __FreeBSD__
