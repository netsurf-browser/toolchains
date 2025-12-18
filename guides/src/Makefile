# Gnu Makefile to compile Documentation

UDO=udo5.ttp
HCP=hcp.ttp

%.stg: %.u; $(UDO) --stg -o $@ $< 
../%.hyp: %.stg; $(HCP) -o$@ $<

all: ../ldg.hyp ../mem.hyp ../ldgcpx.hyp

../ldg.hyp: ldg.stg
ldg.stg: ldg.u
../mem.hyp: mem.stg
../ldgcpx.hyp: ldgcpx.stg

clean:
	$(RM) ldg.stg ldg.uhs ldg.ulh ldg.uls

