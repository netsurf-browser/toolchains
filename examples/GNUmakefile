# Makefile for OSLib examples

# ***********************************
# ***    C h a n g e   L i s t    ***
# ***********************************
# Date          Name         Description
# ----          ----         -----------
# 16-Aug-94     JC           Created.
# 05-Sep-94     AMcC         Added Generic RISC OS Build rules
# 27th Feb 1995 JC           Added Macros headers
# 21st Jun 1995 JRC          Split into 4 pieces
# 11th Apr 2001 TV   	     Amended Include path
#	       	     	     removed dynamic dependencies; they don't help
# 07th Jul 2004 TV	     new version renamed GNUmakefile for building under GNU/Unix

#------------------------------------------------------------------------
# paths & directories
LIBDIR		= ../release/unix/OSLib/
INCLUDE 	= -I${LIBDIR}

#------------------------------------------------------------------------
# tools
AS		= ${GCCSDK_INSTALL_CROSSBIN}/gcc -xassembler -Wa,${ASOPTIONS}
GCC		= ${GCCSDK_INSTALL_CROSSBIN}/gcc
GPP		= ${GCCSDK_INSTALL_CROSSBIN}/g++
LD		= ${GCCSDK_INSTALL_CROSSBIN}/gcc
RM		= rm -f


EXAMPLES = \
Ex0 \
Ex1 \
Ex2 \
Helloworld \
P1-046 \
P1-053 \
P1-067 \
P1-068 \
P1-108 \
P1-109 \
P1-110 \
P1-219 \
P1-332 \
P1-334 \
P1-467 \
P1-606 \
P1-754 \
P1-833 \
P1-834 \
P2-042 \
P2-083 \
P2-099 \
P2-383 \
P2-413 \
P2-491 \
P2-492 \
P2-493 \
P2-524 \
P2-526 \
P2-589 \
P2-615 \
P2-616 \
P2-617 \
P2-618 \
P2-619 \
P2-625 \
P2-629 \
P2-632 \
P2-634 \
P2-636 \
P2-638 \
P2-691 \
P2-735 \
P2-740 \
P2-743 \
P3-018 \
P3-023 \
P3-024 \
P3-025 \
P3-063 \
P3-070 \
P3-072 \
P3-073 \
P3-162 \
P3-185 \
P3-198 \
P3-231 \
P3-246 \
P3-411 \
P3-421 \
P3-433 \
P3-469 \
P3-549 \
P3-659 \
P3-743

all:		examples

examples:	$(addsuffix .o,${EXAMPLES} )

clean:		;${RM} $(addsuffix .o,${EXAMPLES} )

.SUFFIXES: .c .o
.c.o:		;${GCC} -c ${INCLUDE} -D__swi -o $@ $<

#------------------------------------------------------------------------
# Dynamic dependencies:
