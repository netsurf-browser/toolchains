#!make -f
#
# $Id: gcc.mak 37 2005-10-25 22:51:53Z bercegeay $

ifeq ($(CROSS),yes)
CC     = m68k-atari-mint-gcc
AR     = m68k-atari-mint-ar
else
CC     = gcc
AR     = ar
endif

CFLAGS   = -W -Wall -O -I../../include

app-gcc.prg: client.c interface.h 
	$(CC) $(CFLAGS) -o app-gcc.prg client.c ../../lib/gcc/libldg.a -lgem


lib-gcc.ldg: lib.c interface.h
	$(CC) $(CFLAGS) -o lib-gcc.ldg lib.c ../../lib/gcc/libldg.a -lgem



