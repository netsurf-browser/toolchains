--- src/pbl.c.orig	2012-08-04 20:29:29.000000000 +0100
+++ src/pbl.c	2012-08-04 20:29:37.000000000 +0100
@@ -49,7 +49,6 @@
 
 #include <stdio.h>
 #include <string.h>
-#include <memory.h>
 
 #ifndef __APPLE__
 #include <malloc.h>
--- src/pblCollection.c.orig	2012-08-04 20:27:49.000000000 +0100
+++ src/pblCollection.c	2012-08-04 20:28:00.000000000 +0100
@@ -43,7 +43,7 @@
 char* pblCollection_c_id = "$Id: pblCollection.c,v 1.13 2010/05/19 22:38:45 peter Exp $";
 
 #include <stdio.h>
-#include <memory.h>
+#include <string.h>
 
 #ifndef __APPLE__
 #include <malloc.h>
--- src/pblhash.c.orig	2012-08-04 20:29:03.000000000 +0100
+++ src/pblhash.c	2012-08-04 20:29:19.000000000 +0100
@@ -64,7 +64,7 @@
 char* pblhash_c_id = "$Id: pblhash.c,v 1.18 2010/05/30 20:06:45 peter Exp $";
 
 #include <stdio.h>
-#include <memory.h>
+#include <string.h>
 
 #ifndef __APPLE__
 #include <malloc.h>
--- src/pblHeap.c.orig	2012-08-04 20:25:40.000000000 +0100
+++ src/pblHeap.c	2012-08-04 20:25:52.000000000 +0100
@@ -36,7 +36,7 @@
 char* pblHeap_c_id = "$Id: pblHeap.c,v 1.5 2010/08/31 21:06:20 peter Exp $";
 
 #include <stdio.h>
-#include <memory.h>
+#include <string.h>
 
 #ifndef __APPLE__
 #include <malloc.h>
--- src/pblIterator.c.orig	2012-08-04 20:28:11.000000000 +0100
+++ src/pblIterator.c	2012-08-04 20:28:21.000000000 +0100
@@ -48,7 +48,7 @@
 char * PblIteratorMagic = "PblIteratorMagic";
 
 #include <stdio.h>
-#include <memory.h>
+#include <string.h>
 
 #ifndef __APPLE__
 #include <malloc.h>
--- src/pblkf.c.orig	2012-08-04 20:28:37.000000000 +0100
+++ src/pblkf.c	2012-08-04 20:28:47.000000000 +0100
@@ -72,7 +72,6 @@
 #include <sys/stat.h>
 
 #include <fcntl.h>
-#include <memory.h>
 #include <string.h>
 
 
--- src/pblList.c.orig	2012-08-04 20:27:23.000000000 +0100
+++ src/pblList.c	2012-08-04 20:27:35.000000000 +0100
@@ -52,7 +52,7 @@
 char * PblLinkedListMagic = "PblLinkedListMagic";
 
 #include <stdio.h>
-#include <memory.h>
+#include <string.h>
 
 #ifndef __APPLE__
 #include <malloc.h>
--- src/pblMap.c.orig	2012-08-04 20:26:15.000000000 +0100
+++ src/pblMap.c	2012-08-04 20:26:29.000000000 +0100
@@ -54,7 +54,7 @@
 char* pblMap_c_id = "$Id: pblMap.c,v 1.7 2010/05/30 20:06:45 peter Exp $";
 
 #include <stdio.h>
-#include <memory.h>
+#include <string.h>
 
 #ifndef __APPLE__
 #include <malloc.h>
--- src/pblPriorityQueue.c.orig	2012-08-04 20:22:41.000000000 +0100
+++ src/pblPriorityQueue.c	2012-08-04 20:22:56.000000000 +0100
@@ -43,7 +43,7 @@
         "$Id: pblPriorityQueue.c,v 1.12 2010/10/01 20:44:31 peter Exp $";
 
 #include <stdio.h>
-#include <memory.h>
+#include <string.h>
 
 #ifndef __APPLE__
 #include <malloc.h>
--- src/pblSet.c.orig	2012-08-04 20:26:54.000000000 +0100
+++ src/pblSet.c	2012-08-04 20:27:07.000000000 +0100
@@ -62,7 +62,7 @@
 
 
 #include <stdio.h>
-#include <memory.h>
+#include <string.h>
 
 #ifndef __APPLE__
 #include <malloc.h>
