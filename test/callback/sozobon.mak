#!make -f
#
# $Id: sozobon.mak 37 2005-10-25 22:51:53Z bercegeay $

CC     = $(CC_SOX)
AR     = $(AR_SOX)

CFLAGS   = -W -Wall -O -I../../include

app-sox.prg: client.c interface.h 
	$(CC) $(CFLAGS) -o app-sox.prg client.c ../../lib/sozobon/libldg.a -lgem


lib-sox.ldg: lib.c interface.h
	$(CC) $(CFLAGS) -o lib-sox.ldg lib.c ../../lib/sozobon/libldg.a -lgem



