--- e_os.h.orig	2019-02-13 14:25:17.725058588 +0000
+++ e_os.h	2019-02-13 14:35:39.790029062 +0000
@@ -399,6 +399,10 @@
 # ifndef OPENSSL_NO_SECURE_MEMORY
    /* unistd.h defines _POSIX_VERSION */
 #  if (defined(OPENSSL_SYS_UNIX) \
+        && !defined(OPENSSL_SYS_RISCOS) \
+        && !defined(OPENSSL_SYS_AMIGAOS3) \
+        && !defined(OPENSSL_SYS_AMIGAOS4) \
+        && !defined(OPENSSL_SYS_MINT) \
         && ( (defined(_POSIX_VERSION) && _POSIX_VERSION >= 200112L)      \
              || defined(__sun) || defined(__hpux) || defined(__sgi)      \
              || defined(__osf__) )) \
