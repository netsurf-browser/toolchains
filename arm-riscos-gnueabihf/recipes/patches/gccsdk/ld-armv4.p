Index: ld/d-link/Makefile.am
===================================================================
--- ld/d-link/Makefile.am	(revision 7698)
+++ ld/d-link/Makefile.am	(working copy)
@@ -68,7 +68,7 @@
 
 ld_riscos_eabihf_so_CPPFLAGS = -I$(top_srcdir)/d-link -I$(top_srcdir)/d-link/riscos \
 		      -DNO_UNDERSCORE -DVERBOSE_DLINKER -D__TARGET_SOLOADER__
-ld_riscos_eabihf_so_CFLAGS = -fPIC -O4 -Wall -std=gnu99 -march=armv7-a+fp+neon
+ld_riscos_eabihf_so_CFLAGS = -fPIC -O4 -Wall -std=gnu99 -march=armv4
 ld_riscos_eabihf_so_LDFLAGS = --shared -Wl,--no-undefined -nostdlib -nostartfiles -Wl,-e -Wl,_dl_start \
 			-Wl,-soname -Wl,ld-riscos/so/2
 
