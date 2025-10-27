--- apps/speed.c.orig	2017-11-21 22:49:00.185608040 +0000
+++ apps/speed.c	2017-11-21 22:49:21.488219518 +0000
@@ -62,6 +62,12 @@
 #include "./testdsa.h"
 #include <openssl/modes.h>
 
+
+#if defined(OPENSSL_SYS_AMIGAOS3) || defined(OPENSSL_SYS_AMIGAOS4)
+# define HAVE_FORK 0
+# undef SIGALRM
+#endif
+
 #ifndef HAVE_FORK
 # if defined(OPENSSL_SYS_VMS) || defined(OPENSSL_SYS_WINDOWS) || defined(OPENSSL_SYS_VXWORKS)
 #  define HAVE_FORK 0
@@ -186,6 +192,18 @@
 
     return ret;
 }
+#elif defined(OPENSSL_SYS_AMIGAOS3) || defined(OPENSSL_SYS_AMIGAOS4)
+static void alarm_aos(unsigned int secs)
+{
+    (void) secs;
+}
+#define alarm alarm_aos
+
+static double Time_F(int s)
+{
+    double ret = app_tminterval(s, usertime);
+    return ret;
+}
 #else
 # error "SIGALRM not defined and the platform is not Windows"
 #endif
