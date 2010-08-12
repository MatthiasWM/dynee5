/*
 *  sym2db.cpp
 *  Albert
 *
 *  Created by Matthias Melcher on 07.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "sym2db.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


FILE *in, *out;

// 0x0036340C ArProbEncodeLu1
// - begin data

// 0x00382324 TReservedContiguousMemory::New(char *) static
// - begin functions

// 0x003AF000 gROMMagicPointerTable
// - data

// (0x003afdc4 SYMcfunction  SYM... symbols)

// 0x0063AD05 MP0000  // end of symbols, data? at odd addresse?

// 0x0067FA44 Rupbitmap // pointers and data

// 0x00681CA0 RSSYM_2A  // two words

// 0x006853DC gEnum80DaysMonths

// 0x0071FC4C ROM$$Size

// 0x01A00000 FIQHandler  // jump table at run time

// 0x01C10858 TVoyagerPlatform::StartKeyboardDriver(unsigned char)  // end of jump table

// 0x04006400 gParamBlockFromImagePhysical  // ??

// 0x0C008400 gParamBlockFromImage  // globals??

// 0x0C107E14 _end  // last symbol, Rom Extension (REX) follows

// 0x007ec7fc end of REX



/*
 
 Another attempt at the Newton 2100 memory map (ROM 717006):
 
  0x0000.0000: ROM
      00.0000: CODE reset
      00.0004: CODE undefined instruction
      00.0008: CODE software int
      00.000c: CODE prefetch abort
      00.0010: CODE data abort
      00.0018: CODE IRQ
      00.001c: CODE FIQ
      00.0020: Data
      00.2000: Begin of Patch tables (list of branch instructions) -> 01.285c
      01.3000: Begin of Branch tables (branch instr.) -> 01.5e0c
      01.6000: Begin of ROM page patch table
      01.8400: CODE (Low level calls, intermixed with a few tables!)
      02.1438: CODE C++
      36.340c: Read Only Data, probaably HWR patterns
      38.2324: CODE C++, ends in some low level functions
      3a.f000: Magic Pointer Table (873 entries)
      3a.fda8: Symbols
      67.fa40: Binary Objects
      71.fc4c: ROM code end, beginning of REX (ROM Extension, may contain code)
      7e.c7fc: REX code end, rest is empty
  0x0080.0000: End of ROM
  0x01a0.0000: Jump Table in RAM
  0x01c1.085c: End of Jump Table
  0x0400.6400: Param Block From Image Physical ??
  0x0c00.0000: ??
  0x0c00.8400: system variables start RAM
  0x0c10.7e14: end system variables RAM
  0x0f00.0000: DMA registers
  0x0f18.0000: start of GPIO registers
  0x0f20.0000: other registers (reboot reason, etc.)
  0x2000.0000: Flash RAM mapped to various channels (8 bit, 16 bit)
  0x4000.0000: System RAM 32 bit wide
  0x6000.0000: Current Application (Newton Script)
  0xe000.0000: screen memory
  0xe001.2c00: End of screen memory (320 x 280 x 4 bit)

  0x0036.340C: 
 
SWI's      
 00      GetPortSWI
 01      PortSendSWI
 02      PortReceiveSWI
 03      EnterAtomicSWI
 04      ExitAtomicSWI
 05      GenericSWI
 06      GenerateMessageIRQ
 07      PurgeMMUTBLEntry
 08      FlushMMU
 09      FlushCache
 0a      GetCPUVersion
 0b      SemaphoreOpGlue
 0c      SetDomainRegister
 0d      SMemSetBufferSWI
 0e      SMemGetSizeSWI
 0f      SMemCopyToSharedSWI
 10      SMemCopyFromSharedSWI
 11      SMemMsgSetTimerParmsSWI
 12      SMemMsgSetMsgAvailPortSWI
 13      SMemMsgGetSenderTaskIdSWI
 14      SMemMsgSetUserRefConSWI
 15      SMemMsgGetUserRefConSWI
 16      SMemMsgCheckForDoneSWI
 17      SMemMsgMsgDoneSWI
 18      TurnOffCache
 19      TurnOnCache
 1b      MonitorDispatchSWI
 1c      MonitorExitSWI
 1d      MonitorThrowSWI
 1e      EnterFIQAtomicSWI
 1f      ExitFIQAtomicSWI
 20      MonitorFlushSWI
 21      PortResetFilterSWI
 22      DoSchedulerSWI
*/ 

unsigned char ROM[0x00800000];

