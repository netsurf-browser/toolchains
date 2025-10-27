Index: ro-install
===================================================================
--- ro-install	(revision 7698)
+++ ro-install	(working copy)
@@ -72,7 +72,7 @@
 
     # Check that static libraries really do contain static objects
     if grep -q "\.a\$" <<< $param || grep -q "\.a-static" <<< $param ; then
-      if GCCSDK_BIN/arm-unknown-riscos-objdump -p $param | grep -q "position independent"; then
+      if GCCSDK_BIN/arm-riscos-gnueabi-objdump -p $param | grep -q "position independent"; then
         echo "RISC OS Cross installer: static archive $param contains position independent code" 1>&2
         exit 1
       fi
