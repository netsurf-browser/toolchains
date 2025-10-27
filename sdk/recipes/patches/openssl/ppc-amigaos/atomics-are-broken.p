--- include/internal/tsan_assist.h.orig	2017-11-21 22:49:00.185608040 +0000
+++ include/internal/tsan_assist.h	2017-11-21 22:49:21.488219518 +0000
@@ -48,7 +48,7 @@
  */
 
 #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L \
-    && !defined(__STDC_NO_ATOMICS__)
+    && !defined(__STDC_NO_ATOMICS__) && !defined(__AMIGA__)
 # include <stdatomic.h>
 
 # if defined(ATOMIC_POINTER_LOCK_FREE) \
--- include/internal/refcount.h.orig	2017-11-21 22:49:00.185608040 +0000
+++ include/internal/refcount.h	2017-11-21 22:49:21.488219518 +0000
@@ -15,7 +15,7 @@
 
 # if defined(OPENSSL_THREADS) && !defined(OPENSSL_DEV_NO_ATOMICS)
 #  if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L \
-      && !defined(__STDC_NO_ATOMICS__)
+      && !defined(__STDC_NO_ATOMICS__) && !defined(__AMIGA__)
 #   include <stdatomic.h>
 #   define HAVE_C11_ATOMICS
 #  endif
