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
# $Source: /cvsroot/windom/windom/examples/tutorial/Makefile,v $
# 
# CVS Info :
#  $Author: bercegeay $
#  $Date: 2006/01/03 14:39:25 $
#  $Revision: 1.4 $

# Makefile for WinDom tutorial

ifeq ($(CROSS),yes)
CC = m68k-atari-mint-gcc
else
CC = gcc
endif

CFLAGS  = -O2 -I../../include
LDFLAGS = -L../../lib/gcc
LDLIBS  = -lwindom -lgem -lldg
MODULES = tut-1-1.app tut-1-2.app tut-1-3.app \
          tut-2-1.app tut-2-2.app tut-2-3.app

%.app: %.o; $(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

all: $(MODULES)

tut-2-1.app tut-2-2.app tut-2-3.app: text.o
tut-2-1.o: text.h
tut-2-2.o tut-2-3.o: tut_2.h text.h

clean: ; $(RM) $(MODULES:.app=.o) text.o 
veryclean: ; $(RM) $(MODULES)
