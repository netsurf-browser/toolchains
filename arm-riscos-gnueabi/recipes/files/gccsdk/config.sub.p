--- config.sub.orig	2022-06-01 07:47:44.364644855 +0100
+++ config.sub	2022-06-01 07:48:02.892796689 +0100
@@ -135,7 +135,7 @@
 			| linux-newlib* | linux-musl* | linux-uclibc* | uclinux-uclibc* \
 			| uclinux-gnu* | kfreebsd*-gnu* | knetbsd*-gnu* | netbsd*-gnu* \
 			| netbsd*-eabi* | kopensolaris*-gnu* | cloudabi*-eabi* \
-			| storm-chaos* | os2-emx* | rtmk-nova*)
+			| storm-chaos* | os2-emx* | rtmk-nova* | riscos-gnu*)
 				basic_machine=$field1
 				os=$maybe_os
 				;;
