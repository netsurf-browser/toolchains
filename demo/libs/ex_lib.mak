#
#  Example with Pure C (Pure C has a good math library)
#

CC      = gcc
CFLAGS  = -I..\..\include -O
LDFLAGS = -L..\..\lib\gcc281 -lgem -lm -lldg

all: ex_lib.ldg

ex_lib.ldg: ex_lib.o
	$(CC) $< -o $@ $(LDFLAGS)

clean:
	$(RM) ex_lib.o

