--- apps/include/http_server.h.orig	2017-11-21 22:49:00.185608040 +0000
+++ apps/include/http_server.h	2017-11-21 22:49:21.488219518 +0000
@@ -27,7 +27,7 @@
 # endif
 
 # if !defined(NO_FORK) && !defined(OPENSSL_NO_SOCK) \
-    && !defined(OPENSSL_NO_POSIX_IO)
+    && !defined(OPENSSL_NO_POSIX_IO) && !defined(NO_SYSLOG)
 #  define HTTP_DAEMON
 #  include <sys/types.h>
 #  include <sys/wait.h>
