--- CONFIGVARS	2011-05-13 23:38:45.000000000 +0100
+++ CONFIGVARS	2011-05-13 23:40:11.000000000 +0100
@@ -3,6 +3,7 @@
 # cross compiling
 #CROSS = yes
 #CROSS = no
+CROSS = yes
 
 # 
 # cross compiling
@@ -33,7 +34,7 @@
  CROSS := yes
 else
 ifeq ($(CROSS),yes)
- CROSS_TOOL=m68k-atari-mint
+ CROSS_TOOL=m5475-atari-mint
  CROSSPREFIX=$(CROSS_TOOL)-
 else
  CROSSPREFIX=
@@ -46,7 +47,7 @@
 NATIVECFLAGS = -O
 CRLF = echo crlf -s
 
-PREFIX=$(shell $(CROSSPREFIX)gcc -print-sysroot)/usr
+PREFIX=/opt/netsurf/m5475-atari-mint/cross/m5475-atari-mint
 ifeq ($(PREFIX),)
   PREFIX=/usr/$(CROSS_TOOL)
 endif
