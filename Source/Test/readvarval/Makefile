# Test os_read_var_val_size interface.

.PHONY:	all clean

all:	readvarval$(EXEEXT)

clean:
	-$(RM) readvarval,ff8 readvarval,e1f readvarval.o

readvarval$(EXEEXT): readvarval.o $(OSLIBLIB)
	$(CC) -o $@ $^

.SUFFIXES: .o .c

.c.o:
	$(CC) -I$(TOPHEADERDIR) -c $< -o $@
