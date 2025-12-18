#!make -f
#
# LDG : Gem Dynamical Libraries
# Copyright (c) 1997-2011 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
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

ifeq ($(CROSS),yes)
CC      = m68k-atari-mint-gcc
AS      = m68k-atari-mint-gcc
LD      = m68k-atari-mint-gcc
STRIP      = m68k-atari-mint-strip
else
CC     = gcc
AS     = gcc
LD     = gcc
STRIP     = strip
endif

CFLAGS  = -mcpu=5475 -W -Wall -O -I. -I.. -I../../include
#-DNOSTARTUP
ASFLAGS = -mcpu=5475
STARTUP =
GEMLIB  = gem
LDFLAGS = -v -mcpu=5475 $(M68K_ATARI_MINT_LDFLAGS)
LDLIBS  = -l$(GEMLIB) ../../lib/gcc/m5475/libldg.a
OBJDIR   = .gcc

OBJS = tsrinfo.o ldglist.o ldgcook.o cookiec.o tsrmain.o
OBJECTS = $(addprefix $(OBJDIR)/,$(OBJS))
TARGET  = ../../auto/gcc/coldfire/ldg.prg
TARGETD = $(dir $(TARGET))

all: $(TARGET)

$(OBJDIR)/%.o: %.c $(OBJDIR)
	@echo CC $@
	@$(CC) -c $(CFLAGS) $< -o $@

$(TARGET): $(OBJECTS) $(TARGETD)
	@echo LD $@
	$(LD) $(LDFLAGS) -o$@ $(OBJECTS) $(STARTUP) $(LDLIBS)
	@chmod +x $@
	$(STRIP) $(TARGET)

# dependencies
$(OBJDIR):; mkdir $(OBJDIR)
$(TARGETD):; mkdir -p $@

clean:; $(RM) -r $(OBJDIR)
