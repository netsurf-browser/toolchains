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
# $Source: /cvsroot/windom/windom/src/sox.mak,v $
# Makefile for 'WinDOM' an its utility
# 
# CVS Info :
#  $Author: bercegeay $
#  $Date: 2006/01/03 13:52:35 $
#  $Revision: 1.10 $

#
# makefile for windom 
# sozobonx version


# note:
# CC_SOX must point to your cc.ttp program
# AR_SOX must point to your ar.ttp program

CC     = $(CC_SOX)
AS     = $(AS_SOX)
AR     = $(AR_SOX)

CFLAGS= -O -I../include $(XCFLAGS)

# add bug-free strdup() (the one in xdlibs is bugged)
EXTRAOBJ  = strdup.o

include files.mak

OBJDIR=.sox
OBJECTS=$(addprefix $(OBJDIR)/,$(WINDOMOBJ))

.SUFFIXES:
.SUFFIXES: .c .S .o

#.c.o:
$(OBJDIR)/%.o: %.c
	@echo SOX-CC $(notdir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

#.s.o:
$(OBJDIR)/%.o: %.s $(OBJDIR)
	@echo SOX-AS $(notdir $@)
	@$(AS) -o $@ $<

TARGET = $(LIB_SOZOBON)

$(TARGET): import_xobjc $(OBJDIR) $(OBJECTS)
	@echo LD $@
	@rm -f $@
	@mkdir -p `dirname $(LIB_SOZOBON)`
	@$(AR) -r $@ $(OBJECTS)

$(OBJDIR):; mkdir $(OBJDIR)

include import_xobjc.mak

