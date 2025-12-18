# Library example : a GEM popup function.
# Makefile for compiling with gcc and an unix environnement

# Undefined with variable to use cross compilation
PREFIX_CROSS=m68k-atari-mint-

CC      = $(PREFIX_CROSS)gcc
CFLAGS  = -I../../include -O
LDFLAGS = -L../../lib/gcc281 
LDLIBS  = -lgem -lldg

popup.ldg: popup; cp $< $@
popup: popup.o
clean:; $(RM) popup.o popup
