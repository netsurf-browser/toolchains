# Targets: all install clean
# Using: CC CFLAGS LD LDFLAGS RM INSTALL INSTALL_PROGRAM bindir

.PHONY: all install clean
OBJS = \
	reduceaof.o

# In case they are not defined, use default values.
CC      ?= /usr/bin/gcc
CFLAGS  ?= -g
LD      ?= /usr/bin/gcc
LDFLAGS ?= 
RM      ?= rm
INSTALL ?= cp
INSTALL_PROGRAM ?= $(INSTALL)
bindir ?= ../../Bin

# No user options from here on:
ALL_CFLAGS = -Wall -funsigned-char
ALL_LDFLAGS = $(LDFLAGS)

TARGET = reduceaof

all: $(TARGET)

install: $(TARGET)
	$(INSTALL_PROGRAM) $(TARGET) $(bindir)/$(TARGET)

clean:
	-$(RM) $(TARGET) *.o $(bindir)/$(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(ALL_LDFLAGS) -o $@ $(OBJS)

# Rules:
.SUFFIXES:
.SUFFIXES: .o .c

.c.o: ;$(CC) $(ALL_CFLAGS) -o $@ -c $<
