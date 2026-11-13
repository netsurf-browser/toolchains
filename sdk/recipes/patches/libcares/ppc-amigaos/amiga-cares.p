From a8864a85357cb4e0d490b87ee732404b71c23cb6 Mon Sep 17 00:00:00 2001
From: Chris Young <chris@unsatisfactorysoftware.co.uk>
Date: Wed, 9 Oct 2024 14:14:29 +0100
Subject: [PATCH 01/11] Use bsdsocket.library ObtainDomainNameServerList() This
 is the native Roadshow (may also be available in other Amiga TCP/IP stacks)
 function for obtaining the system's list of DNS server addresses.

---
 src/lib/ares_sysconfig.c | 87 ++++++++++++++++++++++++++++++++++++++++
 1 file changed, 87 insertions(+)

diff --git src/lib/ares_sysconfig.c src/lib/ares_sysconfig.c
index 9f0d7e5061..d30bddedec 100644
--- src/lib/ares_sysconfig.c
+++ src/lib/ares_sysconfig.c
@@ -51,6 +51,14 @@
 #  define MAX_DNS_PROPERTIES   8
 #endif
 
+#if defined(__amigaos4__)
+#  include <proto/exec.h>
+#  include <exec/lists.h>
+#  include <exec/nodes.h>
+#  include <proto/bsdsocket.h>
+#  include <bsdsocket/socketbasetags.h>
+#endif
+
 #if defined(CARES_USE_LIBRESOLV)
 #  include <resolv.h>
 #endif
@@ -169,6 +177,83 @@ static ares_status_t ares_init_sysconfig_riscos(const ares_channel_t *channel,
 }
 #endif
 
+#if defined (__amigaos4__)
+static ares_status_t ares_init_sysconfig_amiga(const ares_channel_t *channel,
+                                                ares_sysconfig_t     *sysconfig)
+{
+  size_t        i;
+  ares_status_t status;
+
+  struct SocketIFace *ISocket;
+  struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
+
+  struct Library *base = IExec->OpenLibrary("bsdsocket.library", 4);
+
+  if(base) {
+    ISocket = (struct SocketIFace *)IExec->GetInterface(base, "main", 1, NULL);
+    if(ISocket) {
+      ULONG dnsapi = 0;
+
+      ISocket->SocketBaseTags(SBTM_GETREF(SBTC_HAVE_DNS_API), (ULONG)&dnsapi,
+                     TAG_DONE);
+
+      if(dnsapi) {
+        /* Use the DNS API in bsdsocket.library
+         * This exists in Roadshow
+         */
+        
+        struct List *dnslist = ISocket->ObtainDomainNameServerList();
+        struct DomainNameServerNode *node;
+        struct DomainNameServerNode *nnode;
+                
+        if(dnslist == NULL) {
+          IExec->DropInterface(ISocket);
+          IExec->CloseLibrary(base);
+          return ARES_ENOMEM;
+        }
+        
+        if(IsListEmpty(dnslist)) {
+          /* No DNS entries */
+          IExec->DropInterface(ISocket);
+          IExec->CloseLibrary(base);
+          return ARES_SUCCESS;
+        }
+        
+        node = (struct DomainNameServerNode *)IExec->GetHead(dnslist);
+
+        do {
+          nnode = (struct DomainNameServerNode *)IExec->GetSucc((struct MinNode *)node);
+          
+          status = ares_sconfig_append_fromstr(channel, &sysconfig->sconfig, node->dnsn_Address,
+                                           ARES_TRUE);
+
+          if (status != ARES_SUCCESS) {
+            IExec->DropInterface(ISocket);
+            IExec->CloseLibrary(base);
+            return status;
+          }
+    
+        } while((node = nnode));
+
+        ISocket->ReleaseDomainNameServerList(dnslist);
+        
+      } else {
+        /* DNS API does not exist */
+        IExec->DropInterface(ISocket);
+        IExec->CloseLibrary(base);
+        return ARES_ENOMEM;
+      }
+
+      IExec->DropInterface(ISocket);
+    }
+
+    IExec->CloseLibrary(base);
+  }
+
+  return ARES_SUCCESS;
+}
+#endif
+
 #if defined(WATT32)
 static ares_status_t ares_init_sysconfig_watt32(const ares_channel_t *channel,
                                                 ares_sysconfig_t     *sysconfig)
