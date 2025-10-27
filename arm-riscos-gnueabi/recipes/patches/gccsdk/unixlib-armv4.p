Index: libunixlib/gcccompat/atomics.c
===================================================================
--- libunixlib/gcccompat/atomics.c	(revision 7698)
+++ libunixlib/gcccompat/atomics.c	(working copy)
@@ -378,7 +378,7 @@
 ATOMIC_OP_FETCH(or,(*ptr) |= val,unsigned)
 ATOMIC_OP_FETCH(nand,(*ptr) = ~((*ptr) & val),unsigned)
 
-#ifdef __ARM_EABI__
+#if defined(__ARM_EABI__) && defined(__ARM_ARCH) && (__ARM_ARCH >= 6)
 
 static int __cmpxchg (int oldval, int newval, volatile int *ptr)
 {
Index: libunixlib/pthread/_ints.s
===================================================================
--- libunixlib/pthread/_ints.s	(revision 7698)
+++ libunixlib/pthread/_ints.s	(working copy)
@@ -14,6 +14,15 @@
 	.protected __pthread_enable_ints
 	.global	__pthread_protect_unsafe
 
+#if defined(__ARM_ARCH) && (__ARM_ARCH__ < 6)
+	/* Modern gas objects to our runtime-conditional use of LDREX/STREX
+	 * in here. If we're building for pre-armv6 architectures, tell gas
+	 * that to allow armv6 instructions, but mark the resulting object
+	 * as armv4 compatible. */
+	.arch armv6
+	.object_arch armv4
+#endif
+
 @ Disable context switches by incrementing the semaphore
 @ May be called from USR or SVC mode
 	NAME	__pthread_disable_ints
@@ -23,7 +32,7 @@
 	LDR	a1, .L0				@ =__ul_global
  PICEQ "LDR	a1, [a2, a1]"
 	LDR	ip, [a1, #GBL_PTH_CALLEVERY_RMA]
-#ifdef __ARM_EABI__
+#if defined(__ARM_EABI__) && defined(__ARM_ARCH) && (__ARM_ARCH >= 6)
 	ADD	a1, ip, #PTHREAD_CALLEVERY_RMA_WORKSEMAPHORE
 #else
 	LDR	a4, [a1, #GBL_CPU_FLAGS]
@@ -93,7 +102,7 @@
 	LDR	a4, .L2				@ =__ul_global
  PICEQ "LDR	a4, [a1, a4]"
 	LDR	ip, [a4, #GBL_PTH_CALLEVERY_RMA]
-#ifdef __ARM_EABI__
+#if defined(__ARM_EABI__) && defined(__ARM_ARCH) && (__ARM_ARCH >= 6)
 	ADD	a1, ip, #PTHREAD_CALLEVERY_RMA_WORKSEMAPHORE
 #else
 	LDR	a2, [a4, #GBL_CPU_FLAGS]
Index: libunixlib/sys/_syslib.s
===================================================================
--- libunixlib/sys/_syslib.s	(revision 7698)
+++ libunixlib/sys/_syslib.s	(working copy)
@@ -49,6 +49,15 @@
 	.weak	__dynamic_da_max_size
 	.weak   __stack_size
 
+#if defined(__ARM_ARCH) && (__ARM_ARCH__ < 6)
+	/* Modern gas objects to our runtime-conditional use of LDREX/STREX
+	 * in here. If we're building for pre-armv6 architectures, tell gas
+	 * that to allow armv6 instructions, but mark the resulting object
+	 * as armv4 compatible. */
+	.arch armv6
+	.object_arch armv4
+#endif
+
 	@ RMEnsure the minimum version of the SharedUnixLibrary we need.
 	@ Now check System modules first as UnixLib package is deprecated.
 #if !defined(__SOFTFP__) && defined(__VFP_FP__)
@@ -1414,7 +1423,7 @@
 	@ we go for a straight OS_Exit scenario.  Anything better we
 	@ can do ?
 	ADD	a3, a4, #GBL_PANIC_MODE
-#ifndef __ARM_EABI__
+#if !defined(__ARM_EABI__) || !defined(__ARM_ARCH) || (__ARM_ARCH < 6)
 	LDR	a2, [a4, #GBL_CPU_FLAGS]
 	TST	a2, #__CPUCAP_HAVE_SWP
 	BEQ	0f
