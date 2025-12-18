# Build various support routines for OSLib users.

SUBDIRS = Choices_ Err_ Events_ Message_ Task_ trace_ X_

FILES = $(addsuffix /Modules, $(SUBDIRS))

HEADERS = Choices.h Err.h Event.h Gadgettypes.h Message.h Systypes.h \
	Task.h Toolboxtypes.h trace.h Wimptypes.h X.h

include $(FILES)

.PHONY: all clean install

all: $(OSLIBSUPPORTLIB)

clean:
	-$(RM) $(OSLIBSUPPORTLIB)
	-$(RM) $(OBJS)

install: $(OSLIBSUPPORTLIB)
	-$(MKDIR) $(GCCSDK_INSTALL_ENV)/lib
	cd $(dir $(OSLIBSUPPORTLIB)) && $(INSTALL) $(notdir $(OSLIBSUPPORTLIB)) $(GCCSDK_INSTALL_ENV)/lib/$(notdir $(OSLIBSUPPORTLIB))
	-$(MKDIR) $(GCCSDK_INSTALL_ENV)/include
	cd $(OSLIBSUPPORTDIR) && $(INSTALL) $(HEADERS) $(GCCSDK_INSTALL_ENV)/include

$(OSLIBSUPPORTLIB): $(OBJS)
	-$(MKDIR) $(OSLIBSUPPORTDIR)/Doc
	$(LIBFILE) $@ $(OBJS)
	$(CP) $(HEADERS) $(OSLIBSUPPORTDIR)
	$(CP) SetVars,feb $(OSLIBSUPPORTDIR)
	$(CP) Doc/X $(OSLIBSUPPORTDIR)/Doc/X

.SUFFIXES:
.SUFFIXES: .o .h .Hdr

%.o: %.c $(HEADERS)
	$(CC) -c $< -I. -I$(TOPHEADERDIR) -o $@
