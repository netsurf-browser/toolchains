--- /dev/null	2017-10-10 15:19:45.611896396 +0100
+++ Configurations/50-amigaos.conf	2017-10-17 15:42:25.304921197 +0100
@@ -0,0 +1,32 @@
+my %targets = (
+
+    "ppc-amigaos" => {
+        inherit_from     => [ "BASE_unix" ],
+        cc               => "ppc-amigaos-gcc",
+        cflags           => picker(default => "-DB_ENDIAN -DTERMIOS -DNO_SYS_UN_H -DNO_SYSLOG",
+                                   debug   => "-O0 -g",
+                                   release => "-O3 -fomit-frame-pointer",
+                                   threads("-D_REENTRANT")),
+        thread_scheme    => "(unknown)",
+        asm_arch         => "ppc32",
+        perlasm_scheme   => "linux32",
+        sys_id           => "AMIGAOS4",
+        bn_ops           => "THIRTY_TWO_BIT BN_LLONG RC4_CHAR",
+        ranlib           => "ppc-amigaos-ranlib",
+    },
+
+    "m68k-unknown-amigaos" => {
+        inherit_from     => [ "BASE_unix" ],
+        cc               => "m68k-unknown-amigaos-gcc",
+        cflags           => picker(default => "-DB_ENDIAN -DTERMIOS -DNO_SYS_UN_H -DNO_SYS_PARAM_H -DNO_SYSLOG -m68020 -std=gnu99",
+                                   debug   => "-O0 -g",
+                                   release => "-O3 -fomit-frame-pointer",
+                                   threads("-D_REENTRANT")),
+        thread_scheme    => "(unknown)",
+        ex_libs          => add("-lm -lnet"),
+        sys_id           => "AMIGAOS3",
+        bn_ops           => "THIRTY_TWO_BIT BN_LLONG",
+        bn_asm_src       => "asm/bn_m68k.s",
+        ranlib           => "m68k-unknown-amigaos-ranlib",
+    },
+);
