--- src/lib/ares_private.h.orig
+++ src/lib/ares_private.h
@@ -40,6 +40,10 @@
 #  include <netinet/in.h>
 #endif
 
+extern int strcasecmp(const char *s1, const char *s2);
+extern int strncasecmp(const char *s1, const char *s2, size_t n);
+extern int snprintf(char *str, size_t size, const char *format, ...);
+
 #include "ares_mem.h"
 #include "ares_ipv6.h"
 #include "util/ares_math.h"
      --- src/tools/ahost.c.orig
+++ src/tools/ahost.c
@@ -37,6 +37,8 @@
 #  include <strings.h>
 #endif
 
+extern char *strdup(const char *s);
+
 #include "ares.h"
 #include "ares_dns.h"
 #include "ares_getopt.h"
--- src/tools/adig.c
+++ src/tools/adig.c
@@ -42,6 +42,9 @@
 #  include <strings.h>
 #endif
 
+extern int snprintf(char *str, size_t size, const char *format, ...);
+extern char *strdup(const char *s);
+
 #include "ares.h"
 #include "ares_array.h"
 #include "ares_buf.h"
