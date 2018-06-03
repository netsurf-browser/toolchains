--- /dev/null	2018-06-01 10:46:49.956110101 +0200
+++ Configurations/50-atari.conf	2018-06-03 03:10:55.000000000 +0200
@@ -0,0 +1,42 @@
+%targets = (
+    "atari-common" => {
+        inherit_from     => [ "BASE_unix" ],
+        template         => 1,
+        cc               => "m68k-atari-mint-gcc",
+        cflags           => add_before(picker(default => "-DB_ENDIAN -DTERMIOS -DNO_SYSLOG -DNO_SYS_UN_H -DOPENSSL_USE_IPV6=0 -Wall",
+                                              debug   => "-g -O0",
+                                              release => "-O3 -fomit-frame-pointer")),
+        sys_id           => "MINT",
+        ranlib           => "m68k-atari-mint-ranlib",
+# ?        bn_ops           => "THIRTY_TWO_BIT BN_LLONG",
+        bn_ops           => "BN_LLONG",
+#        ex_libs          => "-lnetwork",
+#        perlasm_scheme   => "elf",
+        thread_scheme    => "(unknown)",
+#        dso_scheme       => "dlfcn",
+#        shared_target    => "gnu-shared",
+#        shared_cflag     => "-fPIC",
+#        shared_ldflag    => "-shared",
+#        shared_extension => ".so.\$(SHLIB_MAJOR).\$(SHLIB_MINOR)",
+    },
+
+    "m68k-atari-mint000" => {
+        inherit_from     => [ "atari-common" ],
+# XXX: does it work on 68000?
+#        bn_asm_src       => "asm/bn_m68k.s",
+    },
+
+    "m68k-atari-mint" => {
+        inherit_from     => [ "atari-common" ],
+        cflags           => add(picker(release => "-m68020-60")),
+        bn_asm_src       => "asm/bn_m68k.s",
+    },
+
+    "m5475-atari-mint" => {
+        inherit_from     => [ "atari-common" ],
+        cc               => "m5475-atari-mint-gcc",
+#        bn_asm_src       => "asm/bn_m68k.s",
+        bn_ops           => "BN_LLONG DES_RISC1",
+        ranlib           => "m5475-atari-mint-ranlib",
+    },
+);
