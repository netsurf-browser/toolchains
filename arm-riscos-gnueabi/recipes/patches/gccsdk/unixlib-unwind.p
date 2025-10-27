Index: libunixlib/Makefile.am
===================================================================
--- libunixlib/Makefile.am      (revision 7698)
+++ libunixlib/Makefile.am      (working copy)
@@ -33,7 +33,7 @@
 # arguments can not be tested for NULL in UnixLib itself.
 if ARM_EABI
 AM_CFLAGS = -D__GNU_LIBRARY__ -DNO_LONG_DOUBLE -D_GNU_SOURCE=1 \
-	-D__UNIXLIB_NO_NONNULL -std=c99 $(LIBM_FLAGS)
+	-D__UNIXLIB_NO_NONNULL -std=c99 -mpoke-function-name -funwind-tables $(LIBM_FLAGS)
 UNIXLIB_CHUNKED_STACK=0
 else
 AM_CFLAGS = -D__GNU_LIBRARY__ -DNO_LONG_DOUBLE -D_GNU_SOURCE=1 \
Index: libunixlib/signal/_signal.s
===================================================================
--- libunixlib/signal/_signal.s	(revision 7698)
+++ libunixlib/signal/_signal.s	(working copy)
@@ -352,8 +352,8 @@
 	CHGMODE	a1, USR_Mode	@ Back to USR mode now we have a stack
 
 #ifdef __ARM_EABI__
-	STMFD	sp!, {v1, v3}
-	ADD	fp, sp, #4
+	ANDS	v2, sp, #7		@ Align stack
+	SUBEQ	sp, sp, #4
 #else
 	ADR	v4, __h_error + 4*3	@ Point at handler name for backtrace
 	STMFD	sp!, {v1, v2, v3, v4}	@ Setup an APCS-32 stack frame so we
@@ -758,10 +758,9 @@
 	SWINE	XOS_Byte		@ This calls our escape handler
 
 #ifdef __ARM_EABI__
