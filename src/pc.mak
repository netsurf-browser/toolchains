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
# $Source: /cvsroot/windom/windom/src/pc.mak,v $
# Makefile for 'WinDOM' an its utility
# 
# CVS Info :
#  $Author: bercegeay $
#  $Date: 2006/01/03 13:52:34 $
#  $Revision: 1.13 $

# makefile for windom 
# pure c version


# note:
# CC_PUREC must point to your pcc.ttp program
# AR_PUREC must point to your plink.ttp program
# AS_PUREC must point to your pasm.ttp program
# PURECINC must point to your pure_c/include directory

CC     = $(CC_PUREC)
AS     = $(AS_PUREC)
AR     = $(AR_PUREC) -J

CFLAGS = -I$(PURECINC) -I../include $(XCFLAGS) -P

# extra objects
EXTRAOBJ  =

include files.mak

OBJDIR = .purec
OBJECTS = $(addprefix $(OBJDIR)/,$(WINDOMOBJ))

.SUFFIXES:
.SUFFIXES: .c .s .o

$(OBJDIR)/%.o: %.c
	@echo PUREC-CC $(notdir $@)
	@cp $*.c tmppc.c
	@$(CC) $(CFLAGS) -Otmppc.o tmppc.c
	@mv -f tmppc.o $(OBJDIR)/$*.o
	@rm -f tmppc.c

# PASM.TTP generates filename in UPPERCASE, so...
$(OBJDIR)/%.o: %.s $(OBJDIR)
	@echo PUREC-AS $(notdir $@)
	@$(AS) $*.s -O$(OBJDIR)/TMPPC.O
	@mv $(OBJDIR)/TMPPC.O $(OBJDIR)/$*.o

TARGET = $(LIB_PUREC)

all: $(TARGET)

$(TARGET): import_xobjc $(OBJDIR) $(OBJECTS)
	@rm -f $@
	@mkdir -p `dirname $(LIB_PUREC)`
	@echo LD $@
	$(AR) $(OBJECTS)
	@mv -f Linker.Out $@


$(OBJDIR):; mkdir $(OBJDIR)

include import_xobjc.mak

