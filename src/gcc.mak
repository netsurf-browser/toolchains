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
# $Source: /cvsroot/windom/windom/src/gcc.mak,v $
# Makefile for 'WinDOM' an its utility
# 
# CVS Info :
#  $Author: bercegeay $
#  $Date: 2006/06/14 21:09:00 $
#  $Revision: 1.12 $

#
# makefile for windom 
# gcc >= 2.95.3 (sparemint) version
#

ifeq ($(CROSS),yes)
CROSSPREFIX = m68k-atari-mint
CC     = $(CROSSPREFIX)-gcc
AR     = $(CROSSPREFIX)-ar
PREFIX = /usr/$(CROSSPREFIX)
else
CC     = gcc
AR     = ar
PREFIX = /usr
endif

WARN = \
	-Wall \
	-Wmissing-prototypes \
	-Winline \
	-Wshadow \
	-Wpointer-arith \
	-Wcast-qual \
	-Waggregate-return

CFLAGS= -O2 -fomit-frame-pointer $(WARN) -I../include $(M68K_ATARI_MINT_CFLAGS)
#CFLAGS= -O0 -ggdb $(WARN) -I../include $(M68K_ATARI_MINT_CFLAGS)

# no extra objects for GCC
EXTRAOBJ  = 

include files.mak

OBJDIR = .gcc295
OBJECTS = $(addprefix $(OBJDIR)/,$(WINDOMOBJ))

.SUFFIXES:
.SUFFIXES: .c .S .o

$(OBJDIR)/%.o: %.c
#	if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi
#	@echo CC $(notdir $@)
	$(CC) $(CFLAGS) -c $< -o $@

TARGET = $(LIB_GCC)

$(TARGET): import_xobjc $(OBJDIR) $(OBJECTS)
	@echo LD $@
	@rm -f $@
	@mkdir -p `dirname $(LIB_GCC)`
	@$(AR) cru $@ $(OBJECTS)

$(OBJDIR):; mkdir $(OBJDIR)

install:
	cp -vf ../include/windom.h ../include/mt_wndm.h $(PREFIX)/include
	mkdir -p $(PREFIX)/include/windom
	cp -vf ../include/windom/*.h $(PREFIX)/include/windom
	cp -vf ../lib/gcc/libwindom.a $(PREFIX)/lib

include import_xobjc.mak

