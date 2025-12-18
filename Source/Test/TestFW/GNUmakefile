# Index: OSLib/!OsLib/Source/Test/TestFW/GNUmakefile
#===================================================================

#------------------------------------------------------------------------
# paths & directories
SOURCEDIR	= ../..
include ${SOURCEDIR}/GNUmakefile.settings
include ${SOURCEDIR}/../GNUmakefile.settings

#------------------------------------------------------------------------
#target-specific macros
TARGET	 = TestFW
OBJS  	 = ${TARGET}.o
BIN   	 = ${TARGET},ff8
OSLIB		= ${SOURCEDIR}/OSLib.o
OSLIB32		= ${SOURCEDIR}/OSLib32.o

SOURCES 	= Computer Core Toolbox User Types Macros
INCLUDES	= $(addprefix -I${SOURCEDIR}/, ${SOURCES} )


#------------------------------------------------------------------------
#Tools
GCC		= ${GCCSDK_INSTALL_CROSSBIN}/gcc
LD		= ${GCCSDK_INSTALL_CROSSBIN}/gcc
RM		= rm -f

#------------------------------------------------------------------------
#Toolflags

#------------------------------------------------------------------------
# Targets
.PHONY:	all clean

all:	${BIN}

${BIN}:	${OBJS} ${OSLIB32}
	${LD} -o $@ $^

clean:
	-${RM} ${OBJS} ${BIN}

#------------------------------------------------------------------------
#General rules
.SUFFIXES: .o .c

.c.o:
	${GCC} -c ${INCLUDES} $< -o $@

