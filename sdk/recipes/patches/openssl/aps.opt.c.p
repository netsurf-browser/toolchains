--- apps/opt.c.orig	2017-11-22 08:42:07.605882648 +0000
+++ apps/opt.c	2017-11-22 08:48:31.712843519 +0000
@@ -353,6 +353,11 @@
 #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L && \
     defined(INTMAX_MAX) && defined(UINTMAX_MAX)
 
+#if defined(OPENSSL_SYS_AMIGAOS3) || defined(OPENSSL_SYS_AMIGAOS4)
+# define strtoimax strtol
+# define strtoumax strtoul
+#endif
+
 /* Parse an intmax_t, put it into *result; return 0 on failure, else 1. */
 int opt_imax(const char *value, intmax_t *result)
 {
