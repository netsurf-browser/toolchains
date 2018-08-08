--- gcc4/Makefile.orig	2018-01-26 10:40:26.623881778 +0000
+++ gcc4/Makefile	2018-01-26 10:41:24.141623273 +0000
@@ -719,8 +719,7 @@
 # Download CLooG source:
 $(SRCORIGDIR)/cloog-ppl-$(CLOOG_VERSION).tar.gz:
 	-mkdir -p $(SRCORIGDIR)
-	cd $(SRCORIGDIR) && wget -c ftp://gcc.gnu.org/pub/gcc/infrastructure/cloog-ppl-$(CLOOG_VERSION).tar.gz
-	touch $@
+	wget -O $@ -c http://ci.netsurf-browser.org/toolchain/arm-unknown-riscos/cloog-ppl-$(CLOOG_VERSION).tar.gz
 
 # Download newlib source:
 $(SRCORIGDIR)/newlib-$(NEWLIB_VERSION).tar.gz:
