
  A L B E R T
 =============
 

  1. What is this?
  2. What does it do?
  3. What's so difficult then? (Are we there yet?)
  4. What will be added?
  
  
 What is this?
---------------

Do you remember the Apple MessagePad, also commonly known as the Newton? Made in
1993, it was arguably the first PDA ever. It had some fascinating features that
a surprisingly large and active community of fans would like to preserve.

One huge stap in this direction was made when Paul Guyot wrote "Einstein", and
emulator that uses a copy of the original MessagePad ROM to emulate the perfect
NewtonOS experience on desktop machines.

"Albert" is a set of tools that goes beyond emulation. It tries to understand
every bit of code in the ROM and convert a mess of bytes and words into a 
logical hierarchy of source code files which can be understood by a human,
modified and fixed, and reassembled into a better ROM.

Ultimately, Albert will provide a binary translation of the NewtonOS ROM into 
a new file format that can be run on many other platforms.


 What does it do?
------------------

Albert uses static analysis to understand as much of the ROM as possible. 

It applies all known symbolic information to the ROM addresses.

It finds ARM machine code and seperates code and data word. 

It finds NewtonScript code and converts it into readbale format. Bitmaps are 
written as .bmp files.

It also recognizes jump tables and replaces binary information with symbolic
labes.

The output is a pretty huge ARM assembler code file containing symbolic 
information and comments throughout, which can then be recompiled back into the 
original ROM code using the GNU assembler for ARM. A second toll, armdiff,
verifies that the original ROM and the generated ROM are the same.

Albert can also output a group of pseudo C++ code files. They will not compile,
but should be sufficient to be analysed by Doxygen and onther C++ code analysis
tools to genrate call graphs and other interesting information.


 What's so difficult then? (Are we there yet?)
-----------------------------------------------

The task will we complete when we can change code in our source files around, 
and the compilition will still generate a valid ROM.

The biggest problem on the way there is to correctly identify sybolic 
information. Only if all address pointers in the ROM are correctly identified
and replced with a symbolic pointer, changes in code position will still
generate correct code.

The only way to do this is to analyse all data word and find out if they are
pointers, numeric data (integer or floating point), ASCII text, Unicode text,
ID's (4 byte ASCII style magic words), or maybe even two packed 16 bit values
as in graphics coordinates.

Only if we can qualify every byte in the ROM, twe can generate a new ROM for a
different platform.


 What will be added?
---------------------

Currenltly, the source code is a mess. Sorry about that - it was never meant to
be published. Nevertheless, it outputs quite informative code and correctly
recompiles into a fresh ROM. It has already al little databse function for 
understanding C++ classes which are not used yet.

So here's my todo list:

  - General
    - clean up as we go along
  - Jump Tables
    - generate the correct offsets for the remaining jump tables
    - verify
  - ARM code section
    - understand and label every word in the code (grep "\.word")
    - create symbolic information
    - use Einstein to record byte vs. word access to ROM
    - use class information to analyse byte offsets
  - NewtonScript code section
    - understand and label all data byte
    - generate symbolic labels for jump instructions in Byte Code
  - Databases
    - not much done here yet: try to understand and disassemble the compressed
      section
    - reassemble and compress whatever we find out
  - ROM Extensions
    - develop strategy
 
 
  - Matt