@@ -508,6 +593,8 @@ ares_status_t ares_init_by_sysconfig(ares_channel_t *channel)
   status = ares_init_sysconfig_mvs(channel, &sysconfig);
 #elif defined(__riscos__)
   status = ares_init_sysconfig_riscos(channel, &sysconfig);
+#elif defined(__amigaos4__)
+  status = ares_init_sysconfig_amiga(channel, &sysconfig);
 #elif defined(WATT32)
   status = ares_init_sysconfig_watt32(channel, &sysconfig);
 #elif defined(ANDROID) || defined(__ANDROID__)

From a89d47d90466fa1b29e4528552c7b45fa20532dc Mon Sep 17 00:00:00 2001
From: Chris Young <chris@unsatisfactorysoftware.co.uk>
Date: Wed, 11 Nov 2026 13:22:42 +0000
Subject: [PATCH 02/11] IoctlSocket() takes pointer to flags

---
 src/lib/ares_set_socket_functions.c | 2 +-
 1 file changed, 1 insertion(+), 1 deletion(-)

diff --git src/lib/ares_set_socket_functions.c src/lib/ares_set_socket_functions.c
index 615fbfa28b..742faf5e3a 100644
--- src/lib/ares_set_socket_functions.c
+++ src/lib/ares_set_socket_functions.c
@@ -174,7 +174,7 @@ static int setsocknonblock(ares_socket_t sockfd, /* operate on this */
 
   /* Amiga */
   long flags = nonblock ? 1L : 0L;
-  return IoctlSocket(sockfd, FIONBIO, flags);
+  return IoctlSocket(sockfd, FIONBIO, &flags);
 
 #elif defined(HAVE_SETSOCKOPT_SO_NONBLOCK)
 

From c6466e8ea769b7e6a409ecd1bbe8c3d1b184f135 Mon Sep 17 00:00:00 2001
From: Chris Young <chris@unsatisfactorysoftware.co.uk>
Date: Wed, 11 Nov 2026 15:41:56 +0000
Subject: [PATCH 03/11] bsdsocket.library should be opened by the calling
 program, not the lib itself

---
 src/lib/ares_sysconfig.c | 106 ++++++++++++++++-----------------------
 src/tools/adig.c         |  23 +++++++++
 src/tools/ahost.c        |  23 +++++++++
 3 files changed, 88 insertions(+), 64 deletions(-)

diff --git src/lib/ares_sysconfig.c src/lib/ares_sysconfig.c
index 627958329c..87a17354ca 100644
--- src/lib/ares_sysconfig.c
+++ src/lib/ares_sysconfig.c
@@ -184,70 +184,48 @@ static ares_status_t ares_init_sysconfig_amiga(const ares_channel_t *channel,
   size_t        i;
   ares_status_t status;
 
-  struct SocketIFace *ISocket;
-  struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
-
-  struct Library *base = IExec->OpenLibrary("bsdsocket.library", 4);
-
-  if(base) {
-    ISocket = (struct SocketIFace *)IExec->GetInterface(base, "main", 1, NULL);
-    if(ISocket) {
-      ULONG dnsapi = 0;
-
-      ISocket->SocketBaseTags(SBTM_GETREF(SBTC_HAVE_DNS_API), (ULONG)&dnsapi,
-                     TAG_DONE);
-
-      if(dnsapi) {
-        /* Use the DNS API in bsdsocket.library
-         * This exists in Roadshow
-         */
-        
-        struct List *dnslist = ISocket->ObtainDomainNameServerList();
-        struct DomainNameServerNode *node;
-        struct DomainNameServerNode *nnode;
-                
-        if(dnslist == NULL) {
-          IExec->DropInterface(ISocket);
-          IExec->CloseLibrary(base);
-          return ARES_ENOMEM;
-        }
-        
-        if(IsListEmpty(dnslist)) {
-          /* No DNS entries */
-          IExec->DropInterface(ISocket);
-          IExec->CloseLibrary(base);
-          return ARES_SUCCESS;
-        }
-        
-        node = (struct DomainNameServerNode *)IExec->GetHead(dnslist);
-
-        do {
-          nnode = (struct DomainNameServerNode *)IExec->GetSucc((struct MinNode *)node);
-          
-          status = ares_sconfig_append_fromstr(channel, &sysconfig->sconfig, node->dnsn_Address,
-                                           ARES_TRUE);
-
-          if (status != ARES_SUCCESS) {
-            IExec->DropInterface(ISocket);
-            IExec->CloseLibrary(base);
-            return status;
-          }
-    
-        } while((node = nnode));
-
-        ISocket->ReleaseDomainNameServerList(dnslist);
-        
-      } else {
-        /* DNS API does not exist */
-        IExec->DropInterface(ISocket);
-        IExec->CloseLibrary(base);
-        return ARES_ENOMEM;
-      }
-
-      IExec->DropInterface(ISocket);
-    }
-
-    IExec->CloseLibrary(base);
+  ULONG dnsapi = 0;
+
+  SocketBaseTags(SBTM_GETREF(SBTC_HAVE_DNS_API), (ULONG)&dnsapi,
+				 TAG_DONE);
+
+  if(dnsapi) {
+	/* Use the DNS API in bsdsocket.library
+	 * This exists in Roadshow
+	 */
+	
+	struct List *dnslist = ObtainDomainNameServerList();
+	struct DomainNameServerNode *node;
+	struct DomainNameServerNode *nnode;
+			
+	if(dnslist == NULL) {
+	  return ARES_ENOMEM;
+	}
+	
+	if(IsListEmpty(dnslist)) {
+	  /* No DNS entries */
+	  return ARES_SUCCESS;
+	}
+	
+	node = (struct DomainNameServerNode *)GetHead(dnslist);
+
+	do {
+	  nnode = (struct DomainNameServerNode *)GetSucc((struct MinNode *)node);
+	  
+	  status = ares_sconfig_append_fromstr(channel, &sysconfig->sconfig, node->dnsn_Address,
+									   ARES_TRUE);
+
+	  if (status != ARES_SUCCESS) {
+		return status;
+	  }
+
+	} while((node = nnode));
+
+	ReleaseDomainNameServerList(dnslist);
+	
+  } else {
+	/* DNS API does not exist */
+	return ARES_ENOMEM;
   }
 
   return ARES_SUCCESS;
diff --git src/tools/adig.c src/tools/adig.c
index 220ba9634c..a52fef06c1 100644
--- src/tools/adig.c
+++ src/tools/adig.c
@@ -26,6 +26,16 @@
  */
 #include "ares_setup.h"
 
+#if defined(__amigaos4__)
+#  include <proto/exec.h>
+#  include <exec/lists.h>
+#  include <exec/nodes.h>
+#  include <proto/bsdsocket.h>
+#  include <bsdsocket/socketbasetags.h>
+
+#  define select(A,B,C,D,E) WaitSelect(A,B,C,D,E,NULL)
+#endif
+
 #ifdef HAVE_NETINET_IN_H
 #  include <netinet/in.h>
 #endif
@@ -1506,6 +1516,14 @@ int main(int argc, char **argv)
   WSAStartup(wVersionRequested, &wsaData);
 #endif
 
+#if defined(__amigaos4__)
+  struct Library *SocketBase = OpenLibrary("bsdsocket.library", 4);
+
+  if(!SocketBase) return 0;
+  ISocket = (struct SocketIFace *)GetInterface(base, "main", 1, NULL);
+  if(ISocket) return 0;
+#endif
+
   status = (ares_status_t)ares_library_init(ARES_LIB_INIT_ALL);
   if (status != ARES_SUCCESS) {
     fprintf(stderr, "ares_library_init: %s\n", ares_strerror((int)status));
@@ -1584,6 +1602,11 @@ int main(int argc, char **argv)
   ares_destroy(channel);
   ares_library_cleanup();
 
+#if defined(__amigaos4__)
+  if(ISocket) DropInterface(ISocket);
+  if(SocketBase) CloseLibrary(SocketBase);
+#endif
+
 #ifdef USE_WINSOCK
   WSACleanup();
 #endif
diff --git src/tools/ahost.c src/tools/ahost.c
index 1bd6da17b4..03361d8cc7 100644
--- src/tools/ahost.c
+++ src/tools/ahost.c
@@ -27,6 +27,16 @@
 
 #include "ares_setup.h"
 
+#if defined(__amigaos4__)
+#  include <proto/exec.h>
+#  include <exec/lists.h>
+#  include <exec/nodes.h>
+#  include <proto/bsdsocket.h>
+#  include <bsdsocket/socketbasetags.h>
+
+#  define select(A,B,C,D,E) WaitSelect(A,B,C,D,E,NULL)
+#endif
+
 #if !defined(_WIN32) || defined(WATT32)
 #  include <netinet/in.h>
 #  include <arpa/inet.h>
@@ -81,6 +91,14 @@ int main(int argc, char **argv)
   WSAStartup(wVersionRequested, &wsaData);
 #endif
 
+#if defined(__amigaos4__)
+  struct Library *SocketBase = OpenLibrary("bsdsocket.library", 4);
+
+  if(!SocketBase) return 0;
+  ISocket = (struct SocketIFace *)GetInterface(base, "main", 1, NULL);
+  if(ISocket) return 0;
+#endif
+
   memset(&options, 0, sizeof(options));
 
   status = ares_library_init(ARES_LIB_INIT_ALL);
@@ -200,6 +218,11 @@ int main(int argc, char **argv)
 
   ares_library_cleanup();
 
+#if defined(__amigaos4__)
+  if(ISocket) DropInterface(ISocket);
+  if(SocketBase) CloseLibrary(SocketBase);
+#endif
+
 #ifdef USE_WINSOCK
   WSACleanup();
 #endif

From 3f56b99cf3011af44bd36eaac6389ef48a706040 Mon Sep 17 00:00:00 2001
From: Chris Young <chris@unsatisfactorysoftware.co.uk>
Date: Wed, 11 Nov 2026 15:54:22 +0000
Subject: [PATCH 04/11] Make Amiga code generic to OS3 and OS4

---
 src/lib/ares_sysconfig.c |  6 +++---
 src/tools/adig.c         | 10 +++++++---
 src/tools/ahost.c        | 10 +++++++---
 3 files changed, 17 insertions(+), 9 deletions(-)

diff --git src/lib/ares_sysconfig.c src/lib/ares_sysconfig.c
index 87a17354ca..5b385a62b8 100644
--- src/lib/ares_sysconfig.c
+++ src/lib/ares_sysconfig.c
@@ -51,7 +51,7 @@
 #  define MAX_DNS_PROPERTIES   8
 #endif
 
-#if defined(__amigaos4__)
+#if defined(__amigaos__) || defined(__amigaos4__)
 #  include <proto/exec.h>
 #  include <exec/lists.h>
 #  include <exec/nodes.h>
@@ -177,7 +177,7 @@ static ares_status_t ares_init_sysconfig_riscos(const ares_channel_t *channel,
 }
 #endif
 
-#if defined (__amigaos4__)
+#if defined(__amigaos__) || defined(__amigaos4__)
 static ares_status_t ares_init_sysconfig_amiga(const ares_channel_t *channel,
                                                 ares_sysconfig_t     *sysconfig)
 {
@@ -714,7 +714,7 @@ ares_status_t ares_init_by_sysconfig(ares_channel_t *channel)
   status = ares_init_sysconfig_mvs(channel, &sysconfig);
 #elif defined(__riscos__)
   status = ares_init_sysconfig_riscos(channel, &sysconfig);
-#elif defined(__amigaos4__)
+#elif defined(__amigaos__) || defined(__amigaos4__)
   status = ares_init_sysconfig_amiga(channel, &sysconfig);
 #elif defined(WATT32)
   status = ares_init_sysconfig_watt32(channel, &sysconfig);
diff --git src/tools/adig.c src/tools/adig.c
index a52fef06c1..c54a581cda 100644
--- src/tools/adig.c
+++ src/tools/adig.c
@@ -26,7 +26,7 @@
  */
 #include "ares_setup.h"
 
-#if defined(__amigaos4__)
+#if defined(__amigaos__) || defined(__amigaos4__)
 #  include <proto/exec.h>
 #  include <exec/lists.h>
 #  include <exec/nodes.h>
@@ -1516,12 +1516,14 @@ int main(int argc, char **argv)
   WSAStartup(wVersionRequested, &wsaData);
 #endif
 
-#if defined(__amigaos4__)
+#if defined(__amigaos__) || defined(__amigaos4__)
   struct Library *SocketBase = OpenLibrary("bsdsocket.library", 4);
 
   if(!SocketBase) return 0;
+#ifdef __amigaos4__
   ISocket = (struct SocketIFace *)GetInterface(base, "main", 1, NULL);
   if(ISocket) return 0;
+#endif
 #endif
 
   status = (ares_status_t)ares_library_init(ARES_LIB_INIT_ALL);
@@ -1602,10 +1604,12 @@ int main(int argc, char **argv)
   ares_destroy(channel);
   ares_library_cleanup();
 
-#if defined(__amigaos4__)
+#if defined(__amigaos__) || defined(__amigaos4__)
+#ifdef __amigaos4__
   if(ISocket) DropInterface(ISocket);
   if(SocketBase) CloseLibrary(SocketBase);
 #endif
+#endif
 
 #ifdef USE_WINSOCK
   WSACleanup();
diff --git src/tools/ahost.c src/tools/ahost.c
index 03361d8cc7..380f4e69ed 100644
--- src/tools/ahost.c
+++ src/tools/ahost.c
@@ -27,7 +27,7 @@
 
 #include "ares_setup.h"
 
-#if defined(__amigaos4__)
+#if defined(__amigaos__) || defined(__amigaos4__)
 #  include <proto/exec.h>
 #  include <exec/lists.h>
 #  include <exec/nodes.h>
@@ -91,12 +91,14 @@ int main(int argc, char **argv)
   WSAStartup(wVersionRequested, &wsaData);
 #endif
 
-#if defined(__amigaos4__)
+#if defined(__amigaos__) || defined(__amigaos4__)
   struct Library *SocketBase = OpenLibrary("bsdsocket.library", 4);
 
   if(!SocketBase) return 0;
+#ifdef __amigaos4__
   ISocket = (struct SocketIFace *)GetInterface(base, "main", 1, NULL);
   if(ISocket) return 0;
+#endif
 #endif
 
   memset(&options, 0, sizeof(options));
@@ -218,10 +220,12 @@ int main(int argc, char **argv)
 
   ares_library_cleanup();
 
-#if defined(__amigaos4__)
+#if defined(__amigaos__) || defined(__amigaos4__)
+#ifdef __amigaos4__
   if(ISocket) DropInterface(ISocket);
   if(SocketBase) CloseLibrary(SocketBase);
 #endif
+#endif
 
 #ifdef USE_WINSOCK
   WSACleanup();

From 7e796a320a6255f3a5fe0e16c21a8d41a6691813 Mon Sep 17 00:00:00 2001
From: Chris Young <chris@unsatisfactorysoftware.co.uk>
Date: Wed, 11 Nov 2026 18:16:28 +0000
Subject: [PATCH 05/11] Define global SocketBase

---
 src/tools/adig.c  | 2 ++
 src/tools/ahost.c | 2 ++
 2 files changed, 4 insertions(+)

diff --git src/tools/adig.c src/tools/adig.c
index c54a581cda..64910690bf 100644
--- src/tools/adig.c
+++ src/tools/adig.c
@@ -34,6 +34,8 @@
 #  include <bsdsocket/socketbasetags.h>
 
 #  define select(A,B,C,D,E) WaitSelect(A,B,C,D,E,NULL)
+
+struct Library *SocketBase = NULL;
 #endif
 
 #ifdef HAVE_NETINET_IN_H
diff --git src/tools/ahost.c src/tools/ahost.c
index 380f4e69ed..fa5388513e 100644
--- src/tools/ahost.c
+++ src/tools/ahost.c
@@ -35,6 +35,8 @@
 #  include <bsdsocket/socketbasetags.h>
 
 #  define select(A,B,C,D,E) WaitSelect(A,B,C,D,E,NULL)
+
+struct Library *SocketBase = NULL;
 #endif
 
 #if !defined(_WIN32) || defined(WATT32)

From 038b70e5174bd178b34710dace06a61099b8d6b1 Mon Sep 17 00:00:00 2001
From: Chris Young <chris@unsatisfactorysoftware.co.uk>
Date: Wed, 11 Nov 2026 18:16:46 +0000
Subject: [PATCH 06/11] Define missing functions for AmigaOS3

---
 src/lib/ares_sysconfig.c | 25 +++++++++++++++++++++++++
 1 file changed, 25 insertions(+)

diff --git src/lib/ares_sysconfig.c src/lib/ares_sysconfig.c
index 5b385a62b8..1017fd6ed9 100644
--- src/lib/ares_sysconfig.c
+++ src/lib/ares_sysconfig.c
@@ -178,6 +178,31 @@ static ares_status_t ares_init_sysconfig_riscos(const ares_channel_t *channel,
 #endif
 
 #if defined(__amigaos__) || defined(__amigaos4__)
+#ifndef __amigaos4__
+static struct Node *GetHead(struct List *list)
+{
+	struct Node *res = NULL;
+
+	if ((NULL != list) && (NULL != list->lh_Head->ln_Succ))
+	{
+		res = list->lh_Head;
+	}
+	return res;
+}
+
+static struct Node *GetPred(struct Node *node)
+{
+	if (node->ln_Pred->ln_Pred == NULL) return NULL;
+	return node->ln_Pred;
+}
+
+static struct Node *GetSucc(struct Node *node)
+{
+	if (node->ln_Succ->ln_Succ == NULL) return NULL;
+	return node->ln_Succ;
+}
+#endif
+
 static ares_status_t ares_init_sysconfig_amiga(const ares_channel_t *channel,
                                                 ares_sysconfig_t     *sysconfig)
 {

From d680d0226b7a5e4444cae655b8672dcabb0a85b0 Mon Sep 17 00:00:00 2001
From: Chris Young <chris@unsatisfactorysoftware.co.uk>
Date: Wed, 11 Nov 2026 18:57:34 +0000
Subject: [PATCH 07/11] Free DNS entries on error

---
 src/lib/ares_sysconfig.c | 3 ++-
 1 file changed, 2 insertions(+), 1 deletion(-)

diff --git src/lib/ares_sysconfig.c src/lib/ares_sysconfig.c
index 1017fd6ed9..5916d1f7e6 100644
--- src/lib/ares_sysconfig.c
+++ src/lib/ares_sysconfig.c
@@ -241,6 +241,7 @@ static ares_status_t ares_init_sysconfig_amiga(const ares_channel_t *channel,
 									   ARES_TRUE);
 
 	  if (status != ARES_SUCCESS) {
+		ReleaseDomainNameServerList(dnslist);
 		return status;
 	  }
 
@@ -250,7 +251,7 @@ static ares_status_t ares_init_sysconfig_amiga(const ares_channel_t *channel,
 	
   } else {
 	/* DNS API does not exist */
-	return ARES_ENOMEM;
+	return ARES_SUCCESS;
   }
 
   return ARES_SUCCESS;

From 5aca13d6192931c0e2f3c8d7a59c0c67e3d89e45 Mon Sep 17 00:00:00 2001
From: Chris Young <chris@unsatisfactorysoftware.co.uk>
Date: Thu, 12 Nov 2026 09:39:12 +0000
Subject: [PATCH 08/11] Skip fcntl call on AmigaOS

---
 src/lib/ares_set_socket_functions.c | 2 +-
 1 file changed, 1 insertion(+), 1 deletion(-)

diff --git src/lib/ares_set_socket_functions.c src/lib/ares_set_socket_functions.c
index 742faf5e3a..029aae851c 100644
--- src/lib/ares_set_socket_functions.c
+++ src/lib/ares_set_socket_functions.c
@@ -217,7 +217,7 @@ static ares_socket_t default_asocket(int domain, int type, int protocol,
     goto fail; /* LCOV_EXCL_LINE */
   }
 
-#if defined(FD_CLOEXEC) && !defined(MSDOS)
+#if defined(FD_CLOEXEC) && !defined(MSDOS) && !defined(__amigaos__)
   /* Configure the socket fd as close-on-exec. */
   if (fcntl(s, F_SETFD, FD_CLOEXEC) != 0) {
     goto fail; /* LCOV_EXCL_LINE */

From 2f8077d7a8e99a00dda41b32953412c3ff6330cc Mon Sep 17 00:00:00 2001
From: Chris Young <chris@unsatisfactorysoftware.co.uk>
Date: Thu, 12 Nov 2026 10:14:26 +0000
Subject: [PATCH 09/11] AmigaOS has struct iovec

---
 src/lib/ares_set_socket_functions.c | 2 +-
 1 file changed, 1 insertion(+), 1 deletion(-)

diff --git src/lib/ares_set_socket_functions.c src/lib/ares_set_socket_functions.c
index 029aae851c..3c341a3698 100644
--- src/lib/ares_set_socket_functions.c
+++ src/lib/ares_set_socket_functions.c
@@ -78,7 +78,7 @@
 #  define TFO_SUPPORTED 0
 #endif
 
-#ifndef HAVE_WRITEV
+#if !defined(HAVE_WRITEV) && !defined(__amigaos__)
 /* Structure for scatter/gather I/O. */
 struct iovec {
   void  *iov_base; /* Pointer to data. */

From ddd1ccf5be7569844fd3416bbc50bc6fd29daa3b Mon Sep 17 00:00:00 2001
From: Chris Young <chris@unsatisfactorysoftware.co.uk>
Date: Thu, 12 Nov 2026 16:16:30 +0000
Subject: [PATCH 10/11] Fix the library base reference for AmigaOS4

---
 src/tools/adig.c  | 2 +-
 src/tools/ahost.c | 2 +-
 2 files changed, 2 insertions(+), 2 deletions(-)

diff --git src/tools/adig.c src/tools/adig.c
index 64910690bf..1338dd8734 100644
--- src/tools/adig.c
+++ src/tools/adig.c
@@ -1523,7 +1523,7 @@ int main(int argc, char **argv)
 
   if(!SocketBase) return 0;
 #ifdef __amigaos4__
-  ISocket = (struct SocketIFace *)GetInterface(base, "main", 1, NULL);
+  ISocket = (struct SocketIFace *)GetInterface(SocketBase, "main", 1, NULL);
   if(ISocket) return 0;
 #endif
 #endif
diff --git src/tools/ahost.c src/tools/ahost.c
index fa5388513e..ab70c6fa9f 100644
--- src/tools/ahost.c
+++ src/tools/ahost.c
@@ -98,7 +98,7 @@ int main(int argc, char **argv)
 
   if(!SocketBase) return 0;
 #ifdef __amigaos4__
-  ISocket = (struct SocketIFace *)GetInterface(base, "main", 1, NULL);
+  ISocket = (struct SocketIFace *)GetInterface(SocketBase, "main", 1, NULL);
   if(ISocket) return 0;
 #endif
 #endif

From ed8304a94b3dc473a968ae08bfc985ae1b017203 Mon Sep 17 00:00:00 2001
From: Chris Young <chris@unsatisfactorysoftware.co.uk>
Date: Thu, 12 Nov 2026 16:38:42 +0000
Subject: [PATCH 11/11] Define global interface for AmigaOS4

---
 src/tools/adig.c  | 3 +++
 src/tools/ahost.c | 3 +++
 2 files changed, 6 insertions(+)

diff --git src/tools/adig.c src/tools/adig.c
index 1338dd8734..3e6ef9555d 100644
--- src/tools/adig.c
+++ src/tools/adig.c
@@ -36,6 +36,9 @@
 #  define select(A,B,C,D,E) WaitSelect(A,B,C,D,E,NULL)
 
 struct Library *SocketBase = NULL;
+#    ifdef __amigaos4__
+struct SocketIFace *ISocket = NULL;
+#    endif
 #endif
 
 #ifdef HAVE_NETINET_IN_H
diff --git src/tools/ahost.c src/tools/ahost.c
index ab70c6fa9f..719158f5fc 100644
--- src/tools/ahost.c
+++ src/tools/ahost.c
@@ -37,6 +37,9 @@
 #  define select(A,B,C,D,E) WaitSelect(A,B,C,D,E,NULL)
 
 struct Library *SocketBase = NULL;
+#    ifdef __amigaos4__
+struct SocketIFace *ISocket = NULL;
+#    endif
 #endif
 
 #if !defined(_WIN32) || defined(WATT32)

From b798161643cd841348860861fbfa2a83136d4574 Mon Sep 17 00:00:00 2001
From: Chris Young <chris@unsatisfactorysoftware.co.uk>
Date: Fri, 13 Nov 2026 09:12:29 +0000
Subject: [PATCH] Don't quit on success

---
 src/tools/adig.c  | 2 +-
 src/tools/ahost.c | 2 +-
 2 files changed, 2 insertions(+), 2 deletions(-)

diff --git src/tools/adig.c src/tools/adig.c
index 3e6ef9555d..d44d65c61a 100644
--- src/tools/adig.c
+++ src/tools/adig.c
@@ -1527,7 +1527,7 @@ int main(int argc, char **argv)
   if(!SocketBase) return 0;
 #ifdef __amigaos4__
   ISocket = (struct SocketIFace *)GetInterface(SocketBase, "main", 1, NULL);
-  if(ISocket) return 0;
+  if(!ISocket) return 0;
 #endif
 #endif
 
diff --git src/tools/ahost.c src/tools/ahost.c
index 719158f5fc..c17ab9f519 100644
--- src/tools/ahost.c
+++ src/tools/ahost.c
@@ -102,7 +102,7 @@ int main(int argc, char **argv)
   if(!SocketBase) return 0;
 #ifdef __amigaos4__
   ISocket = (struct SocketIFace *)GetInterface(SocketBase, "main", 1, NULL);
-  if(ISocket) return 0;
+  if(!ISocket) return 0;
 #endif
 #endif

