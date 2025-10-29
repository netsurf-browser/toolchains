--- srclib/time.in.h	2019-01-06 08:51:41.000000000 +0000
+++ srclib/time.in.h	2020-03-06 19:46:37.429016156 +0000
@@ -76,7 +76,7 @@ extern "C" {
 #    define timespec rpl_timespec
 struct timespec
 {
-  time_t tv_sec;
+  time_t tv_secs;
   long int tv_nsec;
 };
 #    define GNULIB_defined_struct_timespec 1
