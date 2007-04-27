

all: dyne

CFILES = \
	disarm/roughdisarm.c \
	disarm/roughdisarm-cc.c \
	armulator/armcopro.c \
	armulator/armemu.c \
	armulator/arminit.c \
	armulator/armmem.c \
	armulator/armmmu.c \
	armulator/armos.c \
	armulator/armsupp.c \
	armulator/armvirt.c \
	armulator/mmu/cache.c \
	armulator/mmu/rb.c \
	armulator/mmu/sa_mmu.c \
	armulator/mmu/tlb.c \
	armulator/mmu/wb.c \
	armulator/thumbemu.c \
	armulator/wrapper.c

CXXFILES = \
	main.cxx \
	asmview.cxx \
	flash.cxx \
	registerview.cxx \
	hexview.cxx \
	memory.cxx \
	native.cxx \
        cpu.cxx \
	screen.cxx \
	symbols.cxx \
	desym.cxx

FLFILES = \
	dyne.fl

FLCXXFILES = $(FLFILES:.fl=.cxx)

OBJECTS = $(CXXFILES:.cxx=.o) $(FLCXXFILES:.cxx=.o) $(CFILES:.c=.o)

PREDEFS = -DMODET -DNO_DBCT -DMODE32

CC = gcc
CFLAGS = -g $(PREDEFS) -I/usr/local/include -I/usr/local/include/FL/images -c

CXX = g++
CXXFLAGS = -g $(PREDEFS) -I/usr/local/include -I/usr/local/include/FL/images -c

LDFLAGS = -g $(shell fltk-config --ldflags)

LDLIBS =  $(shell fltk-config --libs)

POSTBUILD = fltk-config --post

dyne: $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)
	$(POSTBUILD) $@

clean:
	rm *.o
	rm disarm/*.o
	rm armulator/*.o

depend:
	mkdep $(CFLAGS) $(CFILES)
	mkdep -a $(CXXFLAGS) $(CXXFILES)

.SUFFIXES: .cxx .c .h .fl .o

.cxx.o:
	echo Compiling $<...
	$(CXX) $(CXXFLAGS) $< -o $@

.c.o:
	echo Compiling $<...
	$(CC) $(CFLAGS) $< -o $@

.fl.cxx:
	echo Fluid $<...
	fluid -c $<


