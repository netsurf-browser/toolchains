--- /dev/null	2017-10-10 15:19:45.611896396 +0100
+++ Configurations/50-windows.conf	2017-10-17 15:42:25.304921197 +0100
@@ -0,0 +1,26 @@
+my %targets = (
+
+    "i686-w64-mingw32" => {
+        inherit_from     => [ "BASE_unix",
+                sub { $disabled{shared} ? () : "x86_uplink" } ],
+        cc               => "gcc",
+        cflags           => combine(picker(default => "-DL_ENDIAN -DWIN32_LEAN_AND_MEAN -DUNICODE -D_UNICODE -m32 -Wall",
+                                           debug   => "-g -O0",
+                                           release => "-O3 -fomit-frame-pointer"),
+                                           threads("-D_MT")),
+        sys_id           => "MINGW32",
+        ex_libs          => add("-lws2_32 -lgdi32 -lcrypt32"),
+        bn_ops           => "BN_LLONG EXPORT_VAR_AS_FN",
+        thread_scheme    => "winthreads",
+        asm_arch         => "x86",
+        perlasm_scheme   => "coff",
+        dso_scheme       => "win32",
+        shared_target    => "mingw-shared",
+        shared_cflag     => add("-D_WINDLL"),
+        shared_ldflag    => "-static-libgcc",
+        shared_rcflag    => "--target=pe-i386",
+        shared_extension => ".dll",
+        multilib         => "",
+        apps_aux_src     => add("win32_init.c"),
+    },
+);
