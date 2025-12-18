# Top makefile for building all the tools used for OSLib build.
# Only the tools needed during the cross-compile build are built.

ALL = BindHelp DefMod2 IndexHelp ReduceAOF

.PHONY: ${ALL} install clean

all:	${ALL}

BindHelp:
	${MAKE} -C BindHelp

DefMod2:
	${MAKE} -C DefMod2

IndexHelp:
	${MAKE} -C IndexHelp

ReduceAOF:
	${MAKE} -C ReduceAOF

install:
	$(foreach TARGET, ${ALL}, ${MAKE} -C ${TARGET} install; )

clean:
	$(foreach TARGET, ${ALL}, ${MAKE} -C ${TARGET} clean; )