-	LDR	a3, [sp, #14*4 + 4]	@ saved USR lr
-	LDR	a1, [sp, #11*4 + 4]	@ saved USR fp
-	STMFD	sp!, {a1, a3}		@ create signal frame
-	MOV	fp, sp			@ FIXME: check this with compiler output for similar function
+	MOV	a1, sp			@ a1 -> register save block
+	ANDS	v2, sp, #7
+	SUBNE	sp, sp, #4		@ align stack to 8 bytes
 #else
 	@ Create an APCS-32 compilant signal stack frame
 	ADR	a4, __h_cback + 4*3	@ point at handler name for backtrace
@@ -796,11 +795,12 @@
 	STR	a1, [a3, #GBL_EXECUTING_SIGNALHANDLER]
 
 #ifdef __ARM_EABI__
-	ADD	a1, sp, #8	@ Skip signal frame (fp, lr)
+	TEQ	v2, #0
+	ADDNE	a1, sp, #4	@ Undo stack alignment
 #else
 	ADD	a1, sp, #16	@ Skip signal frame (fp, sp, lr, name ptr)
 #endif
-	ADD	sp, sp, #16+17*4
+	ADD	sp, sp, #17*4
 	SWI	XOS_EnterOS	@ We need to be in SVC mode so reenbling IRQs
 				@ is atomic with returning to USR mode,
 				@ otherwise USR sp could be overwitten by
Index: libunixlib/signal/post.c
===================================================================
--- libunixlib/signal/post.c	(revision 7698)
+++ libunixlib/signal/post.c	(working copy)
@@ -255,19 +255,230 @@
   fprintf (stderr, "\nTermination signal received: %s\n", sys_siglist[signo]);
 }
 
+static void
+__write_abort_block (const unsigned int *blk, int is32bit)
+{
+  const unsigned int pcmask = is32bit ? 0xfffffffcu : 0x03fffffcu;
+
+  fprintf (stderr, "\n  Register dump at %08x:\n", (unsigned int) blk);
+
+  if (!__valid_address (blk, blk + 17))
+    fputs ("\n    [bad register dump address]\n", stderr);
+  else
+    {
+      const char rnames[] = "a1a2a3a4v1v2v3v4v5v6slfpipsplrpc";
+      for (int reg = 0; reg < 16; reg++)
+	{
+	  if ((reg & 0x3) == 0)
+	    fputs ("\n   ", stderr);
+
+	  fprintf (stderr, " %c%c: %8x",
+		   rnames[2*reg + 0], rnames[2*reg + 1], blk[reg + 1]);
+	}
+
+      if (is32bit)
+	fprintf (stderr, "\n    cpsr: %8x\n", blk[0]);
+      else
+	{
+	  const char * const pmode[4] = { "USR", "FIQ", "IRQ", "SVC" };
+	  fprintf (stderr, "\n    Mode %s, flags set: %c%c%c%c%c%c\n",
+		   pmode[blk[15 + 1] & 3],
+		   (blk[15 + 1] & (1<<31)) ? 'N' : 'n',
+		   (blk[15 + 1] & (1<<30)) ? 'Z' : 'z',
+		   (blk[15 + 1] & (1<<29)) ? 'C' : 'c',
+		   (blk[15 + 1] & (1<<28)) ? 'V' : 'v',
+		   (blk[15 + 1] & (1<<27)) ? 'I' : 'i',
+		   (blk[15 + 1] & (1<<26)) ? 'F' : 'f');
+	}
+
+      unsigned int *pc = (unsigned int *) (blk[15 + 1] & pcmask);
+
+      /* Try LR if PC invalid (e.g. with a prefetch abort).  */
+      if (pc < (unsigned int *)0x8000 || !__valid_address (pc - 5, pc + 4))
+	pc = (unsigned int *) (blk[14 + 1] & pcmask);
+
+      if (pc >= (unsigned int *)0x8000 && __valid_address (pc - 5, pc + 4))
+	{
+	  for (unsigned int *diss = pc - 5; diss < pc + 4; diss++)
+	    {
+	      const char *ins;
+	      int length;
+	      _swix (Debugger_Disassemble, _INR(0,1) | _OUTR(1,2),
+		     *diss, diss, &ins, &length);
+
+	      const unsigned char c[4] =
+		{
+		  (*diss >>  0) & 0xFF,
+		  (*diss >>  8) & 0xFF,
+		  (*diss >> 16) & 0xFF,
+		  (*diss >> 24)
+		};
+	      fprintf (stderr, "\n  %08x : %c%c%c%c : %08x : ",
+		       (unsigned int) diss,
+		       (c[0] >= ' ' && c[0] != 127) ? c[0] : '.',
+		       (c[1] >= ' ' && c[1] != 127) ? c[1] : '.',
+		       (c[2] >= ' ' && c[2] != 127) ? c[2] : '.',
+		       (c[3] >= ' ' && c[3] != 127) ? c[3] : '.',
+		       *diss);
+	      fwrite (ins, length, 1, stderr);
+	    }
+	}
+      else
+	fputs ("\n  [Disassembly not available]", stderr);
+    }
+
+  fputs ("\n\n", stderr);
+}
+
 /* Clang and GCC do not have compatible frame pointers.  */
 #ifdef __clang__
 #define FP_OFFSET (0)
 #define LR_OFFSET (1)
-#elif defined (__ARM_EABI__)
-#define FP_OFFSET (-1)
-#define LR_OFFSET (0)
 #else
 #define LR_OFFSET (-1)
 #define FP_OFFSET (-3)
 #endif
 
+#ifdef __ARM_EABI__
+/**
+ * AAPCS does not require the compiler to construct a backtrace structure
+ * in the stack (unlike APCS, which does). This results in FP rarely pointing
+ * at any form of valid stack frame (and, to complicate matters, at the time
+ * of writing, some frames end up with APCS-format frame records, anyway)
+ * which makes it nigh-on impossible to reliably unwind the stack without
+ * additional information). FP is thus often treated as an additional
+ * callee-saved register (i.e. v8) in AAPCS-conformant code.
+ *
+ * Additionally, where frame records are generated, AAPCS has them contain
+ * two entries: previous-FP and LR on entry. There is therefore (unlike APCS)
+ * no way of finding the function entry point from the frame record at all,
+ * even if it did exist.
+ *
+ * So, we cannot trust that FP ever points at a valid stack frame record and
+ * we cannot find function entry points to extract poked function names from.
+ * We can, however, make stack unwinding work if we have some means of
+ * identifying the function in which an arbitrary instruction lies.
+ *
+ * -funwind-tables will result in clang/GCC generating such a data structure,
+ * (an array between __exidx_start and __exidx_end) which will be consulted
+ * by _Unwind_Backtrace() when unwinding the stack.
+ */
+
+#include <unwind.h>
+
+typedef struct {
+  const unsigned int *regs;
+  const unsigned int *last_fn;
+} ul_unwind_ctx;
+
+static void __attribute__((naked))
+__do_unwind (_Unwind_Trace_Fn fn, const void *pw)
+{
+  __asm volatile(
+    "stmfd sp!, {fp, lr};"
+    "add fp, sp, #4;"
+    /* Registers at this point in time will be the initial state.
+     * The trace function must unwind the stack frame we just created
+     * because the personality function will be told there is nothing
+     * to do as we are declared naked.
+     */
+    "bl _Unwind_Backtrace;"
+    "ldmfd sp!, {fp, pc};"
+  );
+}
+
+static _Unwind_Reason_Code
+__write_backtrace_cb (_Unwind_Context *ctx, void *pw)
+{
+  ul_unwind_ctx *uctx = pw;
+  _Unwind_Control_Block *ucbp = NULL;
+  const unsigned int *fn;
+
+  ucbp = (_Unwind_Control_Block *) _Unwind_GetGR(ctx, UNWIND_POINTER_REG);
+  fn = (const unsigned int *) ucbp->pr_cache.fnstart;
+
+  uctx->last_fn = fn;
+
+  if (fn == (const unsigned int *) __do_unwind)
+    {
+      /* First call */
+      if (uctx->regs == NULL)
+        {
+          /* Running thread: unwind on behalf of __do_unwind */
+          _Unwind_VRS_Pop (ctx, _UVRSC_CORE, (1<<11)|(1<<14), _UVRSD_UINT32);
+        }
+      else
+        {
+          /* Thread backtrace: replace entire VRS */
+          int idx;
+          for (idx = 16; idx > 0; idx--)
+            _Unwind_SetGR (ctx, idx - 1, uctx->regs[idx - 1]);
+        }
+
+      return _URC_NO_REASON;
+    }
+
+  fprintf (stderr, "  (%8x) fn: %8x pc: %8x sp: %8x ",
+	   _Unwind_GetGR (ctx, 11), (unsigned int)fn, _Unwind_GetIP (ctx),
+	   _Unwind_GetGR (ctx, 13));
+
+#if PIC
+  /* FIXME: extend this with source location when available.  */
+  const char *lib = NULL;
+  unsigned offset;
+  _swix(SOM_Location,
+	_IN(0) | _OUTR(0,1), _Unwind_GetIP (ctx), &lib, &offset);
+  if (lib)
+    fprintf(stderr, " : %8X : %s\n", offset, lib);
+  else
+#endif
+    {
+      int cplusplus_name;
+      const char *name = extract_name (fn, &cplusplus_name);
+      fprintf (stderr, (cplusplus_name) ? " %s\n" : " %s()\n", name);
+    }
+
+  return _URC_NO_REASON;
+}
+
 static void
+__write_backtrace_thread (const unsigned int *regs)
+{
+  ul_unwind_ctx ctx;
+ 
+  /* First pass: dump trace for stack as provided */
+  ctx.regs = regs;
+  ctx.last_fn = NULL;
+  __do_unwind (__write_backtrace_cb, &ctx);
+
+  /* If we got here via an environment handler, there may be a saved abort
+   * block to look at. We only want to look if the first pass terminated with
+   * __unixlib_raise_signal (being the entry point to all this unwind logic
+   * from the environment handlers) -- if the first pass terminated somewhere
+   * else, then it is likely that we have been invoked directly via raise(),
+   * and so the presence or otherwise of an abort block is irrelevant.
+   *
+   * If an abort block is available, it will be pointed at by the
+   * (misnamed for EABI) __ul_callbackfp; if not __ul_callbackfp will be NULL.
+   * Additionally, we only want to consider the abort block if we're dumping
+   * the running thread, so check for regs being NULL to identify that.
+   */
+  if (__ul_callbackfp != NULL && regs == NULL
+      && ctx.last_fn == (unsigned int *) __unixlib_raise_signal)
+    {
+      /* Abort block: cpsr, r0-r15. */
+      __write_abort_block (__ul_callbackfp, /* is32bit= */ 1);
+
+      /* Dump remaining trace from block (skipping over saved CPSR) */
+      ctx.regs = __ul_callbackfp + 1;
+      ctx.last_fn = NULL;
+      __do_unwind (__write_backtrace_cb, &ctx);
+    }
+
+  fputc ('\n', stderr);
+}
+#else
+static void
 __write_backtrace_thread (const unsigned int *fp)
 {
   /* Running as USR26 or USR32 ?  */
@@ -306,22 +517,6 @@
 	  break;
 	}
 
-#ifdef __ARM_EABI__
-      const unsigned int * const lr = (unsigned int *)fp[LR_OFFSET];
-      fprintf (stderr, "  (%8x) lr: %8x",
-	       (unsigned int)fp, (unsigned int)lr);
-#if PIC
-      /* FIXME: extend this with source location when available.  */
-      const char *lib = NULL;
-      unsigned offset;
-      _swix(SOM_Location,
-	    _IN(0) | _OUTR(0,1), lr, &lib, &offset);
-      if (lib)
-	fprintf(stderr, " : %8X : %s\n", offset, lib);
-      else
-#endif
-	fputc('\n', stderr);
-#else
       /* Retrieve PC counter.
 	 PC counter has been saved using STMxx ..., { ..., PC } so it can be
 	 8 or 12 bytes away from the STMxx instruction depending on the ARM
@@ -347,96 +542,24 @@
       int cplusplus_name;
       const char *name = extract_name (pc, &cplusplus_name);
       fprintf (stderr, (cplusplus_name) ? " %s\n" : " %s()\n", name);
-#endif
+
       oldfp = fp;
       fp = (const unsigned int *)fp[FP_OFFSET];
-#ifndef __ARM_EABI__
       if (__ul_callbackfp != NULL && fp == __ul_callbackfp)
 	{
 	  /* At &oldfp[1] = cpsr, a1-a4, v1-v6, sl, fp, ip, sp, lr, pc */
-	  fprintf (stderr, "\n  Register dump at %08x:\n",
-		   (unsigned int) &oldfp[1]);
-
-	  if (!__valid_address (oldfp + 1, oldfp + 18))
-	    fputs ("\n    [bad register dump address]\n", stderr);
-	  else
-	    {
-	      const char rnames[] = "a1a2a3a4v1v2v3v4v5v6slfpipsplrpc";
-	      for (int reg = 0; reg < 16; reg++)
-		{
-		  if ((reg & 0x3) == 0)
-		    fputs ("\n   ", stderr);
-		  
-		  fprintf (stderr, " %c%c: %8x",
-			   rnames[2*reg + 0], rnames[2*reg + 1], oldfp[reg + 2]);
-		}
-
-	      if (is32bit)
-		fprintf (stderr, "\n    cpsr: %8x\n", oldfp[1]);
-	      else
-		{
-		  const char * const pmode[4] = { "USR", "FIQ", "IRQ", "SVC" };
-		  fprintf (stderr, "\n    Mode %s, flags set: %c%c%c%c%c%c\n",
-			   pmode[oldfp[15 + 2] & 3],
-			   (oldfp[15 + 2] & (1<<31)) ? 'N' : 'n',
-			   (oldfp[15 + 2] & (1<<30)) ? 'Z' : 'z',
-			   (oldfp[15 + 2] & (1<<29)) ? 'C' : 'c',
-			   (oldfp[15 + 2] & (1<<28)) ? 'V' : 'v',
-			   (oldfp[15 + 2] & (1<<27)) ? 'I' : 'i',
-			   (oldfp[15 + 2] & (1<<26)) ? 'F' : 'f');
-		}
-
-	      pc = (unsigned int *) (oldfp[17] & pcmask);
-
-	      /* Try LR if PC invalid (e.g. with a prefetch abort).  */
-	      if (pc < (unsigned int *)0x8000 || !__valid_address (pc - 5, pc + 4))
-		pc = (unsigned int *) (oldfp[16] & pcmask);
-
-	      if (pc >= (unsigned int *)0x8000 && __valid_address (pc - 5, pc + 4))
-		{
-		  for (unsigned int *diss = pc - 5; diss < pc + 4; diss++)
-		    {
-		      const char *ins;
-		      int length;
-		      _swix (Debugger_Disassemble, _INR(0,1) | _OUTR(1,2),
-			     *diss, diss, &ins, &length);
-
-		      const unsigned char c[4] =
-			{
-			  (*diss >>  0) & 0xFF,
-			  (*diss >>  8) & 0xFF,
-			  (*diss >> 16) & 0xFF,
-			  (*diss >> 24)
-			};
-		      fprintf (stderr, "\n  %08x : %c%c%c%c : %08x : ",
-			       (unsigned int) diss,
-			       (c[0] >= ' ' && c[0] != 127) ? c[0] : '.',
-			       (c[1] >= ' ' && c[1] != 127) ? c[1] : '.',
-			       (c[2] >= ' ' && c[2] != 127) ? c[2] : '.',
-			       (c[3] >= ' ' && c[3] != 127) ? c[3] : '.',
-			       *diss);
-		      fwrite (ins, length, 1, stderr);
-		    }
-		}
-	      else
-		fputs ("\n  [Disassembly not available]", stderr);
-	    }
-
-	  fputs ("\n\n", stderr);
+          __write_abort_block (&oldfp[1], is32bit);
 	}
-#endif
     }
 
   fputc ('\n', stderr);
 }
