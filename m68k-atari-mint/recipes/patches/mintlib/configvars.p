--- ./configvars.orig	2012-05-03 19:26:10.000000000 +0200
+++ ./configvars	2012-11-05 22:34:07.000000000 +0100
@@ -12,7 +12,7 @@
 # get installed.  You have to take car yourself that the libraries
 # and include files and so on will get installed in the right place
 # The easiest way to do this is to change "prefix", see below.
-#CROSS=yes
+CROSS=yes
 
 # Silent build or verbose
 AM_DEFAULT_VERBOSITY = 1
@@ -64,7 +64,7 @@
 # This variable is used for various others.  Change its value if 
 # need be.  When cross-compiling you will usually want to set this
 # to "/usr/m68k-atari-mint".
-prefix=/usr
+prefix=/opt/netsurf/m68k-atari-mint/cross/m68k-atari-mint
 
 # This is where include files get installed.
 includedir=${prefix}/include
@@ -141,7 +141,7 @@
 LDFLAGS=
 
 # Additional defines.
-DEFS=
+DEFS=-DREGEX_MALLOC
 
 # Define this to the warning level you want.
 WARN=-Wall -Werror -Wstrict-prototypes -Wmissing-prototypes -Wold-style-declaration -Wold-style-definition
