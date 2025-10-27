Index: libunixlib/time/stdtime.c
===================================================================
--- libunixlib/time/stdtime.c	(revision 7698)
+++ libunixlib/time/stdtime.c	(working copy)
@@ -23,9 +23,11 @@
   register const char *str __asm ("r4") = __str;
   register const _kernel_oserror *err __asm ("r0");
   register char *term __asm ("r1");
+  register size_t left __asm ("r2");
+  register const char *ostr __asm ("r3");
   __asm__ volatile ("SWI\t%[SWI_Territory_ConvertDateAndTime]\n\t"
 		    "MOVVC\tr0, #0\n\t"
-		    : "=r" (err), "=r" (term)
+		    : "=r" (err), "=r" (term), "=r" (left), "=r" (ostr)
 		    : "r" (territory), "r" (ro_time), "r" (buf), "r" (size), "r" (str),
 		      [SWI_Territory_ConvertDateAndTime] "i" (Territory_ConvertDateAndTime | (1<<17))
 		    : "r14", "cc", "memory");
