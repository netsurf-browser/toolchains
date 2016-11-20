--- /home/chris/netsurf/toolchains/m68k-unknown-amigaos/builddir/clib2/stdlib_memory.h	2016-11-18 16:22:21.000000000 +0000
+++ stdlib_memory.h	2016-11-20 14:56:44.477019674 +0000
@@ -40,7 +40,7 @@
  * Uncomment this to build a library which has the memory debugging features
  * enabled.
  */
-/*#define __MEM_DEBUG*/
+#define __MEM_DEBUG
 
 /****************************************************************************/
 
@@ -48,7 +48,7 @@
  * Uncomment this to see reports of where and how much memory is allocated
  * or released.
  */
-/*#define __MEM_DEBUG_LOG*/
+#define __MEM_DEBUG_LOG
 
 /****************************************************************************/
 
@@ -56,7 +56,7 @@
  * Uncomment this to speed up memory data maintenance operations when
  * the memory debugging mode is enabled.
  */
-/*#define __USE_MEM_TREES*/
+#define __USE_MEM_TREES
 
 /****************************************************************************/
 
