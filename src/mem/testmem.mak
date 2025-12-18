CC		= cc
CFLAGS  = -O -I..\..\include
LDFLAGS = -lgem  ..\..\lib\purec\ldg.lib
OBJECTS = testmem.o mem.o
TARGET  = testmem.ttp

all: $(TARGET) run

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

run:
	testmem 1000
	testmem 1000 2

clean:
	$(RM) -f $(OBJECTS)
