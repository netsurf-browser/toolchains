--- e_os.h.orig	2018-06-03 03:19:29.000000000 +0200
+++ e_os.h	2018-06-03 03:19:36.000000000 +0200
@@ -462,6 +462,15 @@
 #    define OPENSSL_USE_IPV6 0
 #   endif
 #  endif
+/*
+ * We mean it
+ */
+#  if (OPENSSL_USE_IPV6 == 0)
+#   undef AF_INET6
+#   warning undef AF_INET6
+#   undef AF_UNIX
+#   warning undef AF_UNIX
+#  endif
 
 # endif
 
