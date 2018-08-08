--- crypto/rand/build.info.orig	2017-11-22 11:36:06.585363219 +0000
+++ crypto/rand/build.info	2017-11-22 11:36:39.267232034 +0000
@@ -1,4 +1,4 @@
 LIBS=../../libcrypto
 SOURCE[../../libcrypto]=\
-        md_rand.c randfile.c rand_lib.c rand_err.c rand_egd.c \
-        rand_win.c rand_unix.c rand_vms.c
+        md_rand.c randfile.c rand_lib.c rand_err.c rand_amiga.c \
+        rand_egd.c rand_win.c rand_unix.c rand_vms.c
