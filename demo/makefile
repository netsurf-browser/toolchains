#!make -f
# @(#)makefile, 
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
# $Source: /cvsroot/windom/windom/demo/makefile,v $
# 
# CVS Info :
#  $Author: bercegeay $
#  $Date: 2006/01/03 14:15:46 $
#  $Revision: 1.3 $

# make file for 'WinDOM' demo program
#
all:
	@echo makefile for the demo
	@echo targets are clean, pc, sox, gcc, cross or gcc281

.PHONY : clean sox gcc281 gcc

clean:
	rm -rf .gcc295 .gcc281 .sox .purec *~

sox:
	make -f sox.mak

gcc281:
	make -f gcc281.mak

gcc:
	make -f gcc.mak

cross:
	make -f gcc.mak CROSS=yes

pc:
	make -f pc.mak

#eof
