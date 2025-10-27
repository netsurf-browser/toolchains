--- crypto/ppccap.c.orig	2017-11-22 13:14:44.655536374 +0000
+++ crypto/ppccap.c	2017-11-22 13:16:03.666387174 +0000
@@ -34,6 +34,8 @@
 
 static sigset_t all_masked;
 
+#ifndef OPENSSL_SYS_AMIGAOS4
+
 static sigjmp_buf ill_jmp;
 static void ill_handler(int sig)
 {
@@ -309,3 +311,29 @@
     sigaction(SIGILL, &ill_oact, NULL);
     sigprocmask(SIG_SETMASK, &oset, NULL);
 }
+
+#else
+#include <proto/exec.h>
+void OPENSSL_cpuid_setup(void)
+{
+    uint32 family, vec;
+
+    IExec->GetCPUInfoTags(GCIT_Family, &family, GCIT_VectorUnit, &vec, TAG_DONE);
+
+    OPENSSL_ppccap_P = PPC_FPU;
+
+    if((family == CPUFAMILY_PA6T) || (family == CPUFAMILY_E5500))
+        OPENSSL_ppccap_P |= PPC_FPU64;
+
+    if((vec == VECTORTYPE_ALTIVEC) &&
+       (LIB_IS_AT_LEAST(SysBase, 51, 12)))
+    {
+        OPENSSL_ppccap_P |= PPC_ALTIVEC;
+    }
+}
+
+uint32_t OPENSSL_rdtsc(void)
+{
+    return 0;
+}
+#endif
