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
# $Source: /cvsroot/windom/windom/demo/sox.mak,v $
# 
# CVS Info :
#  $Author: bercegeay $
#  $Date: 2006/01/03 14:15:46 $
#  $Revision: 1.5 $

# sozobonx version

# note:
# CC_SOX must point to your cc.ttp program

CC     = $(CC_SOX)
CPP    = $(dir $(CC_SOX))cpp.ttp

CPPFLAGS = -I../include -I$(dir $(CC_SOX))../include -I$(dir $(CC_SOX))../include/xdlibs -P -fc++-comments
CFLAGS = -O -I../include
LDFLAGS=  +L-B2 -L../lib/sozobon -lwindom -lgem -lldg -lc

include files.mak

OBJDIR=.sox
OBJECTS=$(addprefix $(OBJDIR)/,$(OBJ))

#.c.o:
$(OBJDIR)/%.o: %.c $(OBJDIR)
	@echo SOX-CC $(notdir $@)
	@$(CPP) $(CPPFLAGS) $< $(basename $<).i
	@$(CC) $(CFLAGS) -c $(basename $<).i -o $@
	@$(RM) -f $(basename $<).i

demo-sox.app : $(OBJECTS)
	$(CC) -o$@ -scrt0.o -O +L-p $(OBJECTS) $(LDFLAGS)

$(OBJDIR):; mkdir $(OBJDIR)
