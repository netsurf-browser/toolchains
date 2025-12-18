#!make -f
#
# LDG : Gem Dynamical Libraries
# Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
#
# Makefile for compiling with Pure C 1.1
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
# $Id: purec.mak 127 2015-06-29 19:34:16Z landemarre $


CC     = $(CC_PUREC)
AR     = $(LD_PUREC)
AS     = $(AS_PUREC)

CFLAGS   = -P -I../../include -I.. -I. -I$(PURECINC)
ARFLAGS  = -J
OBJDIR   = .purec
TARGET   = ../../lib/purec/ldg.lib
EXTRAOBJS = stdcapc.o

PURECRULES=yes
include common.mak

stdcapc.s: stdcall.s
	sed 's,@PREFIX@,,g;s,#ifdef,IFB,g;s,#else,ELSE,g;s,#endif,ENDIF,g;' stdcall.s > stdcapc.s
