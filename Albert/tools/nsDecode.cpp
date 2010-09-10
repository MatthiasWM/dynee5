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

#include "EasyBMP/EasyBMP.h"


char extractBitmap(unsigned int addr);
char extractColorBitmap(unsigned int bits);
char extractPicture(unsigned int addr);


int toInt(unsigned int val) {
  int v = (int)val;
  return (v/4);
}

const char *getSymbol(unsigned int i) {
  unsigned int s = rom_w(i);
  if ((s&0x00000003)!=0x00000001)
    return 0;
  s = s & 0xfffffffc;
  unsigned int id = rom_w(s+8);
  if (id!=0x00055552)
    return 0;
  return (const char*)ROM+s+16;
}

unsigned int decodeNSRef(FILE *newt, unsigned int i) {
  unsigned int val = rom_w(i);  
  if ( (val&0x00000003) == 0x00000000 ) { // Integer
    AsmPrintf(newt, "\tNSInt\t%d\n", toInt(val), i);
  } else if ( (val&0x00000003) == 0x00000001 ) { // Pointer
    const char *sym = get_plain_symbol_at(val&0xfffffffe);
    if (sym)
      AsmPrintf(newt, "\tNSPtr\t%s\t\t@ -> 0x%08X\n", sym, val&0xfffffffe);
    else if (rom_flags_is_set(val&0xfffffffe, flags_is_target))
      AsmPrintf(newt, "\tNSPtr\tL%08X\n", val&0xfffffffe);
    else
      AsmPrintf(newt, "\tNSPtr\t0x%08X\n", val&0xfffffffe);
  } else if ( (val&0x00000003) == 0x00000002 ) { // Special
    switch (val) {
      case 0x00000002: AsmPrintf(newt, "\tNSNil\n"); break;
      case 0x0000001a: AsmPrintf(newt, "\tNSTrue\n"); break;
      case 0x00055552: {
        //int n = strlen((const char*)ROM+i+8);
        AsmPrintf(newt, "\tNSSymbol\t0x%08X, \"%s\"\n", rom_w(i+4), ROM+i+8); 
        // TODO: calculate those offsets!
        for (i+=8;;i++) { char c = ROM[i]; if (c==0) break; }
        for(i++;(i&3);i++) { }
        i-=4;
      } break;
      default: 
        if ( (val&0xfff0000f) == 0x0000000a ) { // Character
          unsigned int c = (val>>4)&0xffff;
          if (c>31 && c<127) 
            AsmPrintf(newt, "\tNSChar\t0x%04X\t\t@ (NSRef UniChar = %c)\n", c, c);
          else
            AsmPrintf(newt, "\tNSChar\t0x%04X\t\t@ (NSRef UniChar)\n", c);
        } else {
          AsmPrintf(newt, "\t.word\t0x%08X\t\t@ 0x%08X (Unknown NSRef Special)\n", rom_w(i), i); 
        }
        break;
    }
  } else if ( (val&0x00000003) == 0x00000003 ) { // Magic Pointer
    unsigned int table = val>>14;
    unsigned int index = (val>>2)&0x00000fff;
    AsmPrintf(newt, "\tNSMagic\t%d, %d\n", table, index, i, table, index);
  }
  return i;
}  


