--- crypto/bn/bn_div.c	2017-01-26 13:22:03.000000000 +0000
+++ crypto/bn/bn_div.c	2017-02-11 16:06:29.437010335 +0000
@@ -175,6 +175,22 @@ int BN_div(BIGNUM *dv, BIGNUM *rem, cons
             q;                                  \
         })
 #    define REMAINDER_IS_ALREADY_CALCULATED
+#   elif defined(__mc68020__)
+   /* Same rationale as for i386. On the 68020-60/CPU32 the divu
+    * instruction will take a 64 bit dividend in a register pair
+    * and overwrite that pair with the remainder:quotient.
+    *                                  Howard Chu
+    */
+#    undef bn_div_words
+#    define bn_div_words(n0,n1,d0)              \
+       ({ asm volatile (                       \
+               "divu.l %4,%2:%3"               \
+               : "=d"(rem), "=d"(q)            \
+               : "0"(n0), "1"(n1), "d"(d0)     \
+               : "cc");                        \
+           q;                                  \
+       })
+#    define REMAINDER_IS_ALREADY_CALCULATED
 #   endif                       /* __<cpu> */
 #  endif                        /* __GNUC__ */
 # endif                         /* OPENSSL_NO_ASM */
