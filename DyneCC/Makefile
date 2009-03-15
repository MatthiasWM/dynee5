
all: rom.new 

diff:
	cmp -b rom.original rom.new

rom.new: rom.elf
	strongarm-elf-objcopy -O binary -j .text rom.elf rom.new

rom.elf: main.o
	strongarm-elf-ld -EB -e 0 main.o -o rom.elf

main.o: main.a
	strongarm-elf-as -EB main.a -o main.o

main.a: rom.original tools/dump_unknown
	tools/dump_unknown rom.original main.a 00000000 00800000

tools/dump_unknown: tools/dump_unknown.cxx
	g++ -g -o tools/dump_unknown tools/dump_unknown.cxx



