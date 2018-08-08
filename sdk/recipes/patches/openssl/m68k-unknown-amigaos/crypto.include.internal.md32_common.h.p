--- crypto/include/internal/md32_common.h	2017-11-02 14:29:03.000000000 +0000
+++ crypto/include/internal/md32_common.h	2018-07-23 20:12:11.228066913 +0100
@@ -182,6 +182,9 @@
 #      define HOST_l2c(l,c)      (*((unsigned int *)(c))=(l), (c)+=4, (l))
 #     endif
 #    endif
+#   elif defined(__mc68020)
+#    define HOST_c2l(c,l) ({ asm volatile ("movel %0@+,%1" : "=a"(c), "=d"(l) : "0"(c)); })
+#    define HOST_l2c(l,c) ({ asm volatile ("movel %1,%0@+" : "=a"(c) : "d"(l), "0"(c)); })
 #   endif
 #  endif
 #  if defined(__s390__) || defined(__s390x__)
@@ -215,6 +218,22 @@
 #    define HOST_l2c(l,c)        ({ asm ("strv   %1,%0"                  \
                                    :"=m"(*(unsigned int *)(c)) :"d"(l));\
                                    (c)+=4; (l);                         })
+#   elif defined(__mc68020)
+#    define HOST_c2l(c,l)        ({ unsigned int r;                      \
+                                    asm volatile ("movel  %1@+,%0\n"     \
+                                                  "rorw   #8,%0\n"       \
+                                                  "swap   %0\n"          \
+                                                  "rorw   #8,%0\n"       \
+                                    : "=d"(r), "=a"(c)                   \
+                                    : "1"(c));                           \
+                                    (l)=r; })
+#    define HOST_l2c(l,c)        ({ unsigned int r=(l);                  \
+                                    asm volatile ("rorw   #8,%0\n"       \
+                                                  "swap   %0\n"          \
+                                                  "rorw   #8,%0\n"       \
+                                                  "movel  %0,%1@+\n"     \
+                                    : "=d"(r), "=a"(c)                   \
+                                    : "0"(r), "1"(c)); })
 #   endif
 #  endif
 #  if defined(__i386) || defined(__i386__) || defined(__x86_64) || defined(__x86_64__)

