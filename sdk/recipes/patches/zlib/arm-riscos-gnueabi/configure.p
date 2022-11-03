--- configure.orig	2012-05-02 05:17:08.000000000 +0100
+++ configure	2012-07-17 17:57:01.000000000 +0100
@@ -423,18 +423,18 @@
 echo >> configure.log
 
 # check for large file support, and if none, check for fseeko()
-cat > $test.c <<EOF
-#include <sys/types.h>
-off64_t dummy = 0;
-EOF
-if try $CC -c $CFLAGS -D_LARGEFILE64_SOURCE=1 $test.c; then
-  CFLAGS="${CFLAGS} -D_LARGEFILE64_SOURCE=1"
-  SFLAGS="${SFLAGS} -D_LARGEFILE64_SOURCE=1"
-  ALL="${ALL} all64"
-  TEST="${TEST} test64"
-  echo "Checking for off64_t... Yes." | tee -a configure.log
-  echo "Checking for fseeko... Yes." | tee -a configure.log
-else
+#cat > $test.c <<EOF
+##include <sys/types.h>
+#off64_t dummy = 0;
+#EOF
+#if try $CC -c $CFLAGS -D_LARGEFILE64_SOURCE=1 $test.c; then
+#  CFLAGS="${CFLAGS} -D_LARGEFILE64_SOURCE=1"
+#  SFLAGS="${SFLAGS} -D_LARGEFILE64_SOURCE=1"
+#  ALL="${ALL} all64"
+#  TEST="${TEST} test64"
+#  echo "Checking for off64_t... Yes." | tee -a configure.log
+#  echo "Checking for fseeko... Yes." | tee -a configure.log
+#else
   echo "Checking for off64_t... No." | tee -a configure.log
   echo >> configure.log
   cat > $test.c <<EOF
@@ -451,7 +451,7 @@
     SFLAGS="${SFLAGS} -DNO_FSEEKO"
     echo "Checking for fseeko... No." | tee -a configure.log
   fi
-fi
+##fi
 
 echo >> configure.log
 
