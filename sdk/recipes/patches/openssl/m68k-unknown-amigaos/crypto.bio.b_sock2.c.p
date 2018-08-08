--- crypto/bio/b_sock2.c.orig	2017-11-23 12:39:17.912640213 +0000
+++ crypto/bio/b_sock2.c	2017-11-23 12:41:56.350294762 +0000
@@ -94,6 +94,7 @@
         }
     }
 
+#if defined(TCP_NODELAY)
     if (options & BIO_SOCK_NODELAY) {
         if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) != 0) {
             SYSerr(SYS_F_SETSOCKOPT, get_last_socket_error());
@@ -101,6 +102,7 @@
             return 0;
         }
     }
+#endif
 
     if (connect(sock, BIO_ADDR_sockaddr(addr),
                 BIO_ADDR_sockaddr_size(addr)) == -1) {
--- crypto/bio/b_sock2.c.orig	2017-11-23 12:43:56.318461652 +0000
+++ crypto/bio/b_sock2.c	2017-11-23 12:44:36.139861658 +0000
@@ -193,6 +193,7 @@
         }
     }
 
+#if defined(TCP_NODELAY)
     if (options & BIO_SOCK_NODELAY) {
         if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) != 0) {
             SYSerr(SYS_F_SETSOCKOPT, get_last_socket_error());
@@ -200,6 +201,7 @@
             return 0;
         }
     }
+#endif
 
 # ifdef IPV6_V6ONLY
     if ((options & BIO_SOCK_V6_ONLY) && BIO_ADDR_family(addr) == AF_INET6) {
