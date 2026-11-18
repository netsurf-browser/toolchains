--- src/spng/CMakeLists.txt.orig	2026-11-19 00:52:04.955851455 +0000
+++ src/spng/CMakeLists.txt	2026-11-19 00:52:38.344326720 +0000
@@ -38,7 +38,7 @@
 if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
   target_compile_options(spng-static PRIVATE -w)
   if(NOT WIN32)
-    target_compile_options(spng-static PRIVATE -fvisibility=hidden)
+    # target_compile_options(spng-static PRIVATE -fvisibility=hidden)
   endif()
 endif()
 if(CMAKE_C_COMPILER_ID MATCHES SunPro)
