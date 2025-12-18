#!make -f
#
# LDG : Gem Dynamical Libraries
# Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
#
# Main makefile for compiling developper C-libraries
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

all:; @echo "Possible targets are: gcc, cross, gcc281, purec, sozobon, total, clean"
purec:
	@echo "building for Pure C ..."
	@$(MAKE) -f purec.mak
vbcc:
	@echo "building for VBCC ..."
	@$(MAKE) -f vbcc.mak
ahcc:
	@echo "building for AHCC ..."
	@$(MAKE) -f ahcc.mak
gcc:
	@echo "building for Gcc  ..."
	@$(MAKE) -f gcc.mak
gcc020:
	@echo "building for Gcc + 020+ ..."
	@$(MAKE) -f gccm68020-60.mak
gcccoldfire:
	@echo "building for Gcc + coldifre ..."
	@$(MAKE) -f gccm5475.mak
cross:
	@echo "building for Gcc  ..."
	@$(MAKE) clean && $(MAKE) -f gcc.mak CROSS=yes
	@$(MAKE) clean && $(MAKE) -f gccm68020-60.mak CROSS=yes
	@$(MAKE) clean && $(MAKE) -f gccm5475.mak CROSS=yes
gcc281:
	@echo "building for Gcc 2.8.1 with short int ..."
	@$(MAKE) -f gcc281.mak INTSIZE=16
	@echo "building for Gcc 2.8.1 with long int ..."
	@$(MAKE) -f gcc281.mak 
sozobon:
	@echo "building for Sozobon 2 ..."
	@$(MAKE) -f sozobon.mak
total: clean purec gcc gcc281 sozobon
clean:; $(RM) -r .gcc .gcc281 .gcc281-16 .purec .sozobon stdcallsc.s stdcapc.s stdcallgcc.S