+#endif
 
-
 void
 __write_backtrace (int signo)
 {
-#ifdef __ARM_EABI__
-  register const unsigned int *fp = __builtin_frame_address(0);
-#else
+#ifndef __ARM_EABI__
   register const unsigned int *fp __asm ("fp");
 #endif
 
@@ -485,7 +608,11 @@
   /* Dump first the details of the current thread.  */
   fprintf (stderr, "Stack backtrace:\n\nRunning thread %p (%s)\n",
 	   __pthread_running_thread, __pthread_running_thread->name);
+#ifdef __ARM_EABI__
+  __write_backtrace_thread (NULL);
+#else
   __write_backtrace_thread (fp);
+#endif
 
   /* And then the other suspended threads if any.  */
   for (pthread_t th = __pthread_thread_list; th != NULL; th = th->next)
@@ -494,7 +621,10 @@
         continue;
 
       fprintf (stderr, "\nThread %p (%s)\n", th, th->name);
-#ifdef __clang__
+#ifdef __ARM_EABI__
+      __write_backtrace_thread (&th->saved_context->r[0]);
+#else
+# ifdef __clang__
       const unsigned int fakestackframe[] =
         {
           (unsigned int)th->saved_context->r[11],
@@ -501,22 +631,16 @@
           (unsigned int)th->saved_context->r[14]
         };
       __write_backtrace_thread (&fakestackframe[0]);
-#elif defined (__ARM_EABI__)
+# else
       const unsigned int fakestackframe[] =
         {
           (unsigned int)th->saved_context->r[11],
-          (unsigned int)th->saved_context->r[14]
-        };
-      __write_backtrace_thread (&fakestackframe[1]);
-#else
-      const unsigned int fakestackframe[] =
-        {
-          (unsigned int)th->saved_context->r[11],
           (unsigned int)th->saved_context->r[13],
           (unsigned int)th->saved_context->r[14],
           (unsigned int)th->saved_context->r[15]
         };
       __write_backtrace_thread (&fakestackframe[3]);
+# endif
 #endif
     }
 }
