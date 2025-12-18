# Librairie Dynamique GEM
# Olivier Landemarre, Dominique B‚r‚ziat, Arnaud Bercegeay
# tous droits r‚serv‚s 1997/1998/1999/2000/2001/2002
#
# makefile to build ldginfo

ifeq ($(CROSS),yes)
CC      = m68k-atari-mint-gcc
LD      = m68k-atari-mint-gcc
STRIP      = m68k-atari-mint-strip
else
CC      = gcc
LD      = gcc
STRIP     = strip
endif
CFLAGS  = -I../../include -I../share -O2
LDFLAGS = 
LDLIBS  = -lgem
STARTUP = 

TARGET  = ../../bin/68k/ldginfo.ttp
TARGETV4E  = ../../bin/coldfire/ldginfo.ttp

all: clean $(TARGET) clean 

$(TARGET):ldginfo 

ldginfo:; 
	$(CC) -c $(CFLAGS) ldginfo.c -o ldginfo.o
	$(CC) -c $(CFLAGS) ../share/internal.c -o internal.o
	$(LD) $(LDFLAGS) -o$@ ldginfo.o internal.o $(STARTUP) $(LDLIBS)  ../../lib/gcc/libldg.a 
	$(STRIP) $@
	mv ldginfo $(TARGET)
	$(RM) $@	
	$(CC) -c $(CFLAGS) -mcpu=5475 ldginfo.c -o ldginfo.o
	$(CC) -c $(CFLAGS) -mcpu=5475 ../share/internal.c -o internal.o
	$(LD) $(LDFLAGS) -mcpu=5475 -o$@ ldginfo.o internal.o $(STARTUP) $(LDLIBS) ../../lib/gcc/m5475/libldg.a
	$(STRIP) $@
	mv ldginfo $(TARGETV4E)
	$(RM) $@
	

clean:;
	$(RM) ldginfo
	$(RM) ldginfo.o internal.o

