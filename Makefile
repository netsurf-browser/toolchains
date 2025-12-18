#  makefile to build packages of windom

include VERSION

.PHONY : srctgz rpmall bigzip;

srctgz:
	make clean
	rm -rf ../windom-$(RPMSUFFIX)
	mkdir -p  ../windom-$(RPMSUFFIX)
	cp -r * ../windom-$(RPMSUFFIX)
	rm -rf ../windom-$(RPMSUFFIX)/lib
	cd .. && tar cvzf windom-$(RPMSUFFIX).tar.gz --exclude=CVS --exclude=.* --exclude=*~ --exclude=*.app --exclude=*.prg --exclude=*.ldg windom-$(RPMSUFFIX)
	rm -rf ../windom-$(RPMSUFFIX)

rpmall: srctgz
	cp src/rpm/windom.spec /usr/src/redhat/SPECS
	cp ../windom-$(VERSION).tar.gz /usr/src/redhat/SOURCES
	cd /usr/src/redhat/SPECS && rpm -ba windom.spec
	
bigzip:
	cd src && make clean
	cd demo && make clean
	cd examples && make clean
	rm -rf ../windom-$(VERSION)
	mkdir -p ../windom-$(VERSION)
	cp -r *   ../windom-$(VERSION)
	cd ../windom-$(VERSION) && \
	rm -rf CVS demo/CVS doc/CVS doc/tutorial examples/CVS examples/*/CVS \
	       include/CVS include/windom/CVS userdef/CVS userdef/*/CVS src/CVS src/*/CVS
	cd .. && rm -f windom-$(VERSION).zip && zip -r windom-$(VERSION).zip windom-$(VERSION)
	rm -rf ../windom-$(VERSION)

clean:
	cd src && make clean
	cd userdef && make clean
	cd examples && make clean
	cd demo && make clean
	rm -f *~
	
