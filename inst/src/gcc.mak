CC      = gcc
TARGET  = ../../install.app
CFLAGS  = -I. -I../../include
LDFLAGS = -L../../lib/gcc295
LDLIBS  = -lwout -lwindom -lgem -lldg

$(TARGET): install; mv $< $@ 
install: install.o viewtext.o
clean:; $(RM) -f install.o viewtext.o
