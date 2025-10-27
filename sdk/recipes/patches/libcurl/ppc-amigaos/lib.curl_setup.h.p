--- lib/curl_setup.h	2024-11-06 07:09:19.000000000 +0000
+++ lib/curl_setup.h	2024-11-08 15:18:49.324610861 +0000
@@ -408,6 +408,11 @@
        /* disable threaded resolver with clib2 - requires newlib or clib-ts */
 #      undef USE_THREADS_POSIX
 #    endif
+#    if defined(USE_AMISSL)
+#      undef HAVE_IOCTL
+#      undef HAVE_IOCTL_FIONBIO
+#      undef HAVE_IOCTL_SIOCGIFADDR
+#    endif
 #  endif
 #  include <exec/types.h>
 #  include <exec/execbase.h>
