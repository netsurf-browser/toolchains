--- lib/random_dev_urandom.c.orig	2026-10-13 23:47:54.758487372 +0100
+++ lib/random_dev_urandom.c	2026-10-16 00:17:52.008046969 +0100
@@ -34,41 +34,15 @@
 
 #include "random_dev_urandom.h"
 
-#if ! defined(_POSIX_C_SOURCE)                                                 \
-    || (defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE < 200809L))
-#  define _POSIX_C_SOURCE 200809L // for O_CLOEXEC
-#endif
+#include <openssl/rand.h>
 
-#include <errno.h>
-#include <fcntl.h>  // open
-#include <unistd.h> // close
-
-/* Extract entropy from /dev/urandom */
+/* Extract entropy from OpenSSL */
 bool
 writeRandomBytes_dev_urandom(void *target, size_t count) {
   int success = false; /* full count bytes written? */
-  size_t bytesWrittenTotal = 0;
 
-  const int fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
-  if (fd < 0) {
-    return false;
-  }
+  int count_as_int = (int) count;
+  success = (RAND_bytes(target, count_as_int) == 1);
 
-  do {
-    void *const currentTarget = (char *)target + bytesWrittenTotal;
-    const size_t bytesToWrite = count - bytesWrittenTotal;
-
-    errno = 0;
-
-    const ssize_t bytesWrittenMore = read(fd, currentTarget, bytesToWrite);
-
-    if (bytesWrittenMore > 0) {
-      bytesWrittenTotal += bytesWrittenMore;
-      if (bytesWrittenTotal >= count)
-        success = true;
-    }
-  } while (! success && (errno == EINTR));
-
-  close(fd);
   return success;
 }
--- expat.pc.in.orig	2026-10-16 11:35:02.015528911 +0100
+++ expat.pc.in	2026-10-16 11:35:16.727647137 +0100
@@ -7,7 +7,7 @@
 Version: @PACKAGE_VERSION@
 Description: expat XML parser
 URL: https://libexpat.github.io/
-Libs: -L${libdir} -l@PACKAGE_NAME@
+Libs: -L${libdir} -l@PACKAGE_NAME@ -lcrypto
 Libs.private: @LIBM@
 Cflags: -I${includedir}
 Cflags.private: -DXML_STATIC
