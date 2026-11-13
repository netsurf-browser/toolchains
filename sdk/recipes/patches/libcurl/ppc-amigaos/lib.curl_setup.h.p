--- lib/curl_setup.h	2024-11-06 07:09:19.000000000 +0000
+++ lib/curl_setup.h	2024-11-08 15:18:49.324610861 +0000
@@ -434,6 +434,11 @@
        /* disable threaded resolver with clib2 - requires newlib or clib-ts */
 #      undef USE_RESOLV_THREADED
 #    endif
+#    if defined(USE_AMISSL)
+#      undef HAVE_IOCTL
+#      undef HAVE_IOCTL_FIONBIO
+#      undef HAVE_IOCTL_SIOCGIFADDR
+#    endif
 #  endif
 #  include <exec/types.h>
 #  include <exec/execbase.h>
@@ -1640,6 +1645,8 @@
 #define curlx_memzero_low(buf, size)  (void)memset_s(buf, size, 0, size)
 #elif defined(HAVE_MEMSET_EXPLICIT)
 #define curlx_memzero_low(buf, size)  (void)memset_explicit(buf, 0, size)
+#elif defined(__AMIGA__)
+/* Use local fallback */
 #elif defined(__CYGWIN__) || \
   (defined(__NEWLIB__) && !defined(__CLIB2__)) || \
   (defined(__GLIBC__) && \
