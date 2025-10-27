--- providers/implementations/rands/seeding/build.info.orig	2017-11-21 22:49:00.185608040 +0000
+++ providers/implementations/rands/seeding/build.info	2017-11-21 22:49:21.488219518 +0000
@@ -5,6 +5,9 @@
 IF[{- $config{target} =~ /vms/i -}]
   $COMMON=$COMMON rand_vms.c
 ENDIF
+IF[{- $config{target} =~ /amiga/i -}]
+  $COMMON=$COMMON rand_amiga.c
+ENDIF

 SOURCE[../../../libdefault.a]=$COMMON
 
