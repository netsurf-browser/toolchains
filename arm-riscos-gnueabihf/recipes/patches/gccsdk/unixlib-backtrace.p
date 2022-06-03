--- libunixlib/signal/post.c	2021-06-15 21:42:03.000000000 +0100
+++ libunixlib/signal/post.c	2022-06-03 19:52:18.316234729 +0100
@@ -307,6 +307,7 @@
 	}
 
 #ifdef __ARM_EABI__
+      const unsigned int *pc = NULL;
       const unsigned int * const lr = (unsigned int *)fp[LR_OFFSET];
       fprintf (stderr, "  (%8x) lr: %8x",
 	       (unsigned int)fp, (unsigned int)lr);
@@ -350,7 +351,6 @@
 #endif
       oldfp = fp;
       fp = (const unsigned int *)fp[FP_OFFSET];
-#ifndef __ARM_EABI__
       if (__ul_callbackfp != NULL && fp == __ul_callbackfp)
 	{
 	  /* At &oldfp[1] = cpsr, a1-a4, v1-v6, sl, fp, ip, sp, lr, pc */
@@ -424,7 +424,6 @@
 
 	  fputs ("\n\n", stderr);
 	}
-#endif
     }
 
   fputc ('\n', stderr);
--- libunixlib/signal/_signal.s	2020-05-31 20:11:06.000000000 +0100
+++ libunixlib/signal/_signal.s	2022-06-03 19:45:32.872892834 +0100
@@ -761,7 +761,7 @@
 	LDR	a3, [sp, #14*4 + 4]	@ saved USR lr
 	LDR	a1, [sp, #11*4 + 4]	@ saved USR fp
 	STMFD	sp!, {a1, a3}		@ create signal frame
-	MOV	fp, sp			@ FIXME: check this with compiler output for similar function
+	ADD	fp, sp, #4		@ FIXME: check this with compiler output for similar function
 #else
 	@ Create an APCS-32 compilant signal stack frame
 	ADR	a4, __h_cback + 4*3	@ point at handler name for backtrace
