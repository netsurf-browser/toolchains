--- apps/lib/s_socket.c.orig	2018-06-03 03:19:29.000000000 +0200
+++ apps/lib/s_socket.c	2018-06-03 03:19:36.000000000 +0200
@@ -179,7 +179,9 @@
                        BIO_ADDRINFO_family(res) == AF_INET6 ? "IPv6 " :
 #endif
                        BIO_ADDRINFO_family(res) == AF_INET ? "IPv4 " :
+#ifdef AF_UNIX
                        BIO_ADDRINFO_family(res) == AF_UNIX ? "unix " : "",
+#endif
                        bindhost != NULL ? bindhost : "",
                        bindport != NULL ? ":" : "",
                        bindport != NULL ? bindport : "");
