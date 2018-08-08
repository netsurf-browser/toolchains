--- srclib/getprogname.c	2017-01-01 23:02:21.000000000 +0000
+++ srclib/getprogname.c	2017-03-26 23:21:25.532012988 +0100
@@ -144,7 +144,7 @@ getprogname (void)
     }
   return p;
 # else
-#  error "getprogname module not ported to this OS"
+  const char *p = ""; /* this is never used for anything important */
 # endif
 }
 
