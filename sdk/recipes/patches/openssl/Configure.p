--- Configure.orig	2017-10-17 14:53:18.369801491 +0100
+++ Configure	2017-10-17 15:11:04.312242542 +0100
@@ -946,6 +946,7 @@
 $target{exe_extension}=".exe" if ($config{target} eq "DJGPP"
                                   || $config{target} =~ /^(?:Cygwin|mingw)/);
 $target{exe_extension}=".pm"  if ($config{target} =~ /vos/);
+$target{exe_extension}=",e1f" if ($config{target} eq "arm-unknown-riscos");
 
 ($target{shared_extension_simple}=$target{shared_extension})
     =~ s|\.\$\(SHLIB_MAJOR\)\.\$\(SHLIB_MINOR\)||;
