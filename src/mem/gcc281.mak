CC      = $(CC_GCC281)
AR      = $(AR_GCC281)

CFLAGS  = -I..\..\include -O
LDFLAGS = -L..\..\lib\gcc281 -lldg -lmgem -lmem

LIB32   = ..\..\lib\gcc281\mem.olb
LIB16   = ..\..\lib\gcc281\mem16.olb

all: $(LIB16) $(LIB32)

$(LIB32): mem.o
	rm -f $(LIB32)
	$(AR) -r $(LIB32) mem32.o

$(LIB16): mem16.o
	rm -f $(LIB16)
	$(AR) -r $(LIB16) mem16.o

mem16.o: mem.c
	$(CC) -c $< -o $@ -mshort $(CFLAGS)

test: test.o
	$(CC) -o test.ttp test.o $(LDFLAGS)

clean:
	$(RM) -f mem.o mem16.o