class Klass {
  static Klass **pKlass;
  static int pn, pN;
  char *pName;
public:
  Klass(const char *name) {
    pName = strdup(name);
  }
  ~Klass() {
    free(pName);
  }
  static void add(const char *name) {
    int i;
    for (i=0; i<pn; i++) {
      if (strcmp(pKlass[i]->pName, name)==0)
        return;
    }
    if (pn==pN) {
      pN += 100;
      pKlass = (Klass**)realloc(pKlass, sizeof(Klass**)*pN);
    }
    pKlass[pn++] = new Klass(name);
  }
  static void write(FILE *out) {
    int i;
    for (i=0; i<pn; i++) {
      fprintf(out, "begin class\n");
      fprintf(out, "  sym %s\n", pKlass[i]->pName);
      fprintf(out, "  base FIXME\n");
      fprintf(out, "  size FIXME\n");
      fprintf(out, "  datac FIXME\n");
      fprintf(out, "end\n\n");
    }
  }
};

Klass **Klass::pKlass = 0;
int Klass::pn = 0, Klass::pN = 0;


/**
 * Return the 4-byte word in rom
 */
unsigned int rom_w(unsigned int addr)
{
  if (addr>=0x00800000)
    return 0;
  else
    return (ROM[addr]<<24)|(ROM[addr+1]<<16)|(ROM[addr+2]<<8)|ROM[addr+3];
}

/**
 * Convert a string into its individual arguments.
 * There is no leading bracket!
 */
void print_attributes(const char *args) 
{
  const char *ket = strrchr(args, ')');
  if (ket) {
    if (strstr(args, "static"))
      fprintf(out, "  static\n");
    if (strstr(args, "const"))
      fprintf(out, "  const\n");
  }
}

/**
 * Convert a string into its individual arguments.
 * There is no leading bracket!
 */
void print_args(const char *line) 
{
  int bc = 1;
  int argc = 0, i;
  char *args = strdup(line);
  char *argv[20], *s = args;
  // count the arguments and set seperating NUL's
  for (;;) {
    if (*s==')') break;
    if (*s==0) {
      printf("Unexpected end of argument list in '(%s'\n", line);
      break;
    }
    if (isspace(*s)) s++;
    argv[argc] = s;
    argc++;
    for (;;) {
      if (*s==0) {
        break;
      } else if (*s=='(') {
        bc++;
      } else if (*s==')') {
        bc--;
        if (bc==0) {
          *s = 0;
          s++;
          break;
        }
      } else if (*s==',') {
        if (bc==1) {
          *s = 0;
          s++;
          break;
        }
      }
      s++;
    }
  }
  // ignore single 'void' argument
  if (argc==1 && strcmp(argv[0], "void")==0)
    argc = 0;
  // now print all the args
  fprintf(out, "  argc %d\n", argc);
  for (i=0; i<argc; i++) {
    fprintf(out, "  arg %d %s\n", i, argv[i]);
  }
  free(args);
}


/**
 * Convert an address/symbol pair
 */
