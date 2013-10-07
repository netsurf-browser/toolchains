--- src/Factory.c.orig	2013-10-07 23:01:32.000000000 +0200
+++ src/Factory.c	2013-10-07 21:54:56.000000000 +0200
@@ -21,12 +21,13 @@
 
 
 /* disable visual c++ warning */
-#if (_MSC_VER>=1000)
-#pragma warning( disable : 4113 )
+#if defined(_MSC_VER)
+# if (_MSC_VER>=1000)
+#  pragma warning( disable : 4113 )
+# endif
 #endif
 
 
-
 int processor;
 
 
