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
# $Source: /cvsroot/windom/windom/src/gcc281.mak,v $
# Makefile for 'WinDOM' an its utility
# 
# CVS Info :
#  $Author: bercegeay $
#  $Date: 2006/01/03 13:52:34 $
#  $Revision: 1.11 $

#
# makefile for WinDom 
# gcc == 2.8.1
#

# note:
# CC_GCC281 must point to your gcc.ttp program
# AR_GCC281 must point to your  ar.ttp program

CC     = $(CC_GCC281)
AR     = $(AR_GCC281)

WARN = \
	-Wall \
	-Wmissing-prototypes \
	-Winline \
	-Wshadow \
	-Wpointer-arith \
	-Wcast-qual \
	-Waggregate-return

CFLAGS= -O2 -fomit-frame-pointer $(WARN) -I../include $(XCFLAGS)

# no extra objects for GCC
EXTRAOBJ  = 

include files.mak

OBJDIR=.gcc281
OBJECTS=$(addprefix $(OBJDIR)/,$(WINDOMOBJ))

.SUFFIXES:
.SUFFIXES: .c .S .o

#.c.o:
#	$(CC) $(CFLAGS) -c $*.c -o $*.o
$(OBJDIR)/%.o: %.c
	@echo GCC281-CC $(notdir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

TARGET = $(LIB_GCC281)

all: $(TARGET)

$(TARGET): import_xobjc $(OBJDIR) $(OBJECTS)
	@echo LD $@
	@rm -f $@
	@mkdir -p `dirname $(LIB_GCC281)`
	@$(AR) cs $@ $(OBJECTS)

$(OBJDIR):; mkdir $(OBJDIR)

include import_xobjc.mak

