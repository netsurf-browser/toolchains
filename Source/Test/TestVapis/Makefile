VALAC := valac
VAPIS := $(wildcard $(VAPIDIR)/*.vapi)

.PHONY:	all clean

all: singlepackages allpackages

.SUFFIXES:
.SUFFIXES: .c .vapi

allpackages: allpackages.vala $(VAPIS)
	$(VALAC) allpackages.vala -C $(addprefix --pkg=,$(basename $(notdir $(VAPIS)))) --vapidir $(VAPIDIR)

allpackages.vala: vapitest.tpl
	$(CP) vapitest.tpl $@

singlepackages: $(addsuffix .c,$(filter-out OSLibTypes,$(basename $(notdir $(VAPIS)))))


%.c:	$(VAPIDIR)/%.vapi
	@echo "Testing $(notdir $<)"
	@echo "using $(subst OSLib,OSLib.,$(basename $@));\n" > $(addsuffix .vala,$(basename $@))
	@cat vapitest.tpl >> $(addsuffix .vala,$(basename $@))
	@$(VALAC) -C $(addsuffix .vala,$(basename $@)) --pkg=$(basename $@) --vapidir $(VAPIDIR)

clean:	;-$(RM) *.c *.vala

