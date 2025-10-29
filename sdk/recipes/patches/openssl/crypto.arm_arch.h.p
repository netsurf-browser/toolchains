--- crypto/arm_arch.h  2015-07-09 12:21:24.000000000 +0000
+++ crypto/arm_arch.h  2015-11-13 13:47:02.386910047 +0000
@@ -51,6 +51,8 @@
 #    define __ARM_ARCH__ 5
 #   elif defined(__ARM_ARCH_4__) || defined(__ARM_ARCH_4T__)
 #    define __ARM_ARCH__ 4
+#   elif defined(__ARM_ARCH_3__)
+#    define __ARM_ARCH__ 3
 #   else
 #    error "unsupported ARM architecture"
 #   endif
