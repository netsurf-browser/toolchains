--- lib/curl_setup.h.orig	2017-10-10 15:19:45.611896396 +0100
+++ lib/curl_setup.h	2017-10-17 15:42:25.304921197 +0100
@@ -314,7 +314,6 @@
  * In clib2 arpa/inet.h warns that some prototypes may clash
  * with bsdsocket.library. This avoids the definition of those.
  */
-#  define __NO_NET_API
 #endif
 
 #include <stdio.h>
