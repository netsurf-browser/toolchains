#!make -f
#
# $Id: purec.mak 38 2005-11-02 21:07:44Z bercegeay $

CC     = $(CC_PUREC)
LNK    = $(AR_PUREC)

CFLAGS   = -P -I../../include -I. -I$(PURECINC)
ARFLAGS  = -J

app-pc.prg: client.c interface.h 
	$(CC) $(CFLAGS) -Otmppc.o client.c
	$(LNK) -V -S=32000 -O=$@ $(PURECLIB)/pcstart.o tmppc.o \
	       ../../lib/purec/ldg.lib $(PURECLIB)/gem.lib $(PURECLIB)/pcstdlib.lib \
		   $(PURECLIB)/pctoslib.lib
	rm -f tmppc.o
	chmod +x $@


lib-pc.ldg: lib.c interface.h
	$(CC) $(CFLAGS) -Otmppc.o lib.c
	$(LNK) -V -S=32000 -O=$@ $(PURECLIB)/pcstart.o tmppc.o \
	       ../../lib/purec/ldg.lib $(PURECLIB)/gem.lib $(PURECLIB)/pcstdlib.lib \
		   $(PURECLIB)/pctoslib.lib
	rm -f tmppc.o
	chmod +x $@
