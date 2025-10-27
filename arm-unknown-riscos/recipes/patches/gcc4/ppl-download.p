--- gcc4/Makefile.orig	2018-01-26 08:03:49.745541843 +0000
+++ gcc4/Makefile	2018-01-26 08:05:15.256056177 +0000
@@ -586,9 +586,9 @@
 	touch $(BUILDSTEPSDIR)/$@
 
 # Unpack & copy ppl source:
-src-ppl-copied: $(SRCORIGDIR)/ppl-$(PPL_VERSION).tar.gz
+src-ppl-copied: $(SRCORIGDIR)/ppl-$(PPL_VERSION).tar.xz
 	-rm -rf $(SRCORIGDIR)/ppl-$(PPL_VERSION) $(SRCDIR)/ppl
-	cd $(SRCORIGDIR) && tar xf $<
+	cd $(SRCORIGDIR) && tar axf $<
 	-mkdir -p $(SRCDIR)/ppl
 	cp -T -p -r $(SRCORIGDIR)/ppl-$(PPL_VERSION) $(SRCDIR)/ppl
 	cd $(SRCDIR)/ppl && PATH="$(PREFIX_BUILDTOOL_GCC)/bin:$(PATH)" && $(SCRIPTSDIR)/do-patch-and-copy $(RECIPEDIR)
@@ -713,10 +713,9 @@
 	touch $@
 
 # Download ppl source:
-$(SRCORIGDIR)/ppl-$(PPL_VERSION).tar.gz:
+$(SRCORIGDIR)/ppl-$(PPL_VERSION).tar.xz:
 	-mkdir -p $(SRCORIGDIR)
-	cd $(SRCORIGDIR) && wget -c https://www.bugseng.com/external/ppl/download/ftp/releases/$(PPL_VERSION)/ppl-$(PPL_VERSION).tar.gz
-	touch $@
+	wget -O $@ -c https://ci.netsurf-browser.org/toolchain/arm-unknown-riscos/ppl-$(PPL_VERSION).tar.xz
 
 # Download CLooG source:
 $(SRCORIGDIR)/cloog-ppl-$(CLOOG_VERSION).tar.gz:
