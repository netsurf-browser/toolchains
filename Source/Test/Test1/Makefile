# This test will include all C headers and ASM headers one for one and see if
# they either can be used stand-alone, either pull enough other headers files
# in.

ALL_CHEADERS := $(wildcard $(HEADERDIR)/*.h)
ALL_ASMHEADERS := $(wildcard $(HEADERDIR)/*.Hdr)

.PHONY:		all clean

TEST_C		= test.c
TEST_S		= test$(ASMEXT)

OBJ_C	:= $(addprefix c_, $(addsuffix .o, $(basename $(notdir $(ALL_CHEADERS)))))
OBJ_S	:= $(addprefix s_, $(addsuffix .o, $(basename $(notdir $(ALL_ASMHEADERS)))))

all:	$(OBJ_C) $(OBJ_S)

.SUFFIXES:
.SUFFIXES: .o .h .Hdr

c_%.o:	$(HEADERDIR)/%.h
	@printf "Testing $(subst .o,.h,$(subst c_,,$@))\n"
	@printf "#include \"oslib/$(subst .o,.h,$(subst c_,,$@))\"\n" > include.h
	@$(CC) -c $(TEST_C) -I$(TOPHEADERDIR) -o /dev/null
	@touch $@

ifeq ($(BUILDTYPE),CROSSGCC_ELF)
s_%.o:	$(HEADERDIR)/%.Hdr
	@printf "Testing $(subst .o,.Hdr,$(subst s_,,$@)) (ELF)\n"
	@printf "#include \"oslib/$(subst .o,.Hdr,$(subst s_,,$@))\"\n" > include.Hdr
	@$(AS) $(TEST_S) -I$(TOPHEADERDIR) -o /dev/null
	@touch $@
else
s_%.o:	$(HEADERDIR)/%.Hdr
	@printf "Testing $(subst .o,.Hdr,$(subst s_,,$@)) (AOF)\n"
	@printf "\tGET\toslib/$(subst .o,.Hdr,$(subst s_,,$@))\n" > include.Hdr
	@$(AS) $(TEST_S) -I$(TOPHEADERDIR) -o /dev/null
	@touch $@
endif

clean:	;-$(RM) include.h include.Hdr c_*.o s_*.o
