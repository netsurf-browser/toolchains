#!make -f
#
# $Id: Makefile 38 2005-11-02 21:07:44Z bercegeay $

all: gcc sox pc

lib-gcc:
	make -f gcc.mak lib-gcc.ldg

app-gcc:
	make -f gcc.mak app-gcc.prg

lib-sox:
	make -f sozobon.mak lib-sox.ldg

app-sox:
	make -f sozobon.mak app-sox.prg

lib-pc:
	make -f purec.mak lib-pc.ldg

app-pc:
	make -f purec.mak app-pc.prg

gcc: app-gcc lib-gcc 

sox: app-sox lib-sox

pc: app-pc lib-pc

clean:
	rm -f *.ldg *.prg
