
all: ROM

ROM: newtonos
	strongarm-elf-objcopy -Obinary newtonos ROM
	# strongarm-elf-objdump -D test
	diff ROM ../data/717006

newtonos: newtonos.o
	strongarm-elf-ld -EB -Ttext 0 -o newtonos newtonos.o

newtonos.o: newtonos.s macros.s Makefile
	strongarm-elf-as -mapcs-32 -mbig-endian -mcpu=strongarm110 newtonos.s -o newtonos.o

