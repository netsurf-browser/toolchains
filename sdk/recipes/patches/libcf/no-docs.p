--- Makefile.orig	2026-10-14 02:05:42.533913512 +0100
+++ Makefile	2026-10-14 02:06:51.218586068 +0100
@@ -104,9 +104,9 @@
 	install -m 755 -d $(PREFIX)/lib/m5475
 	install -m 644 libcflibv4e.a  ${PREFIX}/lib/m5475/libcflib.a
 endif
-	install -m 755 -d $(PREFIX)/stguide
-	install -m 644 doc/cflib.hyp      ${PREFIX}/stguide
-	install -m 644 doc/cflib.ref      ${PREFIX}/stguide
+	#install -m 755 -d $(PREFIX)/stguide
+	#install -m 644 doc/cflib.hyp      ${PREFIX}/stguide
+	#install -m 644 doc/cflib.ref      ${PREFIX}/stguide
 
 uninstall:
 	rm -f ${PREFIX}/include/cflib.h
