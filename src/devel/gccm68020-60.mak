#!make -f
#
# LDG : Gem Dynamical Libraries
# Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
#
# Makefile for compiling with Gcc 2.9.5
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
# $Id: gcc.mak 29 2005-10-19 20:32:05Z bercegeay $

ifeq ($(CROSS),yes)
CC     = m68k-atari-mint-gcc
AR     = m68k-atari-mint-ar
else
CC     = gcc
AR     = ar
endif

CFLAGS   = -m68020-60 -fomit-frame-pointer -W -Wall -O2 -I../../include -I.. -I.
ARFLAGS  = crus
OBJDIR   = .gcc
TARGET   = ../../lib/gcc/m68020-60/libldg.a
EXTRAOBJS = stdcallgcc.o

include common.mak

stdcallgcc.S: stdcall.s
	sed -e 's,\;,\|,g' -e 's,@PREFIX@,_,g' stdcall.s > stdcallgcc.S

