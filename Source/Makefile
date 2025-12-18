# OSLib Source Makefile building the OSLib library and its headers.
# The necessary OSLib build progreams (like defmod, bindhelp, etc) should
# already be built upfront.
# Written by OSLib Development Team.

# Build type: CROSSGCC_AOF, CROSSGCC_ELF (default), [NORCROFT_AOF: not
# supported yet]
export BUILDTYPE ?= CROSSGCC_ELF
# Possible values: "SOFTFPU" (e.g. for UnixLib usage), "HARDFPU" (e.g. for
# SharedCLibrary usage) and "MODULE" (latter also meaning HARDFPU but the
# C code is compiled for module usage).
export ELFOBJECTTYPE ?= SOFTFPU

ifeq ($(BUILDTYPE),CROSSGCC_ELF)
else ifeq ($(BUILDTYPE),CROSSGCC_AOF)
else
$(error Unknown BUILDTYPE value, possible values are CROSSGCC_AOF and CROSSGCC_ELF)
endif
include Makefile.setup.$(BUILDTYPE)

OSLIBVERSION := 700

# Files in the docs directory which need to be included in the release zip files:
DOCFILES := ChangeLog COPYING OSLib_API OSLib_readme WideFuncts

.PHONY: all install \
	release-oslib release-oslibsupport release-cstrong release-examples release-src \
	clean oslib oslibsupport cstrong BuildRequirements vapis release-vapis
all:	oslib oslibsupport

test:	oslib
	$(MAKE) -C Test tests-novapis

test-vapis: vapis
	$(MAKE) -C Test tests-vapis

# Only for GCCSDK Autobuilder cross-compilation install purposes:
install: oslib oslibsupport
	-$(MKDIR) $(GCCSDK_INSTALL_ENV)/lib
	cd $(dir $(OSLIBLIB)) && $(INSTALL) $(notdir $(OSLIBLIB)) $(GCCSDK_INSTALL_ENV)/lib/$(notdir $(OSLIBLIB))
	-$(MKDIR) $(GCCSDK_INSTALL_ENV)/include/oslib
	cd $(HEADERDIR) && $(INSTALL) *.h *.Hdr $(GCCSDK_INSTALL_ENV)/include/oslib
	$(MAKE) -C OSLibSupport install

# Only for GCCSDK Autobuilder cross-compilation install purposes:
install-vapis: vapis
	-$(MKDIR) $(GCCSDK_INSTALL_ENV)/share/vala/vapi
	cd $(VAPIDIR) && $(INSTALL) *.vapi *.deps $(GCCSDK_INSTALL_ENV)/share/vala/vapi

# Sanity test and if successful then zips the OSLib library & its headers (Unix and RISC OS distribution):
release-oslib: test
	-$(MKDIR) $(RELEASEBINDIR)
	# Unix zip:
	#-$(RM) $(RELEASEBINDIR)/OSLib$(OSLIBVERSION)-unix.zip
	#cd $(TOPHEADERDIR) && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEBINDIR)/OSLib$(OSLIBVERSION)-unix.zip oslib
	#cd $(BUILDDIR) && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEBINDIR)/OSLib$(OSLIBVERSION)-unix.zip $(notdir $(OSLIBLIB))
	#cd $(DOCSDIR) && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEBINDIR)/OSLib$(OSLIBVERSION)-unix.zip $(DOCFILES)
	#$(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEBINDIR)/OSLib$(OSLIBVERSION)-unix.zip SetVars,feb
	# RISC OS zip:
	-$(RM) $(RELEASEBINDIR)/OSLib$(OSLIBVERSION).zip
	-$(MKDIR) $(HEADERDIR)/h $(HEADERDIR)/Hdr
	-test "$(LIBSUFFIX)" = "o" && $(MKDIR) $(TOPHEADERDIR)/$(LIBSUFFIX)
	-test "$(LIBSUFFIX)" = "o" && $(CP) $(OSLIBLIB) $(TOPHEADERDIR)/$(LIBSUFFIX)/$(basename $(notdir $(OSLIBLIB)))
	cd $(HEADERDIR) && find . -type f \( -name "*.h" -o -name "*.Hdr" -o -name "*.o" \) -exec rename 's/(.+)\.(.+)/$$2\/$$1/' {} \;
	cd $(TOPHEADERDIR) && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEBINDIR)/OSLib$(OSLIBVERSION).zip .
	-test "$(LIBSUFFIX)" = "a" && cd $(BUILDDIR) && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEBINDIR)/OSLib$(OSLIBVERSION).zip $(notdir $(OSLIBLIB))
	cd $(DOCSDIR) && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEBINDIR)/OSLib$(OSLIBVERSION).zip $(DOCFILES)
	$(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEBINDIR)/OSLib$(OSLIBVERSION).zip SetVars,feb

