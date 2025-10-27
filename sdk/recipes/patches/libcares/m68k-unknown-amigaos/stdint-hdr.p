--- src/lib/ares_sortaddrinfo.c
+++ src/lib/ares_sortaddrinfo.c
@@ -36,6 +36,8 @@
  * SPDX-License-Identifier: BSD-3-Clause
  */
 
+#include <stdint.h>
+
 #include "ares_private.h"
 
 #ifdef HAVE_NETINET_IN_H
