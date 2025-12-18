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
# $Source: /cvsroot/windom/windom/src/import_xobjc.mak,v $
# list of files used in various makefile
# 
# CVS Info :
#  $Author: bercegeay $
#  $Date: 2006/01/03 13:52:34 $
#  $Revision: 1.3 $

UDEFLIBPATH = ../userdef/extended

.PHONY: import_xobjc;

import_xobjc: wsystem.h wsystem.rh wsystem.rsh xobjc_global.h xobjc_interface.h xobjc_char.c\
              xobjc_draw.c xobjc_extend.c xobjc_interface.c xobjc_string.c xobjc_xedit.c

wsystem.h: $(UDEFLIBPATH)/wsystem.h
	cp -f $(UDEFLIBPATH)/wsystem.h .

wsystem.rh: $(UDEFLIBPATH)/wsystem.rh
	cp -f $(UDEFLIBPATH)/wsystem.rh .

wsystem.rsh: $(UDEFLIBPATH)/wsystem.rsh
	cp -f $(UDEFLIBPATH)/wsystem.rsh .

xobjc_global.h: $(UDEFLIBPATH)/global.h
	echo "#define BUILTIN_XOBJC 1" >$@
	sed -e "s/interface.h/xobjc_interface.h/g" $< >>$@

xobjc_interface.h: $(UDEFLIBPATH)/interface.h
	cp -f $< $@

xobjc_char.c: $(UDEFLIBPATH)/char.c
	sed -e "s/global.h/xobjc_global.h/g" $< >$@

xobjc_draw.c: $(UDEFLIBPATH)/draw.c
	sed -e "s/global.h/xobjc_global.h/g" $< >$@

xobjc_extend.c: $(UDEFLIBPATH)/extend.c
	sed -e "s/global.h/xobjc_global.h/g" $< >$@

xobjc_interface.c: $(UDEFLIBPATH)/interface.c
	sed -e "s/global.h/xobjc_global.h/g" $< >$@

xobjc_string.c: $(UDEFLIBPATH)/string.c
	sed -e "s/global.h/xobjc_global.h/g" $< >$@

xobjc_xedit.c: $(UDEFLIBPATH)/xedit.c
	sed -e "s/global.h/xobjc_global.h/g" $< >$@