void convert(const char *s, unsigned int next) 
{
  unsigned int addr;
  char sym[512];
  int n = sscanf(s, "0x%08x", &addr);
  if (n!=1) {
    printf("Failed to convert line '%s'\n", s);
  }
  strcpy(sym, s+11);
  int l = strlen(sym);
  if (sym[l-1]=='\n')
    sym[l-1] = 0;
  /*
  00.0000: CODE reset
  00.0004: CODE undefined instruction
  00.0008: CODE software int
  00.000c: CODE prefetch abort
  00.0010: CODE data abort
  00.0018: CODE IRQ
  00.001c: CODE FIQ
  00.0020: Data
  00.2000: Begin of Patch tables (list of branch instructions) -> 01.285c
  01.3000: Begin of Branch tables (branch instr.) -> 01.5e0c
  01.6000: Begin of ROM page patch table
  01.8400: CODE (Low level calls, intermixed with a few tables!)
  02.1438: CODE C++
  36.340c: Read Only Data
  38.2324: CODE C++, ends in some low level functions
  3a.f000: Magic Pointer Table (873 entries)
  3a.fda8: Symbols
  67.fa40: Binary Objects
  71.fc4c: ROM code end, beginning of REX (ROM Extension, may contain code)
  7e.c7fc: REX code end, rest is empty
  */
  // lets interprete the different possible symbols that we can recognize at this point
  char *bra = strchr(sym, '(');
  char *ket = strrchr(sym, ')');
  char *cpp = strstr(sym, "::");
  if (addr>=0x00800000) {
    fprintf(out, "begin RAM\n");
    fprintf(out, "  at 0x%08X\n", addr);
    fprintf(out, "  next 0x%08X\n", next);
    fprintf(out, "  sym %s\n", sym);
    fprintf(out, "end\n\n");
  } else if (cpp && bra) { // this is a C++ class function
    *cpp = 0;
    *bra = 0;
    char ret = 1;
    fprintf(out, "begin cpp_member_function\n");
    fprintf(out, "  at 0x%08X\n", addr);
    fprintf(out, "  next 0x%08X\n", next);
    fprintf(out, "  class %s\n", sym);
    Klass::add(sym);
    fprintf(out, "  sym %s\n", cpp+2);
    if (strcmp(sym, cpp+2)==0) { fprintf(out, "  ctor\n"); ret = 0; }
    if (cpp[2]=='~' && strcmp(sym, cpp+3)==0) { fprintf(out, "  dtor\n"); ret = 0; }
    if (ket) print_attributes(ket+1);
    print_args(bra+1);
    if (ret) fprintf(out, "  returns FIXME\n"); // unknown return type
    fprintf(out, "end\n\n");
  } else if (bra) { // this is a C++ function outside a class
    *bra = 0;
    fprintf(out, "begin cpp_function\n");
    fprintf(out, "  at 0x%08X\n", addr);
    fprintf(out, "  next 0x%08X\n", next);
    fprintf(out, "  sym %s\n", sym);
    if (ket) print_attributes(ket+1);
    print_args(bra+1);
    fprintf(out, "  returns FIXME\n"); // unknown return type
    fprintf(out, "end\n\n");
  } else if (rom_w(addr)==0xE1A0C00D) { // find "C" functions: mov r12,sp
    fprintf(out, "begin c_function\n");
    fprintf(out, "  at 0x%08X\n", addr);
    fprintf(out, "  next 0x%08X\n", next);
    fprintf(out, "  sym %s\n", sym);
    fprintf(out, "  argc FIXME\n"); // unknown argument count
    fprintf(out, "  returns FIXME\n"); // unknown return type
    fprintf(out, "end\n\n");
  } else if (strncmp(sym, "SYM", 3)==0) {
    fprintf(out, "begin nsSymbol\n");
    fprintf(out, "  at 0x%08X\n", addr);
    fprintf(out, "  next 0x%08X\n", next);
    fprintf(out, "  sym %s\n", sym);
    fprintf(out, "end\n\n");
  } else if (sym[0]=='k' && isupper(sym[1])) {
    fprintf(out, "begin const_data\n");
    fprintf(out, "  at 0x%08X\n", addr);
    fprintf(out, "  next 0x%08X\n", next);
    fprintf(out, "  sym %s\n", sym);
    fprintf(out, "end\n\n");
  } else if (sym[0]=='g' && isupper(sym[1])) {
    fprintf(out, "begin global_data\n");
    fprintf(out, "  at 0x%08X\n", addr);
    fprintf(out, "  next 0x%08X\n", next);
    fprintf(out, "  sym %s\n", sym);
    fprintf(out, "end\n\n");
  } else if ((addr>=0x0036340C && addr<0x00382324)   // unknown data
          || (addr>=0x003AF000 && addr<=0x0071FC4C)  // Newton Script and more
          || (addr>=0x00000020 && addr<=0x000184d0)) // mostly jump tables in this segment
  {
    fprintf(out, "begin data\n");
    fprintf(out, "  at 0x%08X\n", addr);
    fprintf(out, "  next 0x%08X\n", next);
    fprintf(out, "  sym %s\n", sym);
    fprintf(out, "end\n\n");
  } else { // default:
    fprintf(out, "begin unknown\n");
    fprintf(out, "  at 0x%08X\n", addr);
    fprintf(out, "  next 0x%08X\n", next);
    fprintf(out, "  sym %s\n", sym);
    fprintf(out, "end\n\n");
  }
}


/**
 * Convert the symbols list into our new database format.
 */
int main(int argc, char **argv) 
{
  FILE *rom = fopen("/Users/matt/dev/Albert/data/717006", "rb");
  if (!rom) {
    puts("Can't read ROM!");
    return -1;
  }
  fread(ROM, 0x00800000, 1, rom);
  fclose(rom);
  
  in = fopen("/Users/matt/dev/Albert/data/717006.symbols", "rb");
  if (!in) {
    puts("Can't open Symbol file for reading.");
    return -1;
  }
  out = fopen("/Users/matt/dev/Albert/symbols.txt", "wb");
  if (!out) {
    puts("Can't open Database file for writing.");
    fclose(in);
    return -1;
  }
  fprintf(out, "\n");
  fprintf(out, "#import classes.txt\n");
  fprintf(out, "\n");
  
  unsigned int next = 0;
  char buf[1024], buf2[1024];
  fgets(buf, 1024, in);
  while (!feof(in)) {
    strcpy(buf2, buf);
    char *s = fgets(buf, 1024, in);
    if (s) 
      sscanf(s, "0x%08x", &next);
    else
      next = 0x0C107E14; // "_end"
    convert(buf2, next);
    if (!s) 
      break;
  }
  
  fclose(in);
  fclose(out);
  
  out = fopen("/Users/matt/dev/Albert/classes.txt", "wb");
  if (!out) {
    puts("Can't open class database file for writing.");
    return -1;
  }
  Klass::write(out);
  fclose(out);
  
  return 0;
}
