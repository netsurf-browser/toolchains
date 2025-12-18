# ...

# SWIMODULES and MODULES all have their .swi file.  The former have one or
# more SWI's defined, the latter no SWI's at all.
# HDRMODULES only have handwritten .h, .AOFHdr and .ELFHdr files.
# ASMMODULES only have handwritten .asm (AOF) and .gas (ELF) assembler files.

ASMHEADERS	= $(patsubst %,%.Hdr,$(SWIMODULES) $(MODULES) $(HDRMODULES))
CHEADERS	= $(patsubst %,%.h,$(SWIMODULES) $(MODULES) $(HDRMODULES))
VAPIS		= $(patsubst %,OSLib%.vapi,$(SWIMODULES) $(MODULES) $(VAPIMODULES))
VAPIDEPS        = $(patsubst %,OSLib%.deps,$(SWIMODULES) $(MODULES))
VAPIHS		= $(patsubst %,OSLib%.vapih,$(SWIMODULES) $(MODULES))
CSTRONG		= $(patsubst %,%.help,$(SWIMODULES) $(MODULES))
SWIOBJS		= $(patsubst %,%.o,$(SWIMODULES))
ASMOBJS		= $(patsubst %,%.o,$(ASMMODULES))

# Generated files from .swi files (only used for removing those files in clean
# rule)
GENASMHEADERS	= $(patsubst %,%.Hdr,$(SWIMODULES) $(MODULES) $(HDRMODULES))
GENCHEADERS	= $(patsubst %,%.h,$(SWIMODULES) $(MODULES))
GENVAPIS        = $(patsubst %,OSLib%.vapi,$(SWIMODULES) $(MODULES))
GENVAPIDEPS     = $(patsubst %,OSLib%.deps,$(SWIMODULES) $(MODULES))
GENVAPIHS       = $(patsubst %,OSLib%.vapih,$(SWIMODULES) $(MODULES))
GENCSTRONG	= $(patsubst %,%.help,$(SWIMODULES) $(MODULES))

.PHONY: oslib cstrong clean
# Building inside one subdirectory of Source can not happen in parallel
# because of ViaFile creation in defmod.
.NOTPARALLEL:

# Targets:
oslib:  $(ASMHEADERS) $(CHEADERS) $(SWIOBJS) $(ASMOBJS)
	$(CP) *.Hdr $(HEADERDIR)
	$(foreach hdr,$(shell ls *.h),$(eval $(shell $(CP) $(hdr) $(HEADERDIR)/$(call tolower,$(hdr)))))
	if [ ! -z "$(SWIMODULES)" ]; then $(CP) $(patsubst %,l/%,$(SWIMODULES)) $(OBJDIR); fi
	if [ ! -z "$(ASMOBJS)" ]; then $(CP) $(ASMOBJS) $(OBJEXCEPTDIR); fi

vapis:	$(VAPIS) $(VAPIDEPS) $(VAPIHS)
	if [ ! -z "$(VAPIS)" ]; then $(CP) *.vapi $(VAPIDIR); fi
	if [ ! -z "$(VAPIDEPS)" ]; then $(CP) *.deps $(VAPIDIR); fi
	if [ ! -z "$(VAPIHS)" ]; then $(CP) OSLib*.vapih $(VAPIHDIR); fi

cstrong: $(CSTRONG)

clean:
	-$(RM) l/*
	-$(RM) $(GENASMHEADERS) $(GENCHEADERS) $(GENCSTRONG) $(SWIOBJS) $(ASMOBJS) $(GENVAPIS) \
	$(GENVAPIHS) $(VAPIDEPS)

# Convert arg to lower case
tolower = $(shell echo $(1) | tr '[A-Z]' '[a-z]';)

#------------------------------------------------------------------------
# general rules:

.SUFFIXES:
.SUFFIXES: .o .h .swi .Hdr .AOFHdr .ELFHdr .help .asm .gas

# Source files: .h .swi .AOFHdr .ELFHdr .asm .gas
# Derived files: .o .h .help

# Assmebler header generation:
.swi.Hdr:
		$(DEFMOD) -hdr < $< > $@
ifeq ($(BUILDTYPE),CROSSGCC_ELF)
.ELFHdr.Hdr:
		if [ -s "$<" ]; then $(CP) $< $@; fi
else
.AOFHdr.Hdr:
		if [ -s "$<" ]; then $(CP) $< $@; fi
endif

# C header generation:
.swi.h:
		$(DEFMOD) -h < $< > $@

# vapi file generation:
OSLib%.vapi : %.swi
		$(DEFMOD) -vapi < $< > $@
OSLib%.vapih : %.swi
		$(DEFMOD) -vapih < $< > $@
OSLib%.deps : %.swi
		$(DEFMOD) -vapideps < $< > $@

# Object code generation (from .swi file):
.swi.o:
		$(DEFMOD) -l -o l/$* -byte_wide $(BYTEWIDE) -32bit < $<
		$(REDUCEAOF) -via ViaFile
		$(RM) ViaFile
		@touch $@

# StrongHelp generation:
.swi.help:
		$(DEFMOD) -cstrong -o $(CSTRONGDIR) < $<
		@touch $@

# Object code generation (from .asm or .gas file):
ifeq ($(BUILDTYPE),CROSSGCC_ELF)
.gas.o:
		$(AS) -o $@ $<
else
.asm.o:
		$(AS) -o $@ $<
endif
