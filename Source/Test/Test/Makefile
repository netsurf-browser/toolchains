# Test the inclusion of all C and assembler headers (the former in C and C++
# programs.

ALL_CHEADERS := $(wildcard $(HEADERDIR)/*.h)
ALL_ASMHEADERS := $(wildcard $(HEADERDIR)/*.Hdr)

.PHONY: all clean

all: test_c$(EXEEXT) test_cpp$(EXEEXT) test_s$(EXEEXT)

clean:
	-$(RM) test_c,ff8 test_c,e1f test_cpp,ff8 test_cpp,e1f test_s,ff8 test_s,e1f test_s,ff8.o test_s,e1f.o small_test.o all_oslib.h all_oslib.Hdr

test_c$(EXEEXT): test.c small_test.o all_oslib.h
	$(CC) -o $@ -I$(TOPHEADERDIR) test.c small_test.o $(OSLIBLIB)

test_cpp$(EXEEXT): test.cc all_oslib.h
	$(CXX) -o $@ -I$(TOPHEADERDIR) $< $(OSLIBLIB)

test_s$(EXEEXT): test$(ASMEXT) all_oslib.Hdr
	$(AS) -o $@.o -I$(TOPHEADERDIR) $<
	$(CC) -o $@ $@.o

small_test.o: small_test$(ASMEXT) all_oslib.Hdr
	$(AS) -o $@ -I$(TOPHEADERDIR) $<

all_oslib.h: $(ALL_CHEADERS)
	-$(RM) $@
	cat $(ALL_CHEADERS) > all_oslib.h

all_oslib.Hdr: $(ALL_ASMHEADERS)
	-$(RM) $@
	cat $(ALL_ASMHEADERS) > all_oslib.Hdr
