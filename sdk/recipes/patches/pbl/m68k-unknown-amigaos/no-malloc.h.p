--- src/src/pbl.c	2010-10-18 22:45:22.000000000 +0100
+++ src/src/pbl.c	2014-12-19 18:53:46.340018151 +0000
@@ -45,7 +45,8 @@ char* pbl_c_id = "$Id: pbl.c,v 1.15 2010
 #include <string.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#include <stdlib.h>
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/src/pblCollection.c	2010-10-18 22:45:22.000000000 +0100
+++ src/src/pblCollection.c	2014-12-19 18:53:05.372018271 +0000
@@ -46,7 +46,7 @@ char* pblCollection_c_id = "$Id: pblColl
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/src/pblhash.c	2010-10-18 22:45:22.000000000 +0100
+++ src/src/pblhash.c	2014-12-19 18:52:56.652018298 +0000
@@ -45,7 +45,8 @@ char* pblhash_c_id = "$Id: pblhash.c,v 1
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#include <stdlib.h>
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/src/pblHeap.c	2010-10-18 22:45:22.000000000 +0100
+++ src/src/pblHeap.c	2014-12-19 18:53:14.144018245 +0000
@@ -45,7 +45,7 @@ char* pblHeap_c_id = "$Id: pblHeap.c,v 1
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/src/pblIterator.c	2010-10-18 22:45:22.000000000 +0100
+++ src/src/pblIterator.c	2014-12-19 18:53:22.556018222 +0000
@@ -48,7 +48,7 @@ char * PblIteratorMagic = "PblIteratorMa
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/src/pblList.c	2010-10-18 22:45:22.000000000 +0100
+++ src/src/pblList.c	2014-12-19 18:54:17.436018060 +0000
@@ -49,7 +49,7 @@ char * PblLinkedListMagic = "PblLinkedLi
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/src/pblMap.c	2010-10-18 22:45:22.000000000 +0100
+++ src/src/pblMap.c	2014-12-19 18:53:54.336018129 +0000
@@ -45,7 +45,7 @@ char* pblMap_c_id = "$Id: pblMap.c,v 1.7
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/src/pblPriorityQueue.c	2010-10-18 22:45:22.000000000 +0100
+++ src/src/pblPriorityQueue.c	2014-12-19 18:54:14.628018068 +0000
@@ -45,7 +45,7 @@ char* pblPriorityQueue_c_id =
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/src/pblSet.c	2010-10-18 22:45:22.000000000 +0100
+++ src/src/pblSet.c	2014-12-19 18:52:19.716018406 +0000
@@ -49,7 +49,7 @@ char * PblTreeSetMagic = "PblTreeSetMagi
 #include <stdio.h>
 #include <memory.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
--- src/src/pblStringBuilder.c	2010-10-18 22:45:22.000000000 +0100
+++ src/src/pblStringBuilder.c	2014-12-19 18:52:19.716018406 +0000
@@ -46,7 +46,7 @@
 #include <stdarg.h>
 #include <errno.h>
 
-#ifndef __APPLE__
+#if !defined(__APPLE__) && !defined(AMIGA)
 #include <malloc.h>
 #endif
 
