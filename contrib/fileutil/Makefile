CC 		= cc
AR		= plink -J

CFLAGS	= -O -W0,-P -I..\..\include
LDFLAGS	= -s -lgem ..\..\lib\purec\ldg.lib

OBJECTS	  = fileutil.o 

LDGTARGET = ..\..\gemsys\ldg\fileutil.ldg

all: $(LDGTARGET)

$(LDGTARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) $(OBJECTS)
