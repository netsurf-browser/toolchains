--- src/List.h.orig	2013-10-07 22:58:00.000000000 +0200
+++ src/List.h	2013-10-07 21:52:30.000000000 +0200
@@ -30,7 +30,7 @@
 
 
 /* Create a new linked list. Returns 0 if failed */
-HermesList *Hermes_ListNew();
+HermesList *Hermes_ListNew(void);
 
 
 /* Destroy the list pointed to by list */
