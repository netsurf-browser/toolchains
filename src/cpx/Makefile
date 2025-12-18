#!make -f
#
# LDG : Gem Dynamical Libraries
# Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
#
# Makefile for compiling the LDG CPX
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# $Id: Makefile 27 2005-09-10 21:21:38Z bereziat $

# Pure C directory
#PURECDIR = /h/gemapps/devel/pure_c

CC      = $(CC_PUREC)
LD      = $(LD_PUREC)

CFLAGS  = -I. -I../../include -I../share -I$(PURECINC) \
	  -DNOSTARTUP -P -M -DUSE_LDGLOAD
STDLIB  = $(PURECLIB)/pcstdlib.lib $(PURECLIB)/pctoslib.lib
GEMLIB  = $(PURECLIB)/gem.lib
LDLIBS  = $(STDLIB) ../../lib/purec/ldg.lib $(GEMLIB) 
LDFLAGS =

OBJECTS = main.o evnt.o funcs.o bubble.o utils.o internal.o
TARGET  = ../../cpx/ldg.cpx

%.o: %.c
	@echo "CC $<"
	@$(CC) $(CFLAGS) -O=$@ $<

all: cpxhdr $(TARGET)

$(TARGET): ldg.hdr ldg.cp; cat $^ > $@
ldg.hdr: ldg.rsc; ./cpxhdr $< $@
ldg.cp: $(OBJECTS)
	@echo "LD $@"
	@$(LD) $(LDFLAGS) -O=$@ $^ $(LDLIBS) > /dev/null

# Dependencies
main.o: resource.h
main.o funcs.o evnt.o utils.o bubble.o : global.h

# Compilation of cpxhdr (a tool to build CPX header)
cpxhdr: cpxhdr.o
	$(LD)  $(LDFLAGS) -S=4096 -O=$@ $(PURECDIR)/lib/pcstart.o $^ $(STDLIB) $(GEMLIB)
	chmod +x $@
cpxhdr.o: cpxhdr.c
	$(CC) -G+ -P+ -I$(PURECDIR)/include -O=$@ $<

# Generate a clean RSH resource from Interface output
resource.h: ldgcpx.rsh ldgcpx.rh
	sed -f rshconv.sed ldgcpx.rsh > resource.h && grep 'define' ldgcpx.rh >> resource.h

clean:; $(RM) $(OBJECTS) ldg.cp ldg.hdr resource.h
veryclean: clean; $(RM) cpxhdr cpxhdr.o

install:; cp ../../cpx/cpx.ldg /h/gemsys/cpx/
