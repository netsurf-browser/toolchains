# OSLib's top Makefile to build ELF/AOF OSLib versions, install them in the
# GCCSDK cross-compiler environment and to create OSLib releases.
# Written by OSLib Development Team.

.PHONY: all install install-aof install-elf install-vapis \
	clean oslib-aof oslib-elf vapis cstrong \
	oslib-aof-n-release oslib-elf-n-release vapis-n-release \
	oslib-examples-n-release oslib-src-n-release cstrong-n-release \
	tools

# ELF object files can be hard-float or soft-float marked (but as the OSLib
# object code does not contain FPU instructions this is only setting a flag
# in the object files).
# Possible values: "SOFTFPU" (e.g. for UnixLib usage), "HARDFPU" (e.g. for
# SharedCLibrary usage) and "MODULE" (latter also meaning HARDFPU but the
# C code is compiled for module usage).
export ELFOBJECTTYPE ?= SOFTFPU

# Building oslib-elf and oslib-aof can not happen at the same time.
.NOTPARALLEL:

# The default all/install target is ELF.
all: oslib-elf cstrong

install: install-elf
	@echo OSLib ELF installed

# Installs the ELF headers & library in GCCSDK environment.
install-elf: oslib-elf
	$(MAKE) -C Source install BUILDTYPE=CROSSGCC_ELF

# Installs the AOF headers & library in GCCSDK environment.
install-aof: oslib-aof
	$(MAKE) -C Source install BUILDTYPE=CROSSGCC_AOF

# Installs the vapi files in GCCSDK environment.
install-vapis: vapis
	$(MAKE) -C Source install-vapis

clean:
	$(MAKE) -C Tools clean
	$(MAKE) -C Source clean

oslib-aof: tools
	$(MAKE) -C Source oslib BUILDTYPE=CROSSGCC_AOF
	$(MAKE) -C Source oslibsupport BUILDTYPE=CROSSGCC_AOF

oslib-elf: tools
	$(MAKE) -C Source oslib BUILDTYPE=CROSSGCC_ELF
	$(MAKE) -C Source oslibsupport BUILDTYPE=CROSSGCC_ELF

vapis: tools
	$(MAKE) -C Source vapis BUILDTYPE=CROSSGCC_ELF

cstrong: tools
	$(MAKE) -C Source cstrong

## vv Release targets (BEGIN)
oslib-aof-n-release: oslib-aof
	$(MAKE) -C Source release-oslib BUILDTYPE=CROSSGCC_AOF
	$(MAKE) -C Source release-oslibsupport BUILDTYPE=CROSSGCC_AOF

oslib-elf-n-release: oslib-elf
	$(MAKE) -C Source release-oslib BUILDTYPE=CROSSGCC_ELF
	$(MAKE) -C Source release-oslibsupport BUILDTYPE=CROSSGCC_ELF

vapis-n-release: vapis
	$(MAKE) -C Source release-vapis BUILDTYPE=CROSSGCC_ELF

oslib-examples-n-release:
	$(MAKE) -C Source release-examples

oslib-src-n-release:
	$(MAKE) -C Source release-src

cstrong-n-release: cstrong
	$(MAKE) -C Source release-cstrong
## ^^ Release targets (END)

# Build the tools and install them.
tools:
	$(MAKE) -C Tools install
