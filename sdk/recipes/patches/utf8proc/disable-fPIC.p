--- CMakeLists.txt.orig
+++ CMakeLists.txt
@@ -46,7 +46,7 @@ if (NOT MSVC)
 endif ()
 
 set_target_properties (utf8proc PROPERTIES
-  POSITION_INDEPENDENT_CODE ON
+  POSITION_INDEPENDENT_CODE OFF
   VERSION "${SO_MAJOR}.${SO_MINOR}.${SO_PATCH}"
   SOVERSION ${SO_MAJOR}
 )
