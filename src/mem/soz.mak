CC     = $(CC_SOX)
AR     = $(AR_SOX)
CFLAGS = -I..\..\include
TARGET = ..\..\lib\sozobon\mem.a

all: $(TARGET)

$(TARGET): mem.o
	rm -f $(TARGET)
	$(AR) -r $(TARGET) mem.o

clean:
	$(RM) -f mem.o

