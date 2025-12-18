#!make -f
#
# TEXTURED.LDG: a userdef library for windom
# Copyright (c) 2005 Arnaud Bercegeay
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
# $Source: /cvsroot/windom/windom/userdef/textured/Makefile,v $
# 
# CVS Info :
#  $Author: bercegeay $
#  $Date: 2005/02/09 21:12:22 $
#  $Revision: 1.5 $

#
# makefile for windom 
# gcc >= 2.95.3 (sparemint) version
#

ifeq ($(CROSS),yes)
CROSSPREFIX = m68k-atari-mint
CC     = $(CROSSPREFIX)-gcc
AR     = $(CROSSPREFIX)-ar
PREFIX = /usr/$(CROSSPREFIX)
else
CC     = gcc
AR     = ar
PREFIX = /usr
endif

WARN = \
	-Wall \
	-Wmissing-prototypes \
	-Winline \
	-Wshadow \
	-Wpointer-arith \
	-Wcast-qual \
	-Waggregate-return

CFLAGS= -O2 -fomit-frame-pointer -I../../include $(WARN) $(M68K_ATARI_MINT_CFLAGS)

OBJ= interface.o draw.o extend.o xedit.o string.o char.o ldg.o loadimg.o

OBJDIR = .gcc/
OBJECTS=$(addprefix $(OBJDIR),$(OBJ))

OBJTOSDIR = .gcctos/
OBJECTSTOS=$(addprefix $(OBJTOSDIR),$(OBJ))

LDGPATH= ../../gemsys/ldg/windom/userdef

TARGET_STD = $(LDGPATH)/textured.ldg
TARGET_TOS = $(LDGPATH)/tos/textured.ldg

IMG = ice.png rough_paper.png rough_paper_l.png
IMGPATH = $(LDGPATH)/img

.SUFFIXES:
.SUFFIXES: .c .S .o

$(OBJDIR)%.o: %.c $(OBJDIR)
	@echo CC $@
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJTOSDIR)%.o: %.c $(OBJTOSDIR)
	@echo CC $@
	@$(CC) $(CFLAGS) -DUDEFTOS=1 -c $< -o $@

all: $(TARGET_STD) $(TARGET_TOS) img

$(TARGET_STD): $(OBJECTS)
	mkdir -p $(dir $@)
	$(CC) -s -o $@ $(OBJECTS) -lgem -lldg

$(TARGET_TOS): $(OBJECTSTOS)
	mkdir -p $(dir $@)
	$(CC) -s -o $@ $(OBJECTSTOS) -lgem -lldg

$(OBJECTS): global.h

$(OBJDIR):; mkdir $(OBJDIR)

$(OBJTOSDIR):; mkdir $(OBJTOSDIR)

img:
	mkdir -p $(IMGPATH)
	cp -fv $(IMG) $(IMGPATH)
	
cross:
	$(MAKE) CROSS=yes

clean:
	rm -rf $(OBJDIR) $(OBJTOSDIR)

