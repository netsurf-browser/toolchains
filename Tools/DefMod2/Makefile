# Targets: all install clean
# Using: CC CFLAGS LD LDFLAGS YACC YFLAGS RM INSTALL INSTALL_PROGRAM bindir

.PHONY: all install clean
OBJS = \
	assembler.o \
	asmhelp.o \
	cheader.o \
	chelp.o \
	cstrong.o \
	def.o \
	defmod.o \
	hdr.o \
	oslibsupport.o \
	oslib_licence.o \
	utils.o \
	vapi.o \
	vapi_deps.o \
	vapi_h.o

# In case they are not defined, use default values.
CC      ?= /usr/bin/gcc
CFLAGS  ?= -g -DUNIX -DEXECUTE_ON_UNIX -D__swi
LD      ?= /usr/bin/gcc
LDFLAGS ?= 
YACC    ?= bison
YFLAGS  ?=
RM      ?= rm
INSTALL ?= cp
INSTALL_PROGRAM ?= $(INSTALL)
bindir ?= ../../Bin

# No user options from here on:
ALL_CFLAGS = -Wall -funsigned-char -I.. -I../support $(CFLAGS)
ALL_LDFLAGS = $(LDFLAGS)

TARGET = defmod

all: $(TARGET)

install: $(TARGET)
	$(INSTALL_PROGRAM) $(TARGET) $(bindir)/$(TARGET)

clean:
	-$(RM) $(TARGET) *.o defmod.c $(bindir)/$(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(ALL_LDFLAGS) -o $@ $(OBJS)

# Rules:
.SUFFIXES:
.SUFFIXES: .o .c .y

.c.o: ;$(CC) $(ALL_CFLAGS) -o $@ -c $<
.y.c: ;$(YACC) -y -o $@ $<
