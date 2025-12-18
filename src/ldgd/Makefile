#
# Librairie Dynamique GEM
# Olivier Landemarre, Dominique B‚r‚ziat, Arnaud Bercegeay
# tous droits r‚serv‚s 1997-2003
#
# Makefile to build LDG deamon
#
# $Id: Makefile 9 2005-07-25 19:37:25Z bercegeay $
#

CC     = cc
CFLAGS = -O -W0,-P
LDFLAGS= -Wl,-V,-S=4096 -s -lgem -lpctoslib ..\..\lib\purec\ldg.lib
TARGET = ..\..\gemsys\magic\start\ldgd.app

all: $(TARGET)

$(TARGET): ldgd.o ..\..\lib\purec\ldg.lib
	$(CC) ldgd.o -o $(TARGET) $(LDFLAGS)

clean:
	$(RM) -f ldgd.o
