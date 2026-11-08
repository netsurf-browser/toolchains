--- src/tools/adig.c	2025-04-08 13:12:25.000000000 +0100
+++ src/tools/adig.c	2026-11-08 12:49:33.056673509 +0000
@@ -26,6 +26,9 @@
  */
 #include "ares_setup.h"
 
+#include <proto/exec.h>
+#include <proto/bsdsocket.h>
+
 #ifdef HAVE_NETINET_IN_H
 #  include <netinet/in.h>
 #endif
@@ -56,6 +59,8 @@
 #  define PATH_MAX 1024
 #endif
 
+struct Library *SocketBase = NULL;
+
 typedef struct {
   unsigned short port;
   size_t         tries;
@@ -889,7 +894,7 @@ static int event_loop(ares_channel_t *ch
     if (tvp == NULL) {
       break;
     }
-    count = select(nfds, &read_fds, &write_fds, NULL, tvp);
+    count = WaitSelect(nfds, &read_fds, &write_fds, NULL, tvp, NULL);
     if (count < 0) {
 #ifdef USE_WINSOCK
       int err = WSAGetLastError();
@@ -1486,6 +1491,8 @@ int main(int argc, char **argv)
   ares_status_t   status;
   int             rv = 0;
 
+  SocketBase = OpenLibrary("bsdsocket.library", 4);
+
 #ifdef USE_WINSOCK
   WORD    wVersionRequested = MAKEWORD(USE_WINSOCK, USE_WINSOCK);
   WSADATA wsaData;
@@ -1568,6 +1575,8 @@ done:
   ares_destroy(channel);
   ares_library_cleanup();
 
+  CloseLibrary(SocketBase);
+
 #ifdef USE_WINSOCK
   WSACleanup();
 #endif
