--- crypto/bn/Makefile	2017-01-26 13:22:03.000000000 +0000
+++ crypto/bn/Makefile	2017-02-11 15:24:31.133017711 +0000
@@ -60,6 +60,9 @@ lib:	$(LIBOBJ)
 	$(RANLIB) $(LIB) || echo Never mind.
 	@touch lib
 
+bn_m68k.o:	asm/bn_m68k.s
+	$(CC) $(CFLAGS) -c -o $@ asm/bn_m68k.s
+
 bn-586.s:	asm/bn-586.pl ../perlasm/x86asm.pl
 	$(PERL) asm/bn-586.pl $(PERLASM_SCHEME) $(CFLAGS) $(PROCESSOR) > $@
 co-586.s:	asm/co-586.pl ../perlasm/x86asm.pl
