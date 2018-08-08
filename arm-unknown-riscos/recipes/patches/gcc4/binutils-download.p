--- gcc4/Makefile.orig	2018-01-26 10:37:32.006594122 +0000
+++ gcc4/Makefile	2018-01-26 10:38:42.316723336 +0000
@@ -677,8 +677,7 @@
 # Download binutils source:
 $(SRCORIGDIR)/binutils-$(BINUTILS_VERSION).tar.bz2:
 	-mkdir -p $(SRCORIGDIR)
-	cd $(SRCORIGDIR) && wget -c http://ftpmirror.gnu.org/binutils/binutils-$(BINUTILS_VERSION).tar.bz2
-	touch $@
+	wget -O $@ -c http://ci.netsurf-browser.org/toolchain/arm-unknown-riscos/binutils-$(BINUTILS_VERSION).tar.bz2
 
 ifeq "$(GCC_USE_SCM)" "yes"
 # Checkout gcc source:
