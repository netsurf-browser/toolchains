--- configure	2023-12-05 14:57:46.000000000 +0000
+++ configure	2024-09-23 12:22:25.036000000 +0100
@@ -44576,7 +44576,7 @@ fi
              CFLAGS="$CFLAGS -qthreaded"
            fi
            ;;
-        powerpc-*amigaos*)
+        *-amigaos*)
                       PTHREAD=" -lpthread -lamiga"
            ;;
         *)
