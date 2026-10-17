--- configure.orig	2026-10-13 14:47:08.503648278 +0100
+++ configure	2026-10-13 14:47:18.427745247 +0100
@@ -245,7 +245,7 @@
 if test "$gcc" -eq 1 && ($cc -c $test.c) >> configure.log 2>&1; then
   echo ... using gcc >> configure.log
   CC="$cc"
-  CFLAGS="${CFLAGS--O3} -fPIC"
+  CFLAGS="${CFLAGS--O3}"
   SFLAGS="${CFLAGS--O3}"
   if test "$ARCHS"; then
     CFLAGS="${CFLAGS} ${ARCHS}"
--- Makefile.in.orig	2026-10-13 14:48:49.104630963 +0100
+++ Makefile.in	2026-10-13 14:48:59.472732196 +0100
@@ -138,7 +138,7 @@
 
 match.lo: match.S
 	$(CPP) match.S > _match.s
-	$(CC) -c -fPIC _match.s
+	$(CC) -c _match.s
 	mv _match.o match.lo
 	rm -f _match.s
 