unsigned int decodeNSObj(FILE *newt, unsigned int i) {
  unsigned int val = rom_w(i);  
  unsigned int size = val>>8, j, n;
  if ((val&0x0000007f)==0x00000040) { // --- Binary Object
    char decoded[1024] = "";
    char decode = 1;
    const char *sym = getSymbol(i+8);
    if (rom_w(i+8)==0x00055552) {
    } else if (sym==0) {
      strcpy(decoded, "\t\t\t@ (no symbol)");
    } else if (strcmp(sym, "string")==0 || strcmp(sym, "string.noData")==0) {
      AsmPrintf(newt, "\t@ String: \"");
      for (j=i+12, n=i+size-2; j<n; j+=2) {
        unsigned short c = ROM[j+1] + (ROM[j]<<8);
        if (c>=32 && c<127) 
          AsmPrintf(newt, "%c", c);
        else
          AsmPrintf(newt, "\\0x%04x", c);
      }
      AsmPrintf(newt, "\"\n");
    } else if (strcmp(sym, "Real")==0) {
      sprintf(decoded, "\t\t\t@ Real: %f", rom_real(i+12));
      //AsmPrintf(newt, "\tNSObjReal\t%g\n", rom_real(i+12)); i+=16;
      //decode = 0;
    } else if (strcmp(sym, "fixed")==0) {
      sprintf(decoded, "\t\t\t@ fixed: %g", rom_fixed(i+12));
    } else if (strcmp(sym, "bits")==0 ) {
      strcpy(decoded, "\t\t\t@ B&W Bitmap");
      //extractBitmap(i);
    } else if (strcmp(sym, "mask")==0) {
      strcpy(decoded, "\t\t\t@ Transparency Mask");
      //extractBitmap(i);
    } else if (strcmp(sym, "cbits")==0) {
      strcpy(decoded, "\t\t\t@ Color Bitmap");
      //extractColorBitmap(i);
    } else if (strcmp(sym, "picture")==0) {
      strcpy(decoded, "\t\t\t@ Picture");
      //extractPicture(i);
    } else {
      sprintf(decoded, "\t\t\t@ binary data: '%s", sym);
      //@ unknown binary of type 'AirusA
      //@ unknown binary of type 'Comp
      //@ unknown binary of type 'DTEH
      //@ unknown binary of type 'DTEM
      //@ unknown binary of type 'Intl
      //@ unknown binary of type 'PPDB
      //@ unknown binary of type 'Sort
      //@ unknown binary of type 'TDTMFCodec
      //@ unknown binary of type 'Trigram
      //@ unknown binary of type 'Trigrams
      //@ unknown binary of type 'UniC
      //@ unknown binary of type 'boundsrect
      //@ unknown binary of type 'deskey
      //@ unknown binary of type  fixed
      //@ unknown binary of type 'instructions
      //@ unknown binary of type 'kchr
      //@ unknown binary of type 'letterimages
      //@ unknown binary of type  mask
      //@ unknown binary of type 'pattern         (one occurence!)
      //@ unknown binary of type 'polygonshape
      //@ unknown binary of type 'rectangle       (one occurence!)
      //@ unknown binary of type 'roundrectangle
      //@ unknown binary of type 'samples
      //@ unknown binary of type 'sfnt
      //@ unknown binary of type 'string.noData
      //@ unknown binary of type 'table
    }
    if (decode) {
      if ((val&0x000000ff)==0x00000040) {
        AsmPrintf(newt, "\tNSObjBin\t%d%s\n", size-12, decoded); i+=4;
      } else if ((val&0x000000ff)==0x000000C0) {
        AsmPrintf(newt, "\tNSObjXBin\t%d%s\n", size-12, decoded); i+=4;
      }
    }
  } else if ((val&0x0000007f)==0x00000041) { // --- Array
    if ((val&0x000000ff)==0x00000041) {
      AsmPrintf(newt, "\tNSObjArray\t%d\n", (size/4)-3); i+=4;
    } else if ((val&0x000000ff)==0x000000C1) {
      AsmPrintf(newt, "\tNSObjXArray\t%d\n", (size/4)-3); i+=4;
    }
  } else if ((val&0x0000007f)==0x00000043) { // --- Frame
    unsigned int ref = rom_w(i+8);
    char decoded[128] = "";
    if (ref==0x003D45E5) // Sound
      sprintf(decoded, "\t\t\t@ Sound");
    else if (ref==0x003D5CB1) // Bitmap header
      sprintf(decoded, "\t\t\t@ Bitmap('bits, 'bounds)");
    if ((val&0x000000ff)==0x00000043) {
      AsmPrintf(newt, "\tNSObjFrame\t%d %s\n", (size/4)-3, decoded); i+=4;
    } else if ((val&0x000000ff)==0x000000C3) {
      AsmPrintf(newt, "\tNSObjXFrame\t%d %s\n", (size/4)-3, decoded); i+=4;
    }
  } else {
    AsmPrintf(newt, "\t.word\t0x%08X\t\t@ 0x%08X (unknown NewtonScript object)\n", val, i);
  }
  return i;
}


char extractBitmap(unsigned int bits) 
{
  // create the BMP
  int x, wdt = ((ROM[bits+26]<<8)|(ROM[bits+27])) - ((ROM[bits+22]<<8)|(ROM[bits+23]));
  int y, hgt = ((ROM[bits+24]<<8)|(ROM[bits+25])) - ((ROM[bits+20]<<8)|(ROM[bits+21]));
  int bpr = ROM[bits+17];
  BMP bmp;
  bmp.SetSize(wdt, hgt);
  bmp.SetBitDepth(24);
  // copy all bits over
  RGBApixel black = { 0, 0, 0, 0 };
  RGBApixel white = { 255, 255, 255, 0 };
  for (y=0; y<hgt; y++) {
    unsigned int row = bits + 28 + y*bpr;
    for (x=0; x<wdt; x++) {
      unsigned char v = ROM[row+x/8];
      char set = v & (128>>(x&7));
      bmp.SetPixel(x, y, set?black:white);
    }
  }
  // write to file
  char buf[2048];
  sprintf(buf, "/Users/matt/dev/Albert/NewtonOS/images/IMG_%08X.bmp", bits);
  bmp.WriteToFile(buf);
  return 1;
}


