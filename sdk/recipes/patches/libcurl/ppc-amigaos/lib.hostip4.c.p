--- lib/hostip4.c	2017-03-26 23:41:52.304617207 +0100
+++ lib/hostip4.c	2017-03-26 23:41:11.000000000 +0100
@@ -119,6 +119,10 @@ Curl_addrinfo *Curl_getaddrinfo(struct c
  * implying that only threadsafe code and function calls may be used.
  *
  */
+#ifdef __amigaos4__
+#include <proto/exec.h>
+#include <proto/bsdsocket.h>
+#endif
 Curl_addrinfo *Curl_ipv4_resolve_r(const char *hostname,
                                    int port)
 {
@@ -129,6 +133,9 @@ Curl_addrinfo *Curl_ipv4_resolve_r(const
   struct hostent *h = NULL;
   struct in_addr in;
   struct hostent *buf = NULL;
+#ifdef __amigaos4__
+  struct SocketIFace *ISocket = NULL;
+#endif
 
   if(Curl_inet_pton(AF_INET, hostname, &in) > 0)
     /* This is a dotted IP address 123.123.123.123-style */
@@ -291,7 +298,20 @@ Curl_addrinfo *Curl_ipv4_resolve_r(const
      * gethostbyname() is the preferred one.
      */
   else {
+#ifdef __amigaos4__
+    struct Library *SocketBase = IExec->OpenLibrary("bsdsocket.library", 4);
+    if (SocketBase)
+    {
+       ISocket = (struct SocketIFace *)IExec->GetInterface(SocketBase, "main", 1, NULL);
+    }
+
+    if (ISocket)
+    {
+       h = ISocket->gethostbyname((void*)hostname);
+    }
+#else  
     h = gethostbyname((void *)hostname);
+#endif
 #endif /* HAVE_GETADDRINFO_THREADSAFE || HAVE_GETHOSTBYNAME_R */
   }
 
@@ -301,7 +321,14 @@ Curl_addrinfo *Curl_ipv4_resolve_r(const
     if(buf) /* used a *_r() function */
       free(buf);
   }
-
+#ifdef __amigaos4__
+  if (ISocket)
+  {
+     struct Library *SocketBase = ISocket->Data.LibBase;
+     IExec->DropInterface((struct Interface *)ISocket);
+     IExec->CloseLibrary(SocketBase);
+  }
+#endif 
   return ai;
 }
 #endif /* defined(CURLRES_IPV4) && !defined(CURLRES_ARES) */
