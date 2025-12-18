#
# Librairie Dynamique GEM
# Olivier Landemarre, Dominique B‚r‚ziat, Arnaud Bercegeay
# tous droits r‚serv‚s 1997-2012
#
# Makefile to build LDG deamon
#
#

ifeq ($(CROSS),yes)
CC     = m68k-atari-mint-gcc
LD     = m68k-atari-mint-gcc
else
CC     = gcc
LD     = gcc
endif

CFLAGS = -mcpu=5475 -W -Wall -O -I../../include -I.. -I.
STARTUP = 
LDFLAGS= -v $(M68K_ATARI_MINT_LDFLAGS) -mcpu=5475
LDLIBS = ../../lib/gcc/m5475/libldg.a -lgem
OBJDIR   = .gcc
OBJS = ldgmgr.o internal.o
OBJECTS = $(addprefix $(OBJDIR)/,$(OBJS))
TARGET = ../../gemsys/ldg/ldgmgrcf.app
TARGETD = $(dir $(TARGET))

all: $(TARGET)

$(OBJDIR)/%.o: %.c $(OBJDIR)
	@echo CC $@
	@$(CC) -c $(CFLAGS) $< -o $@

$(TARGET): $(OBJECTS) $(TARGETD)
	@echo LD $@
	$(LD) $(LDFLAGS) -o$@ $(OBJECTS) $(STARTUP) $(LDLIBS)
	@chmod +x $@

internal.o: ../share/internal.c

# dependencies
$(OBJDIR):; mkdir $(OBJDIR)
$(TARGETD):; mkdir -p $@

clean:
	$(RM) -r $(OBJDIR)
