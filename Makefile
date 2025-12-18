#  makefile to build packages of LDG and LDG-DEV 

include src/VERSION

.PHONY : srctgz rpmall;

srctgz:
	cd src/devel && make clean
	rm -rf ../ldg-dev-$(VERSION)
	mkdir -p ../ldg-dev-$(VERSION)/src
	cp -r src/devel  src/*.h ../ldg-dev-$(VERSION)/src
	cp -r include ../ldg-dev-$(VERSION)
	cd .. && tar cvzf ldg-dev-$(VERSION).tar.gz --exclude=CVS ldg-dev-$(VERSION)
	rm -rf ../ldg-dev-$(VERSION)

rpmall: srctgz
	cp src/ldg-dev.spec /usr/src/redhat/SPECS
	cp ../ldg-dev-$(VERSION).tar.gz /usr/src/redhat/SOURCES
	cd /usr/src/redhat/SPECS && rpm -ba ldg-dev.spec
	
libtgz:
	rm -rf ../ldg-dev-$(VERSION)
	mkdir -p ../ldg-dev-$(VERSION)/include
	cp -r lib     ../ldg-dev-$(VERSION)
	cp -r include/*.h ../ldg-dev-$(VERSION)/include
	rm -rf ../ldg-dev-$(VERSION)/lib/CVS \
	       ../ldg-dev-$(VERSION)/lib/gcc/CVS \
	       ../ldg-dev-$(VERSION)/lib/gcc281/CVS \
	       ../ldg-dev-$(VERSION)/lib/purec/CVS \
	       ../ldg-dev-$(VERSION)/lib/sozobon/CVS
	cd .. && rm -f ldg-dev-$(VERSION).zip && zip -r ldg-dev-$(VERSION).zip ldg-dev-$(VERSION) -x CVS
	rm -rf ../ldg-dev-$(VERSION)
