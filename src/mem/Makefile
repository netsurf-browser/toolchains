# Makefile for building mem.ldg and libraries

all: libs mem.ldg

mem.ldg:
	@echo Building ldg.mem ...	
	@$(MAKE) -f purec.mak ldg
	
libs:
	@echo Building Pure C library ...
	@$(RM) mem.o
	@$(MAKE) -f purec.mak lib
	@$(RM) mem.o mem16.o
	@echo Building Gcc 2.8.1 libraries ...
	@$(MAKE) -f gcc281.mak
	@$(RM) mem.o mem16.o
	@echo Building Gcc 2.95.3 libraries ...
	@$(MAKE) -f gcc295.mak
	@$(RM) mem.o
	@echo Building Sozobon X library ...
	@$(RM) mem.o
	@$(MAKE) -f soz.mak

test:
	@$(MAKE) -f testmem.mak
	
clean:
	@$(MAKE) -f gcc281.mak clean

