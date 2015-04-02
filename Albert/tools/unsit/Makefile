GETOPT = 
#GETOPT = getopt.o

CFLAGS = -O2

unsit : unsit.o updcrc.o $(GETOPT)
	$(CC) $(LDFLAGS) $(CFLAGS) -o unsit unsit.o updcrc.o $(GETOPT)

unsit.o : unsit.c stuffit.h
updcrc.o : updcrc.c
getopt.o : getopt.c

unsit.shar : README Makefile unsit.c stuffit.h updcrc.c getopt.c unsit.1
	shar README Makefile unsit.c stuffit.h updcrc.c getopt.c unsit.1 \
	>unsit.shar

install: unsit unsit.1
	install -c unsit /usr/local/bin
	install -c unsit.1 /usr/local/man/man1

clean:
	rm -f *.o unsit *~

## Changes to the unsit 1.5f package for compilation on the NeXT
##	1.  Modified the makefile
##		a.  added the CFLAGS line for optimized fat binary compiles
##		b.  added the updcrc.o line to force compilation of that module
##	2.  Modified Stuffit.h to actually typedef the structures,
##		from typedef struct xHdr {} to typedef struct _xHdr {} xHdr;
##	3.  Modified unsit.c
##		a.  to use the newly typedef'd structures
##		b.  to include string.h to prototype strchr()
##	- Subrata Sircar, 8/6/93
