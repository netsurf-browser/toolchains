--- lib/expat_external.h.orig	2026-10-14 00:15:42.736318138 +0100
+++ lib/expat_external.h	2026-10-14 00:20:52.316090830 +0100
@@ -128,9 +128,12 @@
 
 /* Marks a function that Expat still provides but that callers should move off
    of. */
-#  if defined(__clang__) || defined(__GNUC__)
+#  if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR >= 5)))
 #    define XML_ATTR_DEPRECATED(message)                                       \
       __attribute__((__deprecated__(message)))
+#  elif defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR < 5))
+#    define XML_ATTR_DEPRECATED(message)                                       \
+      __attribute__((__deprecated__))
 #  elif defined(_MSC_VER)
 #    define XML_ATTR_DEPRECATED(message) __declspec(deprecated(message))
 #  else
