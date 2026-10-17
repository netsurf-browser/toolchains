--- src/src/pbl.c.orig	2012-08-04 20:29:29.000000000 +0100
+++ src/src/pbl.c	2012-08-04 20:29:37.000000000 +0100
@@ -43,7 +43,6 @@
 
 #include <stdio.h>
 #include <string.h>
-#include <memory.h>
 
 #ifndef __APPLE__
 #include <malloc.h>
--- src/src/pblCollection.c.orig	2012-08-04 20:27:49.000000000 +0100
+++ src/src/pblCollection.c	2012-08-04 20:28:00.000000000 +0100
@@ -44,7 +44,7 @@
 char* pblCollection_c_id = "$Id: pblCollection.c,v 1.21 2021/06/23 14:32:49 peter Exp $";
 
 #include <stdio.h>
-#include <memory.h>
+#include <string.h>
 
 #ifndef __APPLE__
 #include <malloc.h>
--- src/src/pblhash.c.orig	2012-08-04 20:29:03.000000000 +0100
+++ src/src/pblhash.c	2012-08-04 20:29:19.000000000 +0100
@@ -43,7 +43,7 @@
 char* pblhash_c_id = "$Id: pblhash.c,v 1.24 2021/06/23 14:32:49 peter Exp $";
 
 #include <stdio.h>
-#include <memory.h>
+#include <string.h>
 
 #ifndef __APPLE__
 #include <malloc.h>
--- src/src/pblHeap.c.orig	2012-08-04 20:25:40.000000000 +0100
+++ src/src/pblHeap.c	2012-08-04 20:25:52.000000000 +0100
@@ -43,7 +43,7 @@
 char* pblHeap_c_id = "$Id: pblHeap.c,v 1.13 2021/06/23 14:32:49 peter Exp $";
 
 #include <stdio.h>
-#include <memory.h>
+#include <string.h>
 
 #ifndef __APPLE__
 #include <malloc.h>
--- src/src/pblIterator.c.orig	2012-08-04 20:28:11.000000000 +0100
+++ src/src/pblIterator.c	2012-08-04 20:28:21.000000000 +0100
@@ -46,7 +46,7 @@
 char* PblIteratorMagic = "PblIteratorMagic";
 
 #include <stdio.h>
-#include <memory.h>
+#include <string.h>
 
 #ifndef __APPLE__
 #include <malloc.h>
--- src/src/pblkf.c.orig	2012-08-04 20:28:37.000000000 +0100
+++ src/src/pblkf.c	2012-08-04 20:28:47.000000000 +0100
@@ -55,7 +55,6 @@
 #include <sys/stat.h>
 
 #include <fcntl.h>
-#include <memory.h>
 #include <string.h>
 
 
--- src/src/pblList.c.orig	2012-08-04 20:27:23.000000000 +0100
+++ src/src/pblList.c	2012-08-04 20:27:35.000000000 +0100
@@ -47,7 +47,7 @@
 char* PblLinkedListMagic = "PblLinkedListMagic";
 
 #include <stdio.h>
-#include <memory.h>
+#include <string.h>
 
 #ifndef __APPLE__
 #include <malloc.h>
--- src/src/pblMap.c.orig	2012-08-04 20:26:15.000000000 +0100
+++ src/src/pblMap.c	2012-08-04 20:26:29.000000000 +0100
@@ -43,7 +43,7 @@
 char* pblMap_c_id = "$Id: pblMap.c,v 1.18 2021/06/23 14:32:48 peter Exp $";
 
 #include <stdio.h>
-#include <memory.h>
+#include <string.h>
 
 #ifndef __APPLE__
 #include <malloc.h>
--- src/src/pblPriorityQueue.c.orig	2012-08-04 20:22:41.000000000 +0100
+++ src/src/pblPriorityQueue.c	2012-08-04 20:22:56.000000000 +0100
@@ -43,7 +43,7 @@
 char* pblPriorityQueue_c_id = "$Id: pblPriorityQueue.c,v 1.17 2021/06/23 14:32:49 peter Exp $";
 
 #include <stdio.h>
-#include <memory.h>
+#include <string.h>
 
 #ifndef __APPLE__
 #include <malloc.h>
--- src/src/pblSet.c.orig	2012-08-04 20:26:54.000000000 +0100
+++ src/src/pblSet.c	2012-08-04 20:27:07.000000000 +0100
@@ -47,7 +47,7 @@
 char* PblTreeSetMagic = "PblTreeSetMagic";
 
 #include <stdio.h>
-#include <memory.h>
+#include <string.h>
 
 #ifndef __APPLE__
 #include <malloc.h>
--- src/src/pblStringBuilder.c.orig	2012-08-04 20:26:54.000000000 +0100
+++ src/src/pblStringBuilder.c	2012-08-04 20:27:07.000000000 +0100
@@ -42,7 +42,7 @@
 char* pblStringBuilder_c_id = "$Id: pblStringBuilder.c,v 1.9 2021/06/23 14:32:49 peter Exp $";
 
 #include <stdio.h>
-#include <memory.h>
+#include <string.h>
 #include <stdarg.h>
 #include <errno.h>
 
