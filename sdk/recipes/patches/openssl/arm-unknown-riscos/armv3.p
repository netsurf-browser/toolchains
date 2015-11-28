--- crypto/arm_arch.h	2015-07-09 12:21:24.000000000 +0000
+++ crypto/arm_arch.h	2015-11-13 13:47:02.386910047 +0000
@@ -31,6 +31,8 @@
 #    define __ARM_ARCH__ 5
 #   elif defined(__ARM_ARCH_4__) || defined(__ARM_ARCH_4T__)
 #    define __ARM_ARCH__ 4
+#   elif defined(__ARM_ARCH_3__)
+#    define __ARM_ARCH__ 3
 #   else
 #    error "unsupported ARM architecture"
 #   endif
--- crypto/armcap.c	2015-07-09 12:21:24.000000000 +0000
+++ crypto/armcap.c	2015-11-13 15:12:20.385640267 +0000
@@ -67,13 +67,15 @@
     sigprocmask(SIG_SETMASK, &ill_act.sa_mask, &oset);
     sigaction(SIGILL, &ill_act, &ill_oact);
 
-    if (sigsetjmp(ill_jmp, 1) == 0) {
-        _armv7_neon_probe();
-        OPENSSL_armcap_P |= ARMV7_NEON;
-    }
-    if (sigsetjmp(ill_jmp, 1) == 0) {
-        _armv7_tick();
-        OPENSSL_armcap_P |= ARMV7_TICK;
+    if (0) {
+        if (sigsetjmp(ill_jmp, 1) == 0) {
+            _armv7_neon_probe();
+            OPENSSL_armcap_P |= ARMV7_NEON;
+        }
+        if (sigsetjmp(ill_jmp, 1) == 0) {
+            _armv7_tick();
+            OPENSSL_armcap_P |= ARMV7_TICK;
+        }
     }
 
     sigaction(SIGILL, &ill_oact, NULL);
--- crypto/modes/asm/ghash-armv4.pl	2015-07-09 12:21:24.000000000 +0000
+++ crypto/modes/asm/ghash-armv4.pl	2015-11-13 13:44:26.274913291 +0000
@@ -157,7 +157,14 @@
 	ldmia	$Thh,{$Tll-$Thh}	@ load Htbl[nhi]
 	add	$nhi,$nhi,$nhi
 	eor	$Zll,$Tll,$Zll,lsr#4
+#if __ARM_ARCH__>=4
 	ldrh	$Tll,[sp,$nhi]		@ rem_4bit[rem]
+#else
+	ldrb	$Tll,[sp,$nhi]
+	add	$nhi,$nhi,#1
+	ldrb	$nhi,[sp,$nhi]
+	orr	$Tll,$Tll,$nhi,lsl#8
+#endif
 	eor	$Zll,$Zll,$Zlh,lsl#28
 	ldrb	$nhi,[$Xi,#14]
 	eor	$Zlh,$Tlh,$Zlh,lsr#4
@@ -180,7 +187,14 @@
 	eor	$Zll,$Zll,$Zlh,lsl#28
 	eor	$Zlh,$Tlh,$Zlh,lsr#4
 	eor	$Zlh,$Zlh,$Zhl,lsl#28
+#if __ARM_ARCH__>=4
 	ldrh	$Tll,[sp,$nlo]		@ rem_4bit[rem]
+#else
+	ldrb	$Tll,[sp,$nlo]
+	add	$Tlh,$nlo,#1
+	ldrb	$Tlh,[sp,$Tlh]
+	orr	$Tll,$Tll,$Tlh,lsl#8
+#endif
 	eor	$Zhl,$Thl,$Zhl,lsr#4
 	ldrplb	$nlo,[$inp,$cnt]
 	eor	$Zhl,$Zhl,$Zhh,lsl#28
@@ -195,7 +209,14 @@
 	ldrplb	$Tll,[$Xi,$cnt]
 	eor	$Zll,$Zll,$Zlh,lsl#28
 	eor	$Zlh,$Tlh,$Zlh,lsr#4
+#if __ARM_ARCH__>=4
 	ldrh	$Tlh,[sp,$nhi]
+#else
+	ldrb	$Tlh,[sp,$nhi]
+	add	$nhi,$nhi,#1
+	ldrb	$nhi,[sp,$nhi]
+	orr	$Tlh,$Tlh,$nhi,lsl#8
+#endif
 	eor	$Zlh,$Zlh,$Zhl,lsl#28
 	eor	$Zhl,$Thl,$Zhl,lsr#4
 	eor	$Zhl,$Zhl,$Zhh,lsl#28
@@ -245,7 +266,14 @@
 	ldmia	$Thh,{$Tll-$Thh}	@ load Htbl[nhi]
 	add	$nhi,$nhi,$nhi
 	eor	$Zll,$Tll,$Zll,lsr#4
+#if __ARM_ARCH__>=4
 	ldrh	$Tll,[$rem_4bit,$nhi]	@ rem_4bit[rem]
+#else
+	ldrb	$Tll,[$rem_4bit,$nhi]
+	add	$nhi,$nhi,#1
+	ldrb	$nhi,[$rem_4bit,$nhi]
+	orr	$Tll,$Tll,$nhi,lsl#8
+#endif
 	eor	$Zll,$Zll,$Zlh,lsl#28
 	eor	$Zlh,$Tlh,$Zlh,lsr#4
 	eor	$Zlh,$Zlh,$Zhl,lsl#28
@@ -266,7 +294,14 @@
 	eor	$Zll,$Zll,$Zlh,lsl#28
 	eor	$Zlh,$Tlh,$Zlh,lsr#4
 	eor	$Zlh,$Zlh,$Zhl,lsl#28
+#if __ARM_ARCH__>=4
 	ldrh	$Tll,[$rem_4bit,$nlo]	@ rem_4bit[rem]
+#else
+	ldrb	$Tll,[$rem_4bit,$nlo]
+	add	$Tlh,$nlo,#1
+	ldrb	$Tlh,[$rem_4bit,$Tlh]
+	orr	$Tll,$Tll,$Tlh,lsl#8
+#endif
 	eor	$Zhl,$Thl,$Zhl,lsr#4
 	ldrplb	$nlo,[$Xi,$cnt]
 	eor	$Zhl,$Zhl,$Zhh,lsl#28
@@ -280,7 +315,14 @@
 	eor	$Zll,$Tll,$Zll,lsr#4
 	eor	$Zll,$Zll,$Zlh,lsl#28
 	eor	$Zlh,$Tlh,$Zlh,lsr#4
+#if __ARM_ARCH__>=4
 	ldrh	$Tll,[$rem_4bit,$nhi]	@ rem_4bit[rem]
+#else
+	ldrb	$Tll,[$rem_4bit,$nhi]
+	add	$Tlh,$nhi,#1
+	ldrb	$Tlh,[$rem_4bit,$Tlh]
+	orr	$Tll,$Tll,$Tlh,lsl#8
+#endif
 	eor	$Zlh,$Zlh,$Zhl,lsl#28
 	eor	$Zhl,$Thl,$Zhl,lsr#4
 	eor	$Zhl,$Zhl,$Zhh,lsl#28
