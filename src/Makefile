########################################################
#	Compile the whole ldg distribution
#   libdev puis le reste (kernel, ldgd, ldgmgr)
#	target utils : ldginfo, mem.ldg, ldg.cpx
########################################################

all: 
	@echo Select a target : kernel, libs, utils, total
	
total: kernel libs utils

kernel: PHONY
	@echo building devel Pure C library
	@echo -----------------------------	
	@echo devel
	@make -C devel -f purec.mak
	@echo <== devel

	@echo building LDG kernel
	@echo -------------------
	@echo ==> kernel
	@make -C kernel
	@echo <== kernel

	@echo building LDG daemon
	@echo -------------------
	@echo ==> ldgd	
	@make -C ldgd
	@echo <== ldgd

	@echo building LDG manager
	@echo --------------------
	@echo ==> ldgmgr
	@make -C ldgmgr
	@echo <== ldgmgr

libs:
	@echo Building devel libraries for other compiler
	@echo -------------------------------------------
	
	@make -C devel
	

	

utils:
	@echo Building utility programs
	@echo -------------------------
	@echo ==> ldginfo
	@make -C ldginfo
	@echo <== ldginfo

	@echo ==> mem
	@make -C mem
	@echo <== mem

	@echo ==> cpx
	@make -C cpx
	@echo <== cpx

clean:
	@make -C devel clean
	@make -C kernel clean
	@make -C ldgd clean
	@make -C ldgmgr clean
	@make -C cpx clean
	@make -C mem clean

PHONY:

#eof
