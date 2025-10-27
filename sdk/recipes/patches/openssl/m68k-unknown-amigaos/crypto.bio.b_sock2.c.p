--- crypto/bio/bio_sock2.c.orig	2019-02-15 10:07:24.181612545 +0000
+++ crypto/bio/bio_sock2.c	2019-02-15 10:07:15.637654488 +0000
@@ -98,6 +98,7 @@
         }
     }
 
+#if defined(TCP_NODELAY)
     if (options & BIO_SOCK_NODELAY) {
         if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
                        (const void *)&on, sizeof(on)) != 0) {
@@ -107,6 +108,7 @@
             return 0;
         }
     }
+#endif
 
     if (connect(sock, BIO_ADDR_sockaddr(addr),
                 BIO_ADDR_sockaddr_size(addr)) == -1) {
@@ -252,6 +254,7 @@
         }
     }
 
+#if defined(TCP_NODELAY)
     if (options & BIO_SOCK_NODELAY) {
         if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
                        (const void *)&on, sizeof(on)) != 0) {
@@ -261,6 +264,7 @@
             return 0;
         }
     }
+#endif
 
   /* On OpenBSD it is always ipv6 only with ipv6 sockets thus read-only */
 # if defined(IPV6_V6ONLY) && !defined(__OpenBSD__)
