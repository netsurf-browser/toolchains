--- /dev/null	2017-10-10 15:19:45.611896396 +0100
+++ Configurations/50-riscos.conf	2017-10-17 15:42:25.304921197 +0100
@@ -0,0 +1,14 @@
+%targets = (
+
+    "arm-unknown-riscos" => {
+        inherit_from     => [ "BASE_unix" ],
+        cc               => "arm-unknown-riscos-gcc",
+        cflags           => picker(default => "-DL_ENDIAN -DTERMIOS -DNO_SYS_UN_H",
+                                   debug   => "-O0 -g",
+                                   release => "-O3 ",
+				   threads("-D_REENTRANT")),
+        thread_scheme    => "(unknown)",
+        sys_id           => "RISCOS",
+        bn_ops           => "BN_LLONG",
+    },
+);
