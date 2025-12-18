# Librairie Dynamique GEM
# Olivier Landemarre, Dominique B‚r‚ziat, Arnaud Bercegeay
# tous droits r‚serv‚s 1997/1998/1999/2000/2001/2002/2012
#
# makefile to build LDG manager

CC      = $(CC_PUREC)
CFLAGS  = -O -W0,-P
LDFLAGS = -Wl,-V,-S=4096 -s $(PURECLIB)/gem $(PURECLIB)/pctoslib ../../lib/purec/ldg.lib
STARTUP = $(PURECLIB)/PCSTART.O
TARGET  = ../../gemsys/ldg/ldgmgrpc.app
OBJDIR   = .purec
OBJS = ldgmgr.o internal.o
OBJECTS = $(addprefix $(OBJDIR)/,$(OBJS))
TARGETD = $(dir $(TARGET))

all: $(TARGET)

$(OBJDIR)/%.o: %.c $(OBJDIR)
	@echo CC $@
	@$(CC) $(CFLAGS) -O=$@ $< 

$(TARGET): $(OBJECTS) $(TARGETD)
	@echo LD $@
	$(LD) $(LDFLAGS) -O=$@ $(OBJECTS) $(STARTUP) $(LDLIBS)
	@chmod +x $@

internal.o: ..\share\internal.c

# dependencies
$(OBJDIR):; mkdir $(OBJDIR)
$(TARGETD):; mkdir -p $@

clean:
	$(RM) -r $(OBJDIR)



