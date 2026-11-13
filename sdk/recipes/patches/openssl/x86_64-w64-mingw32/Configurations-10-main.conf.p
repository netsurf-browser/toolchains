--- Configurations/10-main.conf.orig
+++ Configurations/10-main.conf
@@ -1741,7 +1741,7 @@
         uplink_arch      => undef,
         perlasm_scheme   => "mingw64",
         shared_rcflag    => "--target=pe-x86-64",
-        multilib         => "64",
+        multilib         => "",
     },
 
     "mingwarm64" => {
