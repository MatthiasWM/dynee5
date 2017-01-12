
Foreword:

I fully respect the Copyrights associated with the Newton ROM. The
goal of this effort is primarily educational, but also meant to
make a piece of unsupported software survive a major bug as well
as the demise of the original hardware.

Plan:

Disect the Newton ROM into small pieces of readable information in order
to fix the dreaded 2010 bug and to allow moving to a new platform.

This is done by creating tools and a make system which:
- reads the ROM image and all available databases describin the ROM
- applies a collection of tools to segments in the ROM 
- creates a bunch of source code files that represent the ROM
- compiles and assembles the ROM again
- compares the original ROM with the created ROM

1:

The first goal is to segment the ROM into data and code sections,
assembly code, C code, C++ code, NewtonScript code and data 
structures and write appropriate tools to convert these segments
into ARM assembler code.

2:

The second goal is to create a source code that contains no absolute
addresses, but uses only symbolic information. Changing the order
of source files or the size of code segments should still create a
fully functional ROM.

3:

The third goal will be simplification from ARM assembler and raw 
data files into their original shape. On the simpler side, images
will be extracted and linked in as PNGs, so they can easily be 
edited and compiled back in. In ascending difficulty, we will have 
tackle NS, C, and C++ code, then compressed and encrypted resource
if such code exists (which I am pretty sure of).

4:

At this point, only a minor part of the code should still be in
ARM assembly. The next goal should be the extraction and 
interpretation of all code related to the dreaded seconds 
counter. We can fix this now.

5:

Now for the final goal: we need to replace all Cirrus specific
code with drivers for the target platform. This should be fairly
easy for the screen driver, but gets increasingly complicated
for low level functionality such as interrupt management and 
the holy grail of this port, the independence of the ARM MMU.


Preparation:

- set up a directory structure and a makefile system
   * . will contain readmes, a makefile, and the directory structure
   * NOS will receive a readable and compileable version of the ROM
   * libs will contain supporting libraries
   * tools will contain all tools we will create
   * data will contain the original image and hint files
- find tools for cross assembly and cross compilation
   * installing the iPhone SDK gets us bintools for ARM. Unfortunatly
     the iPhone SDK is locked into little endian while the Newton 
     uses big endian
   * build binutils from scratch for strongarm-elf
     I used  ../gcc-4.2.3/configure --target=strongarm-elf 
             --prefix=/usr/local/crossgcc/ --with-gnu-as 
             --with-gnu-ld --enable-languages=c,c++
   * update the path so that strongarm-elf-as and friends are available

WE ARE HERE: the basic setup is done. Putting a ROM file into the root
named "./rom.original" and calling "make" will - after a while - create
the very same code named "./rom.new"

Phase 1:

- write a library that extracts data from the Cirrus Image file
- extract the ROM code and write a single assembly file 
  containing only "unknown" directives
- extend the library to output al other information contained
  in the Cirrus image (Symbols and their use)
- write a script to demangle C++ symbols
- write a library that reads and manages the symbolic data file
- extend the symbol library to read user generated "hint"
  databases
- extend the "unknown" extractor to use symbols and hints
- write a "split" tool that manipulates the Makefile system 
  so we can easily assign new tools to segments of ROM

Phase 2:

- indentify ARM assembly code
- write a disassembler that finds embedded data in ARM code
- create a database with data labels and code labels
- indentify jump tables and pointer tables and replace 
  absolute address information with labels
- identify NewtonScript structures and replace NS pointers 
  with labels

Phase 3:

- write a tool that generates NS files that more closely 
  resemble NewtonScript
- write a tool that converts the generated NS files back
  to ARM assembly files
- write tools that extract audio and graphics data 
- extend the NS-to_ARM converter to read the audio and 
  image resources
- write a tool that converts assembly into simplified "C"
- find a "C" compiler that generated the required object 
  code from those "C" files (at this point, we will no 
  longer be able to binar-compare the generated ROM; we
  may have to come up with some form of regression testing)
- extend the NS tool to convert byte code into NewtonScript
- upgrade newt/0 so it will generate the original byte 
  code from the above files
- find encrypted and compressed data, write a decompressor
  and a re-compressor and apply the previous steps

Phase 4:

- document all code that is related to the "seconds"
  counter issue
- fix the code and generate new ROMs - test!
- generate a patch for everyone to use

Phase 5:
- remove hardware dependencies
- replace all ARM code with C code
- write drivers for new platforms
- verify!


