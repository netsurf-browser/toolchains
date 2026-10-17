--- Makefile.old	2012-08-04 20:05:38.000000000 +0100
+++ Makefile	2012-08-04 20:12:39.000000000 +0100
@@ -0,0 +1,14 @@
+SOURCES := $(addprefix src/src/,pblPriorityQueue.c pblHeap.c pblMap.c pblSet.c pblList.c pblCollection.c pblIterator.c pblStringBuilder.c pblisam.c pblkf.c pblhash.c pbl.c)
+
+OBJECTS := $(subst .c,.o,$(SOURCES))
+
+install: libpbl.a
+	install -m 644 libpbl.a $(PREFIX)/lib
+	install -m 644 src/src/pbl.h $(PREFIX)/include
+
+libpbl.a: $(OBJECTS)
+	$(AR) rc $@ $(OBJECTS)
+
+%.o: %.c
+	$(CC) -c -Wall -O3 -o $@ $<
+