char extractColorBitmap(unsigned int bits) 
{
  static RGBApixel lut[] = {
    { 0x00, 0x00, 0x00 },
    { 0x11, 0x11, 0x11 },
    { 0x22, 0x22, 0x22 },
    { 0x33, 0x33, 0x33 },
    { 0x44, 0x44, 0x44 },
    { 0x55, 0x55, 0x55 },
    { 0x66, 0x66, 0x66 },
    { 0x77, 0x77, 0x77 },
    { 0x88, 0x88, 0x88 },
    { 0x99, 0x99, 0x99 },
    { 0xaa, 0xaa, 0xaa },
    { 0xbb, 0xbb, 0xbb },
    { 0xcc, 0xcc, 0xcc },
    { 0xdd, 0xdd, 0xdd },
    { 0xee, 0xee, 0xee },
    { 0xff, 0xff, 0xff },
  };
  // create the BMP
  int x, wdt = ((ROM[bits+26]<<8)|(ROM[bits+27])) - ((ROM[bits+22]<<8)|(ROM[bits+23]));
  int y, hgt = ((ROM[bits+24]<<8)|(ROM[bits+25])) - ((ROM[bits+20]<<8)|(ROM[bits+21]));
  int bpr = ROM[bits+17];
  BMP bmp;
  bmp.SetSize(wdt, hgt);
  bmp.SetBitDepth(24);
  // copy all bits over
  for (y=0; y<hgt; y++) {
    unsigned int row = bits + 28 + y*bpr;
    for (x=0; x<wdt; x++) {
      unsigned char v = ROM[row+x/2];
      bmp.SetPixel(x, y, lut[15-((v>>((x&1^1)*4))&0x0f)]);
    }
  }
  // write to file
  char buf[2048];
  sprintf(buf, "/Users/matt/dev/Albert/NewtonOS/images/IMG_%08X.bmp", bits);
  bmp.WriteToFile(buf);
  return 1;
}


unsigned int rom_s(unsigned int i) {
  return ((ROM[i]<<8)|(ROM[i+1]));
}

// TODO: I do not understand this format yet!
char extractPicture(unsigned int bits) 
{
  unsigned int i = bits;
  printf("Picture:\n");
  printf("  %d %d\n", rom_s(i+12), rom_s(i+14));
  printf("  %d %d\n", rom_s(i+16), rom_s(i+18));
  printf("  %d %d\n", rom_s(i+20), rom_s(i+22));
  printf("  %d %d\n", rom_s(i+24), rom_s(i+26));
  printf("  %d %d\n", rom_s(i+28), rom_s(i+30));
  printf("  %d %d\n", rom_s(i+32), rom_s(i+34));
  printf("  %d %d\n", rom_s(i+36), rom_s(i+38));
  printf("  %d %d\n", rom_s(i+40), rom_s(i+42));
  
  // create the BMP
  int x, wdt = rom_s(i+20)-rom_s(i+16);
  int y, hgt = rom_s(i+18)-rom_s(i+14);
  int bpr = (wdt+7)/8;//ROM[bits+17];
  BMP bmp;
  bmp.SetSize(wdt, hgt);
  bmp.SetBitDepth(24);
  // copy all bits over
  RGBApixel black = { 0, 0, 0, 0 };
  RGBApixel white = { 255, 255, 255, 0 };
  for (y=0; y<hgt; y++) {
    unsigned int row = bits + 28 + y*bpr;
    for (x=0; x<wdt; x++) {
      unsigned char v = ROM[row+x/8];
      char set = v & (128>>(x&7));
      bmp.SetPixel(x, y, set?black:white);
    }
  }
  // write to file
  char buf[2048];
  sprintf(buf, "/Users/matt/dev/Albert/NewtonOS/images/IMG_%08X.bmp", bits);
  bmp.WriteToFile(buf);
  return 1;
}



// TODO: extract pictures

