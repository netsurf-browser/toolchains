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
# $Source: /cvsroot/windom/windom/src/Makefile,v $
# Makefile for 'WinDOM' an its utility
# 
# CVS Info :
#  $Author: bercegeay $
#  $Date: 2006/01/03 13:52:34 $
#  $Revision: 1.9 $

include files.mak

all:
	@echo targets are pc, sox, gcc, gcc281, cross, doc or clean

sozobon sox:
	$(MAKE) -f sox.mak

purec pc:
	$(MAKE) -f pc.mak

gcc:
	$(MAKE) -f gcc.mak

gcc281:
	$(MAKE) -f gcc281.mak

cross:
	 $(MAKE) -f gcc.mak CROSS=yes

clean:
	$(RM) -rf .gcc295 .gcc281 .sox .purec *~

veryclean:
	$(RM) -rf ../html gcc281 gcc295 sox purec

.PHONY : doc

doc: ../doc import_xobjc ../ChangeLog; doxygen windom.doxy
../doc:; mkdir -p $@

include import_xobjc.mak

../ChangeLog:
	cd .. && cvs log -d "20031001<" . >ChangeLog.tmp && \
	perl cvs2cl.pl --separate-header --no-wrap --usermap users --stdin --stdout <ChangeLog.tmp >ChangeLog && \
	rm -f ChangeLog.tmp
