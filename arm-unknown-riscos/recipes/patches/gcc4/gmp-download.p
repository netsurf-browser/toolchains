--- gcc4/Makefile.orig	2018-01-25 22:42:43.937180762 +0000
+++ gcc4/Makefile	2018-01-25 22:44:46.680863288 +0000
@@ -703,7 +703,7 @@
 # Download mpc source:
 $(SRCORIGDIR)/mpc-$(MPC_VERSION).tar.gz:
 	-mkdir -p $(SRCORIGDIR)
-	cd $(SRCORIGDIR) && wget -c https://ftpmirror.gnu.org/mpc/mpc-$(MPC_VERSION).tar.gz
+	cd $(SRCORIGDIR) && wget -c https://ci.netsurf-browser.org/toolchain/arm-unknown-riscos/mpc-$(MPC_VERSION).tar.gz
 	touch $@
 
 # Download mpfr source:
