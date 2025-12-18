#
# Example with Sozobon X
#

CC      = zcc		# Warning, my Sozobon compiler has a z prefix
CFLAGS  = -O
LDFLAGS = -lgem -L../../lib/sozobon -lldg

all: popup.ldg

popup.ldg: popup.o
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	$(RM) popup.o
