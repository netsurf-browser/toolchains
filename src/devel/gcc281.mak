#!make -f
#
# LDG : Gem Dynamical Libraries
# Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
#
# Makefile for compiling with Gcc 2.8.1
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
# $Id: gcc281.mak 29 2005-10-19 20:32:05Z bercegeay $

ifeq ($(INTSIZE),16)
OBJDIR  = .gcc281-16
CFLAGS  = -W -Wall -O -I../../include -I.. -I. -mshort
else
OBJDIR = .gcc281
CFLAGS  = -W -Wall -O -I../../include -I.. -I.
endif

CC      = $(CC_GCC281)
AR      = $(AR_GCC281)
ARFLAGS = crus
TARGET  = ../../lib/gcc281/libldg$(INTSIZE).olb
EXTRAOBJS = stdcallgcc.o

include common.mak

stdcallgcc.s: stdcall.s
	sed -e 's,\;,\|,g' -e 's,@PREFIX@,_,g' stdcall.s > stdcallgcc.s
