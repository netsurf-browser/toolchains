--- libssp/ssp.c.orig	2025-11-13 15:51:26.412000000 +0000
+++ libssp/ssp.c	2025-11-13 15:51:53.320000000 +0000
@@ -66,6 +66,7 @@ see the files COPYING3 and COPYING.RUNTI
 #ifdef HAVE_SYSLOG_H
 # include <syslog.h>
 #endif
+#include <stdlib.h>
 
 void *__stack_chk_guard = 0;
 
