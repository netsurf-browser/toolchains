Index: libunixlib/gcccompat/atomics.c
===================================================================
--- libunixlib/gcccompat/atomics.c	(revision 7698)
+++ libunixlib/gcccompat/atomics.c	(working copy)
@@ -146,8 +146,6 @@
 ATOMIC_COMPARE_AND_EXCHANGE(4,uint32_t)
 ATOMIC_COMPARE_AND_EXCHANGE(8,uint64_t)
 
-#ifndef __ARM_EABI__
-
 /* These built-in functions perform the operation suggested by the name,
  * and return the value that had previously been in *ptr. That is,
  *
@@ -200,8 +198,6 @@
 SYNC_FETCH_AND_OP(xor,^,8,long long)
 SYNC_FETCH_AND_OP(and,&,8,long long)
 
-#endif
-
 /* These built-in functions perform the operation suggested by the name,
  * and return the new value. That is,
  * 
