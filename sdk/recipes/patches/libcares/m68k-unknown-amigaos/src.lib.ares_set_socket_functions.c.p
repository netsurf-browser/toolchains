--- src/lib/ares_set_socket_functions.c	2025-04-08 13:12:25.000000000 +0100
+++ src/lib/ares_set_socket_functions.c	2026-11-08 12:08:24.692580385 +0000
@@ -77,7 +77,7 @@
 #  define TFO_SUPPORTED 0
 #endif
 
-#ifndef HAVE_WRITEV
+#if 0
 /* Structure for scatter/gather I/O. */
 struct iovec {
   void  *iov_base; /* Pointer to data. */
@@ -171,7 +171,7 @@ static int setsocknonblock(ares_socket_t
 
   /* Amiga */
   long flags = nonblock ? 1L : 0L;
-  return IoctlSocket(sockfd, FIONBIO, flags);
+  return IoctlSocket(sockfd, FIONBIO, &flags);
 
 #elif defined(HAVE_SETSOCKOPT_SO_NONBLOCK)
 
