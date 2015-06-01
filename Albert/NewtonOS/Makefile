BINUTILS_PATH = /usr/local/crossgcc/strongarm-elf/bin
OBJCOPY = $(BINUTILS_PATH)/objcopy
AS = $(BINUTILS_PATH)/as
LD = $(BINUTILS_PATH)/ld

all: ROM

ROM: newtonos
	$(OBJCOPY) -Obinary newtonos ROM
	../build/Debug/armdiff

newtonos: newtonos.o
	 $(LD) -EB -Ttext 0 -o newtonos newtonos.o

newtonos.o: newtonos.s macros.s Makefile
	$(AS) -mapcs-32 -mbig-endian -mcpu=strongarm110 newtonos.s -o newtonos.o

clean:
	rm newtonos.*
	rm newtonos
	rm *~
	rm a b
	rm ROM

