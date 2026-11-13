--- include/internal/sockets.h.orig	2018-06-03 03:19:29.000000000 +0200
+++ include/internal/sockets.h	2018-06-03 03:19:36.000000000 +0200
@@ -153,6 +153,16 @@
 #define OPENSSL_USE_IPV6 0
 #endif
 #endif
+/*
+ * We mean it
+ */
+#  if (OPENSSL_USE_IPV6 == 0)
+#   undef AF_INET6
+#   warning undef AF_INET6
+#   undef AF_UNIX
+#   warning undef AF_UNIX
+#   undef IPV6_V6ONLY
+#  endif
 
 /*
  * Some platforms define AF_UNIX, but don't support it
