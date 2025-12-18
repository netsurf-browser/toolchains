#
# Librairie Dynamique GEM
# Olivier Landemarre, Dominique B‚r‚ziat, Arnaud Bercegeay
# tous droits r‚serv‚s 1997-2003
#
# Makefile to build LDG deamon
#
# $Id: Makefile 9 2005-07-25 19:37:25Z bercegeay $
#

CC     = $(CC_PUREC)
LD     = $(LD_PUREC)
CFLAGS = -P -G -I. -I.. -I../../include -I$(PURECINC) 
STARTUP = $(PURECLIB)/PCSTART.O
LDFLAGS= -V -S=4096 
LDLIBS = ../../lib/purec/ldg.lib $(PURECLIB)/gem.lib $(PURECLIB)/pctoslib.lib

OBJDIR   = .purec
OBJS = ldgd.o
OBJECTS = $(addprefix $(OBJDIR)/,$(OBJS))
TARGET = ../../daemon/purec/ldgd.app
TARGETD = $(dir $(TARGET))

all: $(TARGET)

$(OBJDIR)/%.o: %.c $(OBJDIR)
	@echo CC $@
	@$(CC) $(CFLAGS) -O=$@ $< 

$(TARGET): $(OBJECTS) $(TARGETD)
	@echo LD $@
	$(LD) $(LDFLAGS) -O=$@ $(OBJECTS) $(STARTUP) $(LDLIBS)
	@chmod +x $@

# dependencies
$(OBJDIR):; mkdir $(OBJDIR)
$(TARGETD):; mkdir -p $@

clean:
	$(RM) -r $(OBJDIR)
