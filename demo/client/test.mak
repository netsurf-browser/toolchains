CC = gcc
CFLAGS = -I../../include
LDFLAGS= -L../../lib/gcc
LDLIBS=  -lgem -lldg

test-path: test-path.o
