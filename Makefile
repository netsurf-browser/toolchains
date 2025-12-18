# This makefile attempts to build a squeeze executable which can be used
# on Linux to compress RISC OS binaries.

GCCSDK_INSTALL_CROSSBIN ?= /opt/netsurf/arm-unknown-riscos/cross/bin

PREFIX ?= $(GCCSDK_INSTALL_CROSSBIN)/..

PERL ?= perl
export PERL

MAKEHEADER = $(PERL) makeheader.pl

CC := gcc
ASASM := $(GCCSDK_INSTALL_CROSSBIN)/asasm
OBJCOPY := $(wildcard $(GCCSDK_INSTALL_CROSSBIN)/*objcopy)

CFLAGS := $(CFLAGS) -O2

all: squeeze

install: squeeze
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m 755 squeeze $(DESTDIR)$(PREFIX)/bin/squeeze

clean:
	$(RM) squeeze squeeze.o unsqueeze.o unsqrm.o
	$(RM) unsqueeze1.h unsqrm1.h unsqueeze.bin unsqrm.bin

distclean: clean
	$(RM) *~

squeeze: squeeze.o
	$(CC) $(CFLAGS) -o $@ $^

squeeze.o: squeeze.c unsqueeze1.h unsqrm1.h
	$(CC) $(CFLAGS) -o $@ -c $<

unsqrm1.h: unsqrm.bin
	$(MAKEHEADER) unsqueeze_base unsqueeze_limit unsqueeze_end $< $@

unsqueeze1.h: unsqueeze.bin
	$(MAKEHEADER) UnSqueeze_UnSqueezeBase '' UnSqueeze_UnSqueezeLimit $< $@

unsqueeze.bin: unsqueeze.o
	$(OBJCOPY) -O binary -j 'M2$$$$Code' $< $@

unsqrm.bin: unsqrm.o
	$(OBJCOPY) -O binary -j unsqueeze $< $@

%.o:%.s
	$(ASASM) -o $@ -elf -32 $<
