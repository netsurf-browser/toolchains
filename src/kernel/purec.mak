#!make -f
#
# LDG : Gem Dynamical Libraries
# Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
#
# Main makefile for to build kernel (LDG TSR)
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
# $Id: Makefile 9 2005-07-25 19:37:25Z bercegeay $

CC      = $(CC_PUREC)
AS      = $(AS_PUREC)
LD      = $(LD_PUREC)
CFLAGS  = -I. -I.. -I../../include -I$(PURECINC) -P -G 
#-DNOSTARTUP
ASFLAGS = 
STARTUP = $(PURECLIB)/pcstart.o
STDLIB  = $(PURECLIB)/pcstdlib.lib $(PURECLIB)/pctoslib.lib
GEMLIB  = $(PURECLIB)/gem.lib
LDFLAGS = -V
LDLIBS  = $(STDLIB) ../../lib/purec/ldg.lib $(GEMLIB) 

OBJECTS = tsrmain.o tsrinfo.o ldglist.o ldgcook.o cookie.o
TARGET  = ../../auto/purec/ldg.prg

all: $(TARGET)

%.o:%.c
	@echo CC $@
	@$(CC) $(CFLAGS) -O=$@ $< 

%.o:%.s
	@echo AS $@
	@$(AS) $(ASFLAGS) -O=$@ $<

$(TARGET): $(OBJECTS)
	@echo LD $@
	$(LD) $(LDFLAGS) -O=$@ $(STARTUP) $^ $(LDLIBS)
	@chmod +x $@

cookie.o: cookie.s
clean:; $(RM) $(OBJECTS)
