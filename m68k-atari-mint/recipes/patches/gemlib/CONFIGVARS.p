--- CONFIGVARS	2011-05-13 23:38:45.000000000 +0100
+++ CONFIGVARS	2011-05-13 23:40:11.000000000 +0100
@@ -3,6 +3,7 @@
 # cross compiling
 #CROSS = yes
 #CROSS = no
+CROSS = yes
 
 # 
 # cross compiling
@@ -46,7 +47,7 @@
 NATIVECFLAGS = -O
 CRLF = echo crlf -s
 
-PREFIX=$(shell $(CROSSPREFIX)gcc -print-sysroot)/usr
+PREFIX=/opt/netsurf/m68k-atari-mint/cross/m68k-atari-mint
 ifeq ($(PREFIX),)
   PREFIX=/usr/$(CROSS_TOOL)
 endif
