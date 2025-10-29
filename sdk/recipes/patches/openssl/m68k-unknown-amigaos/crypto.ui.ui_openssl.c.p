--- crypto/ui/ui_openssl.c.orig	2017-11-23 13:22:15.760078583 +0000
+++ crypto/ui/ui_openssl.c	2017-11-23 13:21:40.534382076 +0000
@@ -103,6 +103,18 @@
 #  undef SGTTY
 # endif
 
+# if defined(OPENSSL_SYS_AMIGAOS3)
+#  undef TERMIOS
+#  undef TERMIO
+#  undef SGTTY
+struct termios { int foo; };
+#  define TTY_STRUCT             struct termios
+#  define TTY_FLAGS              foo
+#  define TTY_get(tty,data)      /* tcgetattr(tty,data) */ -1
+#  define TTY_set(tty,data)      /* tcsetattr(tty,TCSANOW,data) */ 0
+#  define ECHO 0
+# endif
+
 # ifdef TERMIOS
 #  include <termios.h>
 #  define TTY_STRUCT             struct termios
