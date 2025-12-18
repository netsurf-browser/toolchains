#  Example with Gcc with MiNT environnement.

# Undefined with variable to use cross compilation
ifeq ($(CROSS),yes)
PREFIX_CROSS=m68k-atari-mint-
endif

CC      = $(PREFIX_CROSS)gcc
CFLAGS  = -O2 -I../../include -I../libs -I/home/bereziat/Aranym/src/cvs/windom/include
LDFLAGS = -L../../lib/gcc295 -L/home/bereziat/Aranym/src/cvs/windom/lib/gcc
LDLIBS  = -lldg -lwindom -lgem
OBJECTS = client.o ../libs/exlibbnd.o ../libs/popupbnd.o

client.app: client; cp $< $@
client: $(OBJECTS)
clean:; $(RM) -f *.o client

