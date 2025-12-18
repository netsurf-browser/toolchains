# Example of LDG library programming 
# Compile with gcc in a Unix/MiNT environnement

# Undefined with variable to use cross compilation
PREFIX_CROSS=m68k-atari-mint-

CC      = $(PREFIX_CROSS)gcc
CFLAGS  = -I../../include -O
LDFLAGS = -L../../lib/gcc295 
LDLIBS  = -lgem -lm -lldg

ex_lib.ldg: ex_lib; cp $< $@
ex_lib: ex_lib.o
clean:; $(RM) ex_lib.o ex_lib

