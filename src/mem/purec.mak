#!make -f
#
# LDG : Gem Dynamical Libraries
# Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
#
# Makefile for compiling mem.lib (Pure C)
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
# $Id: purec.mak 9 2005-07-25 19:37:25Z bercegeay $


CC     = $(CC_PUREC)
AR     = $(LD_PUREC)
AS     = $(AS_PUREC)

CFLAGS   = -P -G -DDOSHM -I../../include -I.. -I. -I$(PURECINC)
ARFLAGS  = -J
OBJDIR   = purec
TARGET   = ../../lib/purec/mem.lib
EXTRAOBJS= 

PURECRULES=yes
include common.mak

