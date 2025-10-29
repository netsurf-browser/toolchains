--- src/globals.h.orig	2023-10-01 03:02:16.247962097 +0100
+++ src/globals.h	2023-10-01 03:03:20.403979699 +0100
@@ -806,6 +806,8 @@
 	RSC header and support struct
 *******************************************************************************/
 
+#ifndef __RSXHDR
+#define __RSXHDR
 typedef struct {		/* fichier ressource tendu */
   UWORD rsh_vrsn;			/* should be 3														     */
   UWORD rsh_extvrsn;		/* not used, initialised to 'IN' for Interface */
@@ -827,6 +829,7 @@
   ULONG rsh_nimages;
   ULONG rsh_rssize;			/* total bytes in resource   */
 } RSXHDR;
+#endif
 
 typedef struct {
 	unsigned long	rlen;
