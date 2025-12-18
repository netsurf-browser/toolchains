CC      = cc
CFLAGS  = -W0,-P+ -I..\..\include
LDFLAGS = -L..\..\lib\purec -lgem -lldg -lwindom -lwout
TARGET  = ..\..\install.app

all:$(TARGET)

$(TARGET): install.o viewtext.o
	$(CC) install.o viewtext.o -o $(TARGET) $(LDFLAGS)

viewtext.o: ..\..\..\viewtext\viewtext.c

clean:
	$(RM) -f install.o viewtext.o
