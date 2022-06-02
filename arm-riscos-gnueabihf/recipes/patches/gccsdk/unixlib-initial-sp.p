Index: libunixlib/sys/_syslib.s
===================================================================
--- libunixlib/sys/_syslib.s	(revision 7698)
+++ libunixlib/sys/_syslib.s	(working copy)
@@ -396,8 +396,9 @@
 	SWI	XOS_ReadVarVal	@ Read value of progname$HeapMax
 	BVS	t08
 	TEQ	v1, #1		@ Should be a number variable
-	LDREQ	v2, [sp], #4
+	LDREQ	v2, [sp]
 	MOVEQ	v2, v2, LSL#20	@ Convert MB into bytes
+	ADD	sp, sp, #4
 
 	@ v2 = size of DA area
 t08:
@@ -512,7 +513,15 @@
 	MOVCC	a1, #ERR_NO_FPE
 	BCC	__exit_with_error_num
 #endif
+#else
+#ifdef __ARM_EABI__
+	@ EABI with software FP. Burn the top of the stack to appease
+	@ ARMEABISupport (which does not consider the stack top address
+	@ as part of the stack).
+	SUB	sp, sp, #4
+	BIC	sp, sp, #7	@ AAPCS wants 8 byte alignment
 #endif
+#endif
 
 	@ We need to create this now so that we have its address to give to
 	@ the callback handler, but we can initialise it later.