# Zips the OSLibSupport library & its headers (Unix and RISC OS distribution):
release-oslibsupport: oslibsupport
	-$(MKDIR) $(RELEASEBINDIR)
	# Unix zip:
	#-$(RM) $(RELEASEBINDIR)/OSLibSupport$(OSLIBVERSION)-unix.zip
	#cd $(OSLIBSUPPORTDIR) && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEBINDIR)/OSLibSupport$(OSLIBVERSION)-unix.zip .
	#cd $(DOCSDIR) && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEBINDIR)/OSLibSupport$(OSLIBVERSION)-unix.zip $(DOCFILES)
	# RISC OS zip:
	-$(RM) $(RELEASEBINDIR)/OSLibSupport$(OSLIBVERSION).zip
	-$(MKDIR) $(OSLIBSUPPORTDIR)/h
	-test "$(LIBSUFFIX)" = "o" && $(MKDIR) $(OSLIBSUPPORTDIR)/$(LIBSUFFIX)
	cd $(OSLIBSUPPORTDIR) && find . -type f \( -name "*.h" -o -name "*.Hdr" -o -name "*.o" \) -exec rename 's/(.+)\.(.+)/$$2\/$$1/' {} \;
	cd $(OSLIBSUPPORTDIR) && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEBINDIR)/OSLibSupport$(OSLIBVERSION).zip .
	cd $(DOCSDIR) && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEBINDIR)/OSLibSupport$(OSLIBVERSION).zip $(DOCFILES)

# Zips the StrongHelp file:
release-cstrong: cstrong
	-$(MKDIR) $(RELEASEDIR)
	-$(RM) $(RELEASEDIR)/OSLibStrongHelp$(OSLIBVERSION).zip
	cd $(BUILDDIR) && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEDIR)/OSLibStrongHelp$(OSLIBVERSION).zip $(notdir $(OSLIBCSTRONG))
	cd $(DOCSDIR) && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEDIR)/OSLibStrongHelp$(OSLIBVERSION).zip $(DOCFILES)

