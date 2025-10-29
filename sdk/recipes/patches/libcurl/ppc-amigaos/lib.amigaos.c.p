--- lib/amigaos.c	2024-09-18 06:56:40.000000000 +0100
+++ lib/amigaos.c	2024-10-01 10:00:47.428000000 +0100
@@ -83,16 +83,6 @@ CURLcode Curl_amiga_init(void)
   if(base) {
     ISocket = (struct SocketIFace *)GetInterface(base, "main", 1, NULL);
     if(ISocket) {
-      ULONG enabled = 0;
-
-      SocketBaseTags(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
-                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
-                     TAG_DONE);
-
-      if(enabled) {
-        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
-      }
-
       __CurlISocket = ISocket;
 
       atexit(Curl_amiga_cleanup);

