/*
 *  nsDecode.cpp
 *  Albert
 *
 *  Created by Matthias Melcher on 08.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

//#define VERBOSE

#include "db2src.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <map>


unsigned int decodeNSRef(FILE *newt, unsigned int i) {
  unsigned int val = rom_w(i);  
  if ( (val&0x00000003) == 0x00000000 ) { // Integer
    int v = (int)val;
    fprintf(newt, "\tNSInt\t%d\n", v/4, i);
  } else if ( (val&0x00000003) == 0x00000001 ) { // Pointer
    const char *sym = get_plain_symbol_at(val&0xfffffffe);
    if (sym)
      fprintf(newt, "\tNSPtr\t%s\t\t@ -> 0x%08X\n", sym, val&0xfffffffe);
    else if (rom_flags_is_set(val&0xfffffffe, flags_is_target))
      fprintf(newt, "\tNSPtr\tL%08X\n", val&0xfffffffe);
    else
      fprintf(newt, "\tNSPtr\t0x%08X\n", val&0xfffffffe);
  } else if ( (val&0x00000003) == 0x00000002 ) { // Special
    switch (val) {
      case 0x00000002: fprintf(newt, "\tNSNil\n"); break;
      case 0x0000001a: fprintf(newt, "\tNSTrue\n"); break;
      case 0x00055552: {
        //int n = strlen((const char*)ROM+i+8);
        fprintf(newt, "\tNSSymbol\t0x%08X, \"%s\"\n", rom_w(i+4), ROM+i+8); 
        // TODO: calculate those offsets!
        for (i+=8;;i++) { char c = ROM[i]; if (c==0) break; }
        for(i++;(i&3);i++) { }
        i-=4;
      } break;
      default: 
        if ( (val&0xfff0000f) == 0x0000000a ) { // Character
          unsigned int c = (val>>4)&0xffff;
          if (c>31 && c<127) 
            fprintf(newt, "\tNSChar\t0x%04X\t\t@ (NSRef UniChar = %c)\n", c, c);
          else
            fprintf(newt, "\tNSChar\t0x%04X\t\t@ (NSRef UniChar)\n", c);
        } else {
          fprintf(newt, "\t.word\t0x%08X\t\t@ 0x%08X (Unknown NSRef Special)\n", rom_w(i), i); 
        }
        break;
    }
  } else if ( (val&0x00000003) == 0x00000003 ) { // Magic Pointer
    unsigned int table = val>>14;
    unsigned int index = (val>>2)&0x00000fff;
    fprintf(newt, "\tNSMagic\t%d, %d\n", table, index, i, table, index);
  }
  return i;
}  


unsigned int decodeNSObj(FILE *newt, unsigned int i) {
  unsigned int val = rom_w(i);  
  unsigned int size = val>>8, j, n;
  if ((val&0x0000007f)==0x00000040) { // --- Binary Object
    char decoded[128] = "";
    char decode = 1;
    switch (rom_w(i+8)) {
      case 0x00055552: // Symbol
        break;
      case 0x003C13A5: // String
        fprintf(newt, "\t@ String: \"");
        for (j=i+12, n=i+size-2; j<n; j+=2) {
          unsigned short c = ROM[j+1] + (ROM[j]<<8);
          if (c>=32 && c<127) 
            fputc(c, newt);
          else
            fprintf(newt, "\\0x%04x", c);
        }
        fprintf(newt, "\"\n");
        break;
      case 0x003B6859: // Real
        sprintf(decoded, "\t\t\t@ Real: %f", rom_real(i+12));
        //fprintf(newt, "\tNSObjReal\t%g\n", rom_real(i+12)); i+=16;
        //decode = 0;
        break;
      case 0x003BD889: // Fixed
        sprintf(decoded, "\t\t\t@ Fixed: %g", rom_fixed(i+12));
        break;
      default:
        strcpy(decoded, "\t\t\t@ (undecoded)");
    }
    if (decode) {
      if ((val&0x000000ff)==0x00000040) {
        fprintf(newt, "\tNSObjBin\t%d%s\n", size-12, decoded); i+=4;
      } else if ((val&0x000000ff)==0x000000C0) {
        fprintf(newt, "\tNSObjXBin\t%d%s\n", size-12, decoded); i+=4;
      }
    }
  } else if ((val&0x0000007f)==0x00000041) { // --- Array
    if ((val&0x000000ff)==0x00000041) {
      fprintf(newt, "\tNSObjArray\t%d\n", (size/4)-3); i+=4;
    } else if ((val&0x000000ff)==0x000000C1) {
      fprintf(newt, "\tNSObjXArray\t%d\n", (size/4)-3); i+=4;
    }
  } else if ((val&0x0000007f)==0x00000043) { // --- Frame
    char decoded[128] = "";
    switch (rom_w(i+8)) {
      case 0x003D45E5: // Sound
        sprintf(decoded, "\t\t\t@ Sound");
        break;
      default:
        //strcpy(decoded, "\t\t\t@ (undecoded)");
        break;
    }
    if ((val&0x000000ff)==0x00000043) {
      fprintf(newt, "\tNSObjFrame\t%d %s\n", (size/4)-3, decoded); i+=4;
    } else if ((val&0x000000ff)==0x000000C3) {
      fprintf(newt, "\tNSObjXFrame\t%d %s\n", (size/4)-3, decoded); i+=4;
    }
  } else {
    fprintf(newt, "\t.word\t0x%08X\t\t@ 0x%08X (unknown NewtonScript object)\n", val, i);
  }
  return i;
}


