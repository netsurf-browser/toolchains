ifeq ($(CROSS),yes)
CROSSPREFIX = m68k-atari-mint
CC          = $(CROSSPREFIX)-gcc
AR          = $(CROSSPREFIX)-ar
PREFIX      = /usr/$(CROSSPREFIX)
else
CC          = gcc
AR          = ar
PREFIX      = /usr
endif

CFLAGS  = -I../../include -O2
LDFLAGS = -L../../lib/gcc295
LDLIBS  = -lldg -lmgem -lmem

OBJDIR  = gcc295
LIB32   = ../../lib/$(OBJDIR)/libmem.a
LIB16   = ../../lib/$(OBJDIR)/libmem16.a

$(OBJDIR)/%.o: %.c $(OBJDIR); $(CC) -c $< -o $@ $(CFLAGS)

all: $(LIB32) $(LIB16) 

$(LIB32): $(OBJDIR)/mem.o; $(AR) crus $@ $^
$(LIB16): $(OBJDIR)/mem16.o; $(AR) crus $@ $^
$(OBJDIR)/mem16.o: mem.c $(OBJDIR); $(CC) -c $< -o $@ -mshort $(CFLAGS)

$(OBJDIR):; mkdir $@

clean:; $(RM) -r $(OBJDIR)