# Zips the Vala bindings:
release-vapis: test-vapis
	-$(MKDIR) $(RELEASEDIR)
	# Unix zip:
	#-$(RM) $(RELEASEDIR)/OSLibVapi$(OSLIBVERSION)-unix.zip
	#cd $(BUILDDIR)/\!Vala && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEDIR)/OSLibVapi$(OSLIBVERSION)-unix.zip vapi oslib
	#cd $(DOCSDIR) && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEDIR)/OSLibVapi$(OSLIBVERSION)-unix.zip $(DOCFILES)
	# RISC OS zip:
	-$(RM) $(RELEASEDIR)/OSLibVapi$(OSLIBVERSION).zip
	-$(RM) $(VAPIDIR)/*.vapi,fff
	rename 's/\.vapi$$/.vapi,fff/' $(VAPIDIR)/*.vapi
	-$(RM) vapi/*.deps,fff
	rename 's/\.deps$$/.deps,fff/' $(VAPIDIR)/*.deps
	-$(RM) $(BUILDDIR)/\!Vala/oslib/h/valavapi,fff
	-$(MKDIR) $(BUILDDIR)/\!Vala/oslib/h
	mv $(BUILDDIR)/\!Vala/oslib/valavapi.h $(BUILDDIR)/\!Vala/oslib/h/valavapi,fff
	cd $(BUILDDIR) && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEDIR)/OSLibVapi$(OSLIBVERSION).zip $(notdir $(BUILDDIR)/\!Vala)
	cd $(DOCSDIR) && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEDIR)/OSLibVapi$(OSLIBVERSION).zip $(DOCFILES)

# Zips the OSLib examples:
release-examples: BuildRequirements
	-$(MKDIR) $(RELEASEDIR)
	-$(RM) $(BUILDDIR)/OSLibExamples $(RELEASEDIR)/OSLibExamples$(OSLIBVERSION).zip
	svn export $(EXAMPLESDIR) $(BUILDDIR)/OSLibExamples && cd $(BUILDDIR) && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEDIR)/OSLibExamples$(OSLIBVERSION).zip OSLibExamples
	cd $(DOCSDIR) && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEDIR)/OSLibExamples$(OSLIBVERSION).zip $(DOCFILES)

# Zips the !OSLib sources:
release-src: BuildRequirements
	-$(MKDIR) $(RELEASEDIR)
	-$(RM) $(BUILDDIR)/\!OSLib $(RELEASEDIR)/OSLib$(OSLIBVERSION)-src.zip
	svn export $(SOURCEDIR)/../ $(BUILDDIR)/\!OSLib && cd $(BUILDDIR) && $(GCCSDK_INSTALL_CROSSBIN)/zip -, -9r $(RELEASEDIR)/OSLib$(OSLIBVERSION)-src.zip \!OSLib

clean:
	$(MAKE) -C Types/oslib $@
	$(MAKE) -C Computer/oslib $@
	$(MAKE) -C Core/oslib $@
	$(MAKE) -C Macros/oslib $@
	$(MAKE) -C Toolbox/oslib $@
	$(MAKE) -C User/oslib $@
	-$(RM) $(BUILDDIR)
	$(MAKE) -C OSLibSupport $@
	$(MAKE) -C Test $@

oslib: $(OSLIBLIB)
$(OSLIBLIB): BuildRequirements
	$(MAKE) -C Types/oslib oslib
	$(MAKE) -C Computer/oslib oslib
	$(MAKE) -C Core/oslib oslib
	$(MAKE) -C Macros/oslib oslib
	$(MAKE) -C Toolbox/oslib oslib
	$(MAKE) -C User/oslib oslib
	cd $(OBJDIR) && $(LIBFILE) $@ */*.o

oslibsupport: $(OSLIBLIB)
	$(MAKE) -C OSLibSupport

cstrong: $(OSLIBCSTRONG)
$(OSLIBCSTRONG): BuildRequirements
	$(MAKE) -C Types/oslib cstrong
	$(MAKE) -C Computer/oslib cstrong
	$(MAKE) -C Core/oslib cstrong
	$(MAKE) -C Macros/oslib cstrong
	$(MAKE) -C Toolbox/oslib cstrong
	$(MAKE) -C User/oslib cstrong
	cd $(BUILDDIR) && $(BINDHELP) $(CSTRONGDIR) $@

vapis: BuildRequirements
	if [ ! -d $(VAPIDIR) ]; then $(MKDIR) -p $(VAPIDIR); fi
	if [ ! -d $(VAPIHDIR) ]; then $(MKDIR) $(VAPIHDIR); fi
	$(MAKE) -C Types/oslib vapis
	$(MAKE) -C Computer/oslib vapis
	$(MAKE) -C Core/oslib vapis
	$(MAKE) -C Toolbox/oslib vapis
	$(MAKE) -C User/oslib vapis
	$(MKDIR) -p $(BUILDDIR)/\!Vala/oslib
	cat $(SOURCEDIR)/Types/oslib/start.vapih $(VAPIHDIR)/*.vapih \
	$(SOURCEDIR)/Types/oslib/end.vapih > $(BUILDDIR)/\!Vala/oslib/valavapi.h

BuildRequirements:
	if [ ! -d $(BINDIR) ]; then $(MKDIR) $(BINDIR); fi
	if [ ! -d $(CSTRONGDIR) ]; then $(MKDIR) $(CSTRONGDIR); fi
	if [ ! -d $(HEADERDIR) ]; then $(MKDIR) $(HEADERDIR); fi
	if [ ! -d $(OBJDIR) ]; then $(MKDIR) $(OBJDIR); fi
	if [ ! -d $(OBJEXCEPTDIR) ]; then $(MKDIR) $(OBJEXCEPTDIR); fi
