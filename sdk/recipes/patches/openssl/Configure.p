--- Configure.orig	2017-10-17 14:53:18.369801491 +0100
+++ Configure	2017-10-17 15:11:04.312242542 +0100
@@ -1329,6 +1329,8 @@
 $target{cxxflags}//=$target{cflags} if $target{CXX};
 $target{exe_extension}=".exe" if ($config{target} eq "DJGPP");
 $target{exe_extension}=".pm"  if ($config{target} =~ /vos/);
+$target{exe_extension}=",e1f" if ($config{target} eq "arm-riscos-gnueabi");
+$target{exe_extension}=",e1f" if ($config{target} eq "arm-unknown-riscos");
 
 # Fill %config with values from %user, and in case those are undefined or
 # empty, use values from %target (acting as a default).
