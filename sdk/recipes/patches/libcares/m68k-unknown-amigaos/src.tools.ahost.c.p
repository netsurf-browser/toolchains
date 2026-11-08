--- src/tools/ahost.c	2025-04-08 13:12:25.000000000 +0100
+++ src/tools/ahost.c	2026-11-08 12:39:16.348650243 +0000
@@ -27,6 +27,9 @@
 
 #include "ares_setup.h"
 
+#include <proto/exec.h>
+#include <proto/bsdsocket.h>
+
 #if !defined(_WIN32) || defined(WATT32)
 #  include <netinet/in.h>
 #  include <arpa/inet.h>
@@ -44,6 +47,8 @@
 
 #include "ares_str.h"
 
+struct Library *SocketBase = NULL;
+
 static void callback(void *arg, int status, int timeouts, struct hostent *host);
 static void ai_callback(void *arg, int status, int timeouts,
                         struct ares_addrinfo *result);
@@ -68,6 +73,8 @@ int         main(int argc, char **argv)
   ares_getopt_state_t  state;
   char                *servers = NULL;
 
+  SocketBase = OpenLibrary("bsdsocket.library", 4);
+
 #ifdef USE_WINSOCK
   WORD    wVersionRequested = MAKEWORD(USE_WINSOCK, USE_WINSOCK);
   WSADATA wsaData;
@@ -182,7 +189,7 @@ int         main(int argc, char **argv)
     if (tvp == NULL) {
       break;
     }
-    res = select(nfds, &read_fds, &write_fds, NULL, tvp);
+    res = WaitSelect(nfds, &read_fds, &write_fds, NULL, tvp, NULL);
     if (-1 == res) {
       break;
     }
@@ -197,6 +204,8 @@ int         main(int argc, char **argv)
   WSACleanup();
 #endif
 
+  CloseLibrary(SocketBase);
+
   return 0;
 }
 
