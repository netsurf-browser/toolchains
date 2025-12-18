# Librairie Dynamique GEM
# Olivier Landemarre, Dominique B‚r‚ziat, Arnaud Bercegeay
# tous droits r‚serv‚s 1997/1998/1999/2000/2001/2002/2012
#
# makefile to build LDG manager

all:; @echo "Possible targets are: gcc, gcccoldfire, crosscoldfire, cross, purec, total, clean"

purec:
	@echo "building for Pure C ..."
	@$(MAKE) -f purec.mak
gcc:
	@echo "building for Gcc  ..."
	@$(MAKE) -f gcc.mak
gcccoldfire:
	@echo "building for Gcc + coldifre ..."
	@$(MAKE) -f gccm5475.mak
cross:
	@echo "building for Gcc  ..."
	@$(MAKE) -f gcc.mak CROSS=yes
crosscoldfire:
	@echo "building for Gcc + coldfire ..."
	@$(MAKE) -f gccm5475.mak CROSS=yes

clean:; $(RM) -r .gcc .purec

