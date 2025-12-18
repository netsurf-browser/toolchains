#
#  Example with Gcc
#

CC      = cc
CFLAGS  = -O -I..\..\include -I..\libs
LDFLAGS = -L..\..\lib\purec -lm -lldg -lwindom -lgem 
OBJECTS = client.o exlibbnd.o popupbnd.o

all: client.app

client.app: $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

exlibbnd.o: ..\libs\exlibbnd.c
	$(CC) -c $^ -o $@ $(CFLAGS)

popupbnd.o: ..\libs\popupbnd.c
	$(CC) -c $^ -o $@ $(CFLAGS)

clean:
	$(RM)  $(OBJECTS)

