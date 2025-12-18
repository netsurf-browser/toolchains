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
# $Source: /cvsroot/windom/windom/demo/pc.mak,v $
# 
# CVS Info :
#  $Author: bercegeay $
#  $Date: 2006/01/03 14:15:46 $
#  $Revision: 1.5 $

# pure c version

# note:
# CC_PUREC must point to your pcc.ttp program
# PURECINC must point to your pure_c/include directory

CC     = $(CC_PUREC)
LNK    = $(AR_PUREC)

CFLAGS = -I../include -I$(PURECINC) $(XCFLAGS) -P


include files.mak

OBJDIR=.purec
OBJECTS=$(addprefix $(OBJDIR)/,$(OBJ))

.SUFFIXES:
.SUFFIXES: .c .s .o

$(OBJDIR)/%.o: %.c $(OBJDIR)
	@echo PUREC-CC $(notdir $@)
	@cp $*.c tmppc.c
	@$(CC) $(CFLAGS) -Otmppc.o tmppc.c
	@mv -f tmppc.o $(OBJDIR)/$*.o
	@rm -f tmppc.c

# a revoir...
demo-pc.app : $(OBJECTS)
	$(LNK) -V -S=32000 -O=$@ $(PURECLIB)/pcstart.o $(OBJECTS) \
	       ../lib/purec/windom.lib $(PURECLIB)/gem.lib $(PURECLIB)/pcstdlib.lib \
		   $(PURECLIB)/ldg.lib $(PURECLIB)/pctoslib.lib

$(OBJDIR):; mkdir $(OBJDIR)
