
all: ROM

ROM: newtonos
	strongarm-elf-objcopy -Obinary newtonos ROM
	../build/Debug/armdiff

newtonos: newtonos.o
	strongarm-elf-ld -EB -Ttext 0 -o newtonos newtonos.o

newtonos.o: newtonos.s macros.s Makefile
	strongarm-elf-as -mapcs-32 -mbig-endian -mcpu=strongarm110 newtonos.s -o newtonos.o

clean:
	rm newtonos.*
	rm newtonos
	rm *~
	rm a b
	rm ROM

