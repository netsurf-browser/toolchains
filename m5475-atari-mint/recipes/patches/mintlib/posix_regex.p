--- ./posix/regex.c.orig	2012-05-03 19:26:10.000000000 +0200
+++ ./posix/regex.c	2012-11-05 22:34:07.000000000 +0100
@@ -1364,6 +1364,7 @@
 									\
     DEBUG_PRINT2 ("  Pushing failure id: %u\n", failure_id);		\
     DEBUG_PUSH (failure_id);						\
+    (void) destination;						\
   } while (0)
 
 /* This is the number of items that are pushed and popped on the stack

