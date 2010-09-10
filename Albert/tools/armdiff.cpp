/*
 *  armdiff.cpp
 *  Albert
 *
 *  Created by Matthias Melcher on 08.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "db2src.h"

const char *db_path = "/Users/matt/dev/Albert/";
const char *src_path = "/Users/matt/dev/Albert/src/"; 
const char *c_path = "/Users/matt/dev/Albert/src/"; 
const char *cpp_path = "/Users/matt/dev/Albert/src/"; 
const char *os_path = "/Users/matt/dev/Albert/NewtonOS/"; 

unsigned int ROM_flags[0x00200000];
unsigned int ROM1[0x00200000];
unsigned int ROM2[0x00200000];


unsigned int ARMImm(unsigned int a)
{
  unsigned int shift = (a&0x00000f00)>>7;
  unsigned int value = (a&0x000000ff);
  return (value>>shift) | (value<<(32-shift));
}

int ARMCmp(unsigned int addr) 
{
  if (ROM1[addr]==ROM2[addr]) 
    return 1;
  if ( (ROM_flags[addr] & 0x000000ff) == 1) { // ARM code
    unsigned int a = htonl(ROM1[addr]), b = htonl(ROM2[addr]);
    if (   ((a&0x0e000000) == 0x02000000 )      // data processing with immediate?
        && ((a&0xfffff000) == (b&0xfffff000))   // same parameters?
        && (ARMImm(a) == ARMImm(b))
        ) {
      return 1;
    }
  }
  return 0;
}

int main(int argc, char **argv) 
{
  /*
  if (argc!=4) {
    fprintf(stderr, "Usage: %s ROM1File ROM2File ROMFlagsFile\n");
    return -1;
  }
  */
  const char *rom1FileName = "/Users/matt/dev/Albert/NewtonOS/ROM";
  const char *rom2FileName = "/Users/matt/dev/Albert/data/717006";
  const char *romFlagsFileName = "/Users/matt/dev/Albert/data/flags";
  
  // ---- read the first ROM
  //printf("Reading ROM 1 from %s\n", rom1FileName);
  FILE *rom1 = fopen(rom1FileName, "rb");
  if (!rom1) {
    fprintf(stderr, "Can't read ROM 1!\n");
    return -1;
  }
  if (fread(ROM1, sizeof(int), 0x00200000, rom1)!=0x00200000) {
    fprintf(stderr, "Invalid size ROM 1!\n");
    return -1;
  }
  fclose(rom1);
  
  // ---- read the second ROM
  //printf("Reading ROM 2 from %s\n", rom2FileName);
  FILE *rom2 = fopen(rom2FileName, "rb");
  if (!rom2) {
    fprintf(stderr, "Can't read ROM 2!\n");
    return -1;
  }
  if (fread(ROM2, sizeof(int), 0x00200000, rom2)!=0x00200000) {
    fprintf(stderr, "Invalid size ROM 2!\n");
    return -1;
  }
  fclose(rom2);
  
  // ---- read the flags, so we know what's ARM code
  //printf("Reading ROM Flags from %s\n", romFlagsFileName);
  FILE *rom_flags = fopen(romFlagsFileName, "rb");
  if (!rom_flags) {
    fprintf(stderr, "Can't read ROM flags!\n");
    return -1;
  }
  fread(ROM_flags, 0x00200000, sizeof(int), rom_flags);
  fclose(rom_flags);
  
  unsigned int i, n=0, id=1;
  
  for (i=0; i<0x00200000; i++) {
    if (ROM1[i]!=ROM2[i]) {
      id = 0;
      if (!ARMCmp(i)) {
        if (n<10)
          printf("ROM files differ at %d (0x%08X) 0x%08X!=0x%08X\n", i*4, i*4, htonl(ROM1[i]), htonl(ROM2[i]));
        n++;
      }
    }
  }

  if (n) {
    if (n>10)
      printf("...and %d more positions.\n", n-10);
    printf("****** ROM files differ! ******\n");
    return 1;
  } else {
    if (id)
      printf("ROM files are identical. Perfect!\n");
    else
      printf("ROM files are equal, but not identical. That's good.\n");
    return 0;
  }
}

