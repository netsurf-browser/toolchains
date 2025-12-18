#!make -f
#
# WinDom: a high level GEM library
# Copyright (c) 1997-2006 windom authors (see AUTHORS file)
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
# $Source: /cvsroot/windom/windom/examples/bubble/Makefile,v $
# 
# CVS Info :
#  $Author: bercegeay $
#  $Date: 2006/01/03 14:39:24 $
#  $Revision: 1.2 $

# gcc version

ifeq ($(CROSS),yes)
CROSSPREFIX = m68k-atari-mint
CC     = $(CROSSPREFIX)-gcc
PREFIX = /usr/$(CROSSPREFIX)
else
CC = gcc
PREFIX = /usr
endif

CFLAGS = 
LDFLAGS = $(M68K_ATARI_MINT_LDFLAGS)

WARN = \
        -Wall \
        -Wmissing-prototypes \
        -Winline \
        -Wshadow \
        -Wpointer-arith \
        -Wcast-qual \
        -Waggregate-return

CFLAGS += -O2 -fomit-frame-pointer -I../../include $(WARN) $(M68K_ATARI_MINT_CFLAGS)

OBJ = bubbltst.o
PRG = bubble.app

OBJDIR = .gcc295
OBJECTS = $(addprefix $(OBJDIR)/,$(OBJ))

.SUFFIXES:
.SUFFIXES: .c .S .o

$(OBJDIR)/%.o: %.c $(OBJDIR)
	@echo CC $(notdir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(PRG) : $(OBJECTS)
	$(CC) -o$@ $(OBJECTS) $(LDFLAGS) ../../lib/gcc/libwindom.a -lgem -lldg

$(OBJDIR):; mkdir $(OBJDIR)

.PHONY: clean;

clean:; rm -rf $(OBJDIR)
