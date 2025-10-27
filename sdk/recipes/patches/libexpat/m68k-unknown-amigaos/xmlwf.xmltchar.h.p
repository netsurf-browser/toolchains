--- xmlwf/xmltchar.h.orig	2017-10-10 15:19:45.611896396 +0100
+++ xmlwf/xmltchar.h	2017-10-17 15:42:25.304921197 +0100
@@ -75,5 +75,5 @@
 #  define tremove remove
 #  define tchar char
 #  define tcstof strtof
-#  define tcstoull strtoull
+#  define tcstoull strtoul
 #endif /* not XML_UNICODE */
