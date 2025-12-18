--- src/pbl.c	2010-10-18 22:45:22.000000000 +0100
+++ src/pbl.c	2014-12-19 18:53:46.340018151 +0000
@@ -51,7 +51,7 @@ char* pbl_c_id = "$Id: pbl.c,v 1.15 2010
 #include <string.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/pblCollection.c	2010-10-18 22:45:22.000000000 +0100
+++ src/pblCollection.c	2014-12-19 18:53:05.372018271 +0000
@@ -45,7 +45,7 @@ char* pblCollection_c_id = "$Id: pblColl
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/pblhash.c	2010-10-18 22:45:22.000000000 +0100
+++ src/pblhash.c	2014-12-19 18:52:56.652018298 +0000
@@ -66,7 +66,7 @@ char* pblhash_c_id = "$Id: pblhash.c,v 1
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/pblHeap.c	2010-10-18 22:45:22.000000000 +0100
+++ src/pblHeap.c	2014-12-19 18:53:14.144018245 +0000
@@ -38,7 +38,7 @@ char* pblHeap_c_id = "$Id: pblHeap.c,v 1
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/pblhttst.c	2010-10-18 22:45:22.000000000 +0100
+++ src/pblhttst.c	2014-12-19 18:52:47.260018324 +0000
@@ -56,7 +56,7 @@ char* pblhttst_c_id = "$Id: pblhttst.c,v
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/pblIterator.c	2010-10-18 22:45:22.000000000 +0100
+++ src/pblIterator.c	2014-12-19 18:53:22.556018222 +0000
@@ -50,7 +50,7 @@ char * PblIteratorMagic = "PblIteratorMa
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/pblList.c	2010-10-18 22:45:22.000000000 +0100
+++ src/pblList.c	2014-12-19 18:54:17.436018060 +0000
@@ -54,7 +54,7 @@ char * PblLinkedListMagic = "PblLinkedLi
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/pblMap.c	2010-10-18 22:45:22.000000000 +0100
+++ src/pblMap.c	2014-12-19 18:53:54.336018129 +0000
@@ -56,7 +56,7 @@ char* pblMap_c_id = "$Id: pblMap.c,v 1.7
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/pblMapTest.c	2010-10-18 22:45:22.000000000 +0100
+++ src/pblMapTest.c	2014-12-19 18:52:28.496018379 +0000
@@ -48,7 +48,7 @@ char* pblMapTest_c_id =
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/pblPerform.c	2010-10-18 22:45:22.000000000 +0100
+++ src/pblPerform.c	2014-12-19 18:53:35.700018182 +0000
@@ -60,7 +60,7 @@ char* pblPerform_c_id =
 #include <memory.h>
 #include <sys/time.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/pblPriorityQueue.c	2010-10-18 22:45:22.000000000 +0100
+++ src/pblPriorityQueue.c	2014-12-19 18:54:14.628018068 +0000
@@ -45,7 +45,7 @@ char* pblPriorityQueue_c_id =
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/pblPriorityQueueTest.c	2010-10-18 22:45:22.000000000 +0100
+++ src/pblPriorityQueueTest.c	2014-12-19 18:52:37.076018355 +0000
@@ -45,7 +45,7 @@ char* pblPriorityQueueTest_c_id =
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/pblSet.c	2010-10-18 22:45:22.000000000 +0100
+++ src/pblSet.c	2014-12-19 18:52:19.716018406 +0000
@@ -64,7 +64,7 @@ char * PblTreeSetMagic = "PblTreeSetMagi
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/pbltest.c	2010-10-18 22:45:22.000000000 +0100
+++ src/pbltest.c	2014-12-19 18:51:55.048018477 +0000
@@ -57,7 +57,7 @@ char* pbltest_c_id = "$Id: pbltest.c,v 1
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
