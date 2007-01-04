
all: mpdumper

CFILES = \
	cplus-dem.c \
	safe-ctype.c

CXXFILES = \
	mpdumper.cpp

OBJECTS = $(CXXFILES:.cxx=.o) $(CFILES:.c=.o)

PREDEFS = -D HAVE_STDLIB_H

CC = gcc
CFLAGS = -g $(PREDEFS) -c

CXX = g++
CXXFLAGS = -g $(PREDEFS) -c

LDFLAGS = -g 

LDLIBS =

POSTBUILD = /Developer/Tools/Rez -t APPL -o

mpdumper: $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

clean:
	rm *.o
	rm mpdumper

depend:
	mkdep $(CFLAGS) $(CFILES)
	mkdep -a $(CXXFLAGS) $(CXXFILES)

.SUFFIXES: .cxx .c .h .o

.cxx.o:
	echo Compiling $<...
	$(CXX) $(CXXFLAGS) $< -o $@

.c.o:
	echo Compiling $<...
	$(CC) $(CFLAGS) $< -o $@

