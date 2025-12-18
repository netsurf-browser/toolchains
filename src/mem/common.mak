#! make -f
#
# LDG : Gem Dynamical Libraries
# Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
#
# Common definition for building mem-devel
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
# $Id: common.mak 9 2005-07-25 19:37:25Z bercegeay $

OBJS    = mem.c
OBJECTS = $(addprefix $(OBJDIR)/,$(OBJS))
TARGETD = $(dir $(TARGET))

# Rules
ifeq ($(PURECRULES),yes)
$(OBJDIR)/%.o: %.c $(OBJDIR)
	@echo CC $(notdir $@)
	@$(CC) $(CFLAGS) -O=$@ $<
$(OBJDIR)/%.o: %.s $(OBJDIR)
	@echo AS $(notdir $@)
	@$(AS) $(ASFLAGS) -O=$@ $<
$(TARGET): $(OBJECTS) $(TARGETD)
	@echo LD $(notdir $@)
	@$(AR) $(ARFLAGS) -O=$@ $(OBJECTS)
else
$(OBJDIR)/%.o: %.c $(OBJDIR)
	@echo CC $(notdir $@)
	@$(CC) -c $(CFLAGS) $< -o $@
$(OBJDIR)/%.o: %.s $(OBJDIR)
	@echo AS $(notdir $@)
	@$(CC) -c $(CFLAGS) $< -o $@
$(TARGET): $(OBJECTS) $(TARGETD)
	@echo LD $(notdir $@)
	@$(AR) $(ARFLAGS) $@ $(OBJECTS)
endif

# dependencies
$(OBJDIR):; mkdir $(OBJDIR)
$(TARGETD):; mkdir -p $@
$(OBJECTS): globals.h mem.h

clean:; $(RM) -r $(OBJDIR)
