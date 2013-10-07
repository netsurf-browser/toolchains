--- src/Hermes.h.orig	2013-10-07 22:55:59.000000000 +0200
+++ src/Hermes.h	2013-10-07 21:41:20.000000000 +0200
@@ -17,11 +17,11 @@
 #include "H_Config.h"
 
 /* Initialise Hermes, returns 0 if failed */
-HERMES_C int  HERMES_API Hermes_Init();
+HERMES_C int  HERMES_API Hermes_Init(void);
 
 
 /* Deinitialise Hermes, returns 0 if failed */
-HERMES_C int HERMES_API Hermes_Done();
+HERMES_C int HERMES_API Hermes_Done(void);
 
 
 #endif
