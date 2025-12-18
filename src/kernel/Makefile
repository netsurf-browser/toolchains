#!make -f
#
# LDG : Gem Dynamical Libraries
# Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
#
# Main makefile for to build kernel (LDG TSR)
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
# $Id: Makefile 91 2012-05-12 18:30:32Z landemarre $

all:; @echo "Possible targets are: gcc, gcccoldfire, crosscoldfire, cross, purec, total, clean"

purec:
	@echo "building for Pure C ..."
	@$(MAKE) -f purec.mak
gcc:
	@echo "building for Gcc  ..."
	@$(MAKE) -f gcc.mak
gcccoldfire:
	@echo "building for Gcc + coldifre ..."
	@$(MAKE) -f gccm5475.mak
cross:
	@echo "building for Gcc  ..."
	@$(MAKE) -f gcc.mak CROSS=yes
crosscoldfire:
	@echo "building for Gcc + coldfire ..."
	@$(MAKE) -f gccm5475.mak CROSS=yes

clean:; $(RM) -r .gcc .purec
