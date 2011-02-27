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


char writeBitmap(unsigned int addr);
char extractColorBitmap(unsigned int bits);
char writePICT(unsigned int addr, unsigned int size);

unsigned int extractBitmap(FILE *newt, unsigned int addr, int n);
unsigned int extractBytecode(FILE *newt, unsigned int addr, int n);
unsigned int extractUnicodeString(FILE *newt, unsigned int i, unsigned int size);

unsigned int currentObj = 0;
unsigned int currentObjType = 0;
unsigned int currentFrameMap = 0;

const unsigned int NS_OBJ_NONE = 0;
const unsigned int NS_OBJ_FRAME = 1;
const unsigned int NS_OBJ_ARRAY = 2;


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

const char *getSafeSymbol(unsigned int i) {
  static char buf[64];
  const char *sym = getSymbol(i);
  if (sym) {
    strcpy(buf, sym);
    if (strcmp(buf, ">=")==0) strcpy(buf, "GreaterOrEqual");
    else if (strcmp(buf, "<=")==0) strcpy(buf, "LessOrEqual");
    else if (strcmp(buf, "=")==0) strcpy(buf, "Equal");
    else if (strcmp(buf, "<>")==0) strcpy(buf, "NotEqual");
    else if (strcmp(buf, "<")==0) strcpy(buf, "Less");
    else if (strcmp(buf, ">")==0) strcpy(buf, "Greater");
    else if (strcmp(buf, "<<")==0) strcpy(buf, "ShiftLeft");
    else if (strcmp(buf, ">>")==0) strcpy(buf, "ShiftRight");
    else if (strcmp(buf, "+")==0) strcpy(buf, "Plus");
    else if (strcmp(buf, "-")==0) strcpy(buf, "Minus");
    else if (strcmp(buf, "*")==0) strcpy(buf, "Multiply");
    else if (strcmp(buf, "/")==0) strcpy(buf, "Divide");
    char *dot = strchr(buf, '.');
    if (dot) *dot = '_';
  } else {
    strcpy(buf, "NULL");
  }
  return buf;
}

unsigned int decodeNSRef(FILE *newt, unsigned int i) {
  char extraInfo[200];
  extraInfo[0] = 0;
  unsigned int val = rom_w(i);
  
  unsigned int wOff = (i-currentObj)/4;
  switch (currentObjType) {
    case NS_OBJ_ARRAY:
      if (wOff==2) strcpy(extraInfo, " [ArrayClass]");
      if (wOff>=3) sprintf(extraInfo, " [%d]", wOff-3);
      break;
    case NS_OBJ_FRAME: {
      const char *sym = 0L;
      if (wOff==2) { strcpy(extraInfo, " {map}"); break; }
      // TODO: we can't handle supermaps yet. This pointer would point to the supermap...
      if (rom_w(currentFrameMap+12)==0x00000002) 
        sym = getSymbol(currentFrameMap+4*(wOff+1));
      if (sym) sprintf(extraInfo, " [%s]", sym);
      break;
    }
    default:
      break;
  }
  
  if ( (val&0x00000003) == 0x00000000 ) { // Integer
    if (rom_w(i-4)==0x00000132 && strcmp(extraInfo, " [funcPtr]")==0 && get_plain_symbol_at(4*toInt(val))) {
      if (4*toInt(val) >= 0x00800000)
        AsmPrintf(newt, "\tNSNativePtr\tVEC_%s\t@%s\n", get_plain_symbol_at(4*toInt(val)), extraInfo);
      else
        AsmPrintf(newt, "\tNSNativePtr\t%s\t@%s\n", get_plain_symbol_at(4*toInt(val)), extraInfo);
    } else {
      AsmPrintf(newt, "\tNSInt\t%d\t@%s\n", toInt(val), extraInfo);
    }
  } else if ( (val&0x00000003) == 0x00000001 ) { // Pointer
    const char *sym = get_plain_symbol_at(val&0xfffffffe);
    if (sym)
      AsmPrintf(newt, "\tNSPtr\t%s\t\t@%s -> 0x%08X\n", sym, extraInfo, val&0xfffffffe);
    else if (rom_flags_is_set(val&0xfffffffe, flags_is_target))
      AsmPrintf(newt, "\tNSPtr\tL%08X\t@%s\n", val&0xfffffffe, extraInfo);
    else
      AsmPrintf(newt, "\tNSPtr\t0x%08X\t@%s\n", val&0xfffffffe, extraInfo);
  } else if ( (val&0x00000003) == 0x00000002 ) { // Special
    switch (val) {
      case 0x00000002: AsmPrintf(newt, "\tNSNil\t@%s\n", extraInfo); break;
      case 0x0000001a: AsmPrintf(newt, "\tNSTrue\t@%s\n", extraInfo); break;
      case 0x00000032: AsmPrintf(newt, "\tNSFunc\t@%s\n", extraInfo); break;
      case 0x00000132: AsmPrintf(newt, "\tNSNativeFunc\t@%s\n", extraInfo); break;
      case 0x00055552: {
        //int n = strlen((const char*)ROM+i+8);
        AsmPrintf(newt, "\tNSSymbol\t0x%08X, \"%s\"\n", rom_w(i+4), ROM+i+8); 
        // TODO: calculate those offsets!
        for (i+=8;;i++) { char c = ROM[i]; if (c==0) break; }
        for(i++;(i&3);i++) { }
        i-=4;
      } break;
      default: 
        //if ( (val&0xfff0000f) == 0x0000000a ) {
        if ( (val&0xfff0000f) == 0x00000006 ) { // Character
          // TODO: this is a unicode character!
          unsigned int c = (val>>4)&0xffff;
          if (c>31 && c<127) 
            AsmPrintf(newt, "\tNSChar\t0x%04X\t\t@%s (NSRef UniChar = %c)\n", c, extraInfo, c);
          else
            AsmPrintf(newt, "\tNSChar\t0x%04X\t\t@%s (NSRef UniChar)\n", c, extraInfo);
        } else {
          AsmPrintf(newt, "\t.word\t0x%08X\t\t@%s 0x%08X (unknown NSRef special)\n", rom_w(i), extraInfo, i); 
        }
        break;
    }
  } else if ( (val&0x00000003) == 0x00000003 ) { // Magic Pointer
    unsigned int table = val>>14;
    unsigned int index = (val>>2)&0x00000fff;
    AsmPrintf(newt, "\tNSMagic\t%d, %d\t@%s\n", table, index, extraInfo);
  }
  return i;
}  


unsigned int decodeNSObj(FILE *newt, unsigned int i) {
  unsigned int val = rom_w(i);  
  unsigned int size = val>>8; //, j, n;
  currentObj = i;
  currentObjType = NS_OBJ_NONE;
  if ((val&0x0000007f)==0x00000040) { // --- Binary Object
    char decoded[1024] = "";
    char decode = 1;
    const char *sym = getSymbol(i+8);
    if (rom_w(i+8)==0x00055552) {
    } else if (sym==0) {
      strcpy(decoded, "\t\t\t@ (no symbol)");
    } else if (strcmp(sym, "string")==0 || strcmp(sym, "string.noData")==0) {
      i = extractUnicodeString(newt, i, size);
      decode = 0;
    } else if (strcmp(sym, "Real")==0) {
      sprintf(decoded, "\t\t\t@ Real: %f", rom_real(i+12));
      if ((val&0x000000ff)==0x00000040) {
        AsmPrintf(newt, "\tNSObjBin\t%d%s\n", size-12, decoded);
      } else if ((val&0x000000ff)==0x000000C0) {
        AsmPrintf(newt, "\tNSObjXBin\t%d%s\n", size-12, decoded);
      }
      decodeNSRef(newt, i+8);
      AsmPrintf(newt, "\tNSRealAsBin\t0x%08x%08x\n", rom_w(i+12), rom_w(i+16));
      //AsmPrintf(newt, "\tNSObjReal\t%g\n", rom_real(i+12)); i+=16;
      i+=16; decode = 0;
    } else if (strcmp(sym, "fixed")==0) {
      sprintf(decoded, "\t\t\t@ fixed: %g", rom_fixed(i+12));
    } else if (strcmp(sym, "bits")==0 ) {
      strcpy(decoded, "\t\t\t@ B&W Bitmap");
      if ((val&0x000000ff)==0x00000040) {
        AsmPrintf(newt, "\tNSObjBin\t%d%s\n", size-12, decoded);
      } else if ((val&0x000000ff)==0x000000C0) {
        AsmPrintf(newt, "\tNSObjXBin\t%d%s\n", size-12, decoded);
      }
      //writeBitmap(i);
      i = extractBitmap(newt, i, size);
      decode = 0;
    } else if (strcmp(sym, "mask")==0) {
      strcpy(decoded, "\t\t\t@ Transparency Mask");
      strcpy(decoded, "\t\t\t@ B&W Bitmap");
      if ((val&0x000000ff)==0x00000040) {
        AsmPrintf(newt, "\tNSObjBin\t%d%s\n", size-12, decoded);
      } else if ((val&0x000000ff)==0x000000C0) {
        AsmPrintf(newt, "\tNSObjXBin\t%d%s\n", size-12, decoded);
      }
      //writeBitmap(i);
      i = extractBitmap(newt, i, size);
      decode = 0;
    } else if (strcmp(sym, "cbits")==0) {
      strcpy(decoded, "\t\t\t@ Color Bitmap");
      //extractColorBitmap(i);
    } else if (strcmp(sym, "picture")==0) {
      strcpy(decoded, "\t\t\t@ Picture");
      //writePICT(i+12, size-12);
    } else if (strcmp(sym, "instructions")==0) {
      i = extractBytecode(newt, i, size-12);
      decode = 0;
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
    currentObjType = NS_OBJ_ARRAY;
    if ((val&0x000000ff)==0x00000041) {
      AsmPrintf(newt, "\tNSObjArray\t%d\n", (size/4)-3); i+=4;
    } else if ((val&0x000000ff)==0x000000C1) {
      AsmPrintf(newt, "\tNSObjXArray\t%d\n", (size/4)-3); i+=4;
    }
  } else if ((val&0x0000007f)==0x00000043) { // --- Frame
    currentObjType = NS_OBJ_FRAME;    
    unsigned int ref = currentFrameMap = (rom_w(i+8) & ~3);
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


unsigned int extractUnicodeString(FILE *newt, unsigned int i, unsigned int size)
{
  unsigned int j, n, val = rom_w(i);
  // first, print the string as a long comment
  AsmPrintf(newt, "\t@ String: \"");
  for (j=i+12, n=i+size-2; j<n; j+=2) {
    unsigned short c = ROM[j+1] + (ROM[j]<<8);
    if (c>=32 && c<127) 
      AsmPrintf(newt, "%c", c);
    else
      AsmPrintf(newt, "\\x%04x", c);
  }
  AsmPrintf(newt, "\"\n");
  // print binary object
  if ((val&0x000000ff)==0x00000040) {
    AsmPrintf(newt, "\tNSObjBin\t%d\n", size-12);
  } else if ((val&0x000000ff)==0x000000C0) {
    AsmPrintf(newt, "\tNSObjXBin\t%d\n", size-12);
  }
  decodeNSRef(newt, i+8);
  // now print the macro
  char curr = 0;
  for (j=i+12, n=i+size; j<n; j+=2) {
    unsigned short c = ROM[j+1] + (ROM[j]<<8);
    if (c>=32 && c<127 && c!='\\' && c!='"' && c!='@') {
      if (curr!=1) {
        AsmPrintf(newt, "\tNSUniString\t\"");
        curr = 1;
      }
      AsmPrintf(newt, "%c", c);
    } else {
      if (curr==1) AsmPrintf(newt, "\"\n");
      curr = 2;
      AsmPrintf(newt, "\tNSUniChar\t0x%04x\n", c);
    }
  }
  if (curr==1) AsmPrintf(newt, "\"\n");
  while (j&3) {
    AsmPrintf(newt, "\t.byte\t0x%02x\t@ unused\n", ROM[j]); // .align would be better!
    j++;
  }
  return j-4;
}


void writeFreqFuncBytecode(FILE *newt, unsigned int &addr, unsigned char cmd, int val)
{
  const char *txt = 0;
  switch (val) {
    case  0: txt = "Add"; break;
    case  1: txt = "Subtract"; break;
    case  2: txt = "Aref"; break;
    case  3: txt = "SetAref"; break;
    case  4: txt = "Equals"; break;
    case  5: txt = "Not"; break;
    case  6: txt = "NotEqual"; break;
    case  7: txt = "Multiply"; break;
    case  8: txt = "Divide"; break;
    case  9: txt = "Div"; break;
    case 10: txt = "LessThan"; break;
    case 11: txt = "GreaterThan"; break;
    case 12: txt = "GreaterOrEqual"; break;
    case 13: txt = "LessOrEqual"; break;
    case 14: txt = "BitAnd"; break;
    case 15: txt = "BitOr"; break;
    case 16: txt = "BitNot"; break;
    case 17: txt = "NewIterator"; break;
    case 18: txt = "Length"; break;
    case 19: txt = "Clone"; break;
    case 20: txt = "SetClass"; break;
    case 21: txt = "AddArraySlot"; break;
    case 22: txt = "Stringer"; break;
    case 23: txt = "HasPath"; break;
    case 24: txt = "ClassOf"; break;
  }
  if (txt) {
    AsmPrintf(newt, "\tBC%s\t\t@ %04o: BC%s\n", txt, cmd, txt);
  } else {
    if ((cmd&7)==7) 
      AsmPrintf(newt, "\t.byte\t0x%02x, 0x%02x, 0x%02x\t@ %04o %d: unknown bytecode\n", cmd, ROM[addr+1], ROM[addr+2], cmd, val);
    else
      AsmPrintf(newt, "\t.byte\t0x%02x\t@ %04o: unknown bytecode\n", cmd, cmd);
  }
  if ((cmd&7)==7) addr+=2;
}

void writeBytecode(FILE *newt, unsigned int &addr, unsigned char cmd, int val, const char *txt)
{
  if (val<7 && (cmd&7)==7) {
    AsmPrintf(newt, "\tBC%sL\t%d\t@ %04o: BC%sL %d\n", txt, val, cmd, txt, val);
  } else {
    AsmPrintf(newt, "\tBC%s\t%d\t@ %04o: BC%s %d\n", txt, val, cmd, txt, val);
  }
  if ((cmd&7)==7) addr+=2;
}

void writeBytecodeSigned(FILE *newt, unsigned int &addr, unsigned char cmd, int val, const char *txt)
{
  if (val>=0 && val<7 && (cmd&7)==7) {
    AsmPrintf(newt, "\tBC%sL\t%d\t@ %04o: BC%sL %d\n", txt, (signed short)val, cmd, txt, (signed short)val);
  } else {
    AsmPrintf(newt, "\tBC%s\t%d\t@ %04o: BC%s %d\n", txt, (signed short)val, cmd, txt, (signed short)val);
  }
  if ((cmd&7)==7) addr+=2;
}

unsigned int extractBytecode(FILE *newt, unsigned int addr, int n) 
{
  unsigned int last = addr+n+12;
  unsigned int next = (last+3)&~3; // word align
  unsigned int val = rom_w(addr);
  if ((val&0x000000ff)==0x00000040) {
    AsmPrintf(newt, "\tNSObjBin\t%d\t@ NewtonScript Bytecode\n", n);
  } else if ((val&0x000000ff)==0x000000C0) {
    AsmPrintf(newt, "\tNSObjXBin\t%d\t@ NewtonScript Bytecode (%d instruction bytes)\n", n, n, addr+12, next);
  }
  decodeNSRef(newt, addr+8);
  for ( addr+=12; addr<last; addr++) {
    unsigned char cmd = ROM[addr];
    unsigned int val = cmd&7;
    if (val==7) val = (ROM[addr+1]<<8)+ROM[addr+2];
    switch (cmd&0370) {
      case 00000: // single byte instructions
        switch (cmd) {
          case 0000: AsmPrintf(newt, "\tBCPop\t@ %04o: BCPop\n", cmd); break;
          case 0001: AsmPrintf(newt, "\tBCDup\t@ %04o: BCDup\n", cmd); break;
          case 0002: AsmPrintf(newt, "\tBCReturn\t@ %04o: BCReturn\n", cmd); break;
          case 0003: AsmPrintf(newt, "\tBCPushSelf\t@ %04o: BCPushSelf\n", cmd); break;
          case 0004: AsmPrintf(newt, "\tBCSetLexScope\t@ %04o: BCSetLexScope\n", cmd); break;
          case 0005: AsmPrintf(newt, "\tBCIterNext\t@ %04o: BCIterNext\n", cmd); break;
          case 0006: AsmPrintf(newt, "\tBCIterDone\t@ %04o: BCIterDone\n", cmd); break;
          case 0007: AsmPrintf(newt, "\tBCPopHandlers\t@ %04o: BCPopHandlers\n", cmd); break;
        }
        break;
      case 0030: writeBytecode(newt, addr, cmd, val, "Push"); break;
        // FIXME: the code below pushes a literal value as in NSRef (see above!)
      case 0040: writeBytecodeSigned(newt, addr, cmd, val, "PushConstant"); break;
      case 0050: writeBytecode(newt, addr, cmd, val, "Call"); break;
      case 0060: writeBytecode(newt, addr, cmd, val, "Invoke"); break;
      case 0070: writeBytecode(newt, addr, cmd, val, "Send"); break;
      case 0100: writeBytecode(newt, addr, cmd, val, "SendIfDefined"); break;
      case 0110: writeBytecode(newt, addr, cmd, val, "Resend"); break;
      case 0120: writeBytecode(newt, addr, cmd, val, "ResendIfDefined"); break;
      case 0130: writeBytecode(newt, addr, cmd, val, "Branch"); break;
      case 0140: writeBytecode(newt, addr, cmd, val, "BranchIfTrue"); break;
      case 0150: writeBytecode(newt, addr, cmd, val, "BranchIfFalse"); break;
      case 0160: writeBytecode(newt, addr, cmd, val, "FindVar"); break;
      case 0170: writeBytecode(newt, addr, cmd, val, "GetVar"); break;
      case 0200: writeBytecode(newt, addr, cmd, val, "MakeFrame"); break;
        // FIXME: special case 0xffff: pop integer and create empty array of that size
      case 0210: writeBytecode(newt, addr, cmd, val, "MakeArray"); break;
      case 0220: 
        switch (cmd) {
          case 0220: AsmPrintf(newt, "\tBCGetPath\t@ %04o: BCGetPath\n", cmd); break;
          case 0221: AsmPrintf(newt, "\tBCGetPathPush\t@ %04o: BCGetPathPush\n", cmd); break;
          default: AsmPrintf(newt, "\t.byte\t0x%02x\t@ %04o: unknown bytecode\n", cmd, cmd); break;
        }
        break;
      case 0230: 
        switch (cmd) {
          case 0230: AsmPrintf(newt, "\tBCSetPath\t@ %04o: BCSetPath\n", cmd); break;
          case 0231: AsmPrintf(newt, "\tBCSetPathPush\t@ %04o: BCSetPathPush\n", cmd); break;
          default: AsmPrintf(newt, "\t.byte\t0x%02x\t@ %04o: unknown bytecode\n", cmd, cmd); break;
        }
        break;
      case 0240: writeBytecode(newt, addr, cmd, val, "SetVar"); break;
      case 0250: writeBytecode(newt, addr, cmd, val, "FindAndSetVar"); break;
      case 0260: writeBytecode(newt, addr, cmd, val, "IncrVar"); break;
      case 0270: writeBytecode(newt, addr, cmd, val, "BranchIfLoopNotDone"); break;
      case 0300: writeFreqFuncBytecode(newt, addr, cmd, val); break;
      case 0310: writeBytecode(newt, addr, cmd, val, "NewHandlers"); break;
      default:
        AsmPrintf(newt, "\t.byte\t0x%02x\t@ %04o: unknown bytecode\n", cmd, cmd);
    }
  }
  for ( ; addr<next; addr++) {
    unsigned char cmd = ROM[addr];
    AsmPrintf(newt, "\t.byte\t0x%02x\t@ unused\n", cmd);
  }
  return next-4;
/*
 .ascii  "unary1\000\000"
 .ascii  "unary2\000\000"
 .ascii  "unary3\000\000"
 .ascii  "push\000\000\000\000"
 .ascii  "push-constant\000\000\000"
 .ascii  "call\000\000\000\000"
 .ascii  "invoke\000\000"
 .ascii  "send\000\000\000\000"
 .ascii  "send-if-defined\000"
 .ascii  "resend\000\000"
 .ascii  "resend-if-defined\000\000\000"
 .ascii  "branch\000\000"
 .ascii  "branch-t\000\000\000\000"
 .ascii  "branch-f\000\000\000\000"
 .ascii  "find-var\000\000\000\000"
 .ascii  "get-var\000"
 .ascii  "make-frame\000\000"
 .ascii  "make-array\000\000"
 .ascii  "get-path\000\000\000\000"
 .ascii  "set-path\000\000\000\000"
 .ascii  "set-var\000"
 .ascii  "set-find-var\000\000\000\000"
 .ascii  "incr-var\000\000\000\000"
 .ascii  "branch-if-loop-not-done\000"
 .ascii  "freq-func\000\000\000"
 .ascii  "new-handlers\000\000\000\000"
 .ascii  "pop\000"
 .ascii  "dup\000"
 .ascii  "return\000\000"
 .ascii  "push-self\000\000\000"
 .ascii  "set-lex-scope\000\000\000"
 .ascii  "iter-next\000\000\000"
 .ascii  "iter-done\000\000\000"
 .ascii  "pop-handlers\000\000\000\000"
*/
}

int writeShort(FILE *newt, unsigned int i, const char *comment)
{
  signed short x = ((ROM[i]<<8)|(ROM[i+1]));
  AsmPrintf(newt, "\t.short\t%d\t@ (0x%04x) %s\n", x, x, comment);
  return x;
}

unsigned int extractBitmap(FILE *newt, unsigned int bits, int size) 
{
  unsigned int end = bits+size;
  decodeNSRef(newt, bits+8);
  AsmPrintf(newt, "\t.int\t0x%08x\t@ \n", rom_w(bits+12));
  int x, bpr = writeShort(newt, bits+16, "bytes per row");
  writeShort(newt, bits+18, "...");
  writeShort(newt, bits+20, "top");
  writeShort(newt, bits+22, "left");
  writeShort(newt, bits+24, "bottom");
  writeShort(newt, bits+26, "right");
  
  for (bits+=28; bits<end; ) {
    AsmPrintf(newt, "\t.byte\t");
    for (x=0; x<bpr; x++, bits++) {
      AsmPrintf(newt, "0x%02x%s", ROM[bits], (x<bpr-1)?", ":"");
    }
    AsmPrintf(newt, "\t@ bitmap row\n");
  }
  return end - 4;
}

char writeBitmap(unsigned int bits) 
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

/*
 The format is quite complex. Just write the pure binary data and hope that an
 image reader can read them (3 out of 7 for OS X Finder)
 */ 
char writePICT(unsigned int bits, unsigned int size) 
{
  char buf[2048];
  sprintf(buf, "/Users/matt/dev/Albert/NewtonOS/images/IMG_%08X.pict", bits);
  FILE *f = fopen(buf, "wb");
  fwrite(ROM+bits, 1, size, f);
  fclose(f);
  return 1;
}

void extractStencil(unsigned int s)
{
  unsigned int x, wdt = rom_w(s)*8;
  unsigned int y, hgt = rom_w(s+4);
  unsigned int bits = rom_w(s+8);
  unsigned int bpr = rom_w(s);
  BMP bmp;
  bmp.SetSize(wdt, hgt);
  bmp.SetBitDepth(24);
  RGBApixel black = { 0, 0, 0, 0 };
  RGBApixel white = { 255, 255, 255, 0 };
  for (y=0; y<hgt; y++) {
    unsigned int row = bits + y*bpr;
    for (x=0; x<wdt; x++) {
      unsigned char v = ROM[row+x/8];
      char set = v & (1<<(x&7));
      bmp.SetPixel(x, y, set?black:white);
    }
  }
  // write to file
  char buf[2048];
  sprintf(buf, "/Users/matt/dev/Albert/NewtonOS/images/RAW_%08X.bmp", s);
  bmp.WriteToFile(buf);
}

void extractStencils(unsigned int base)
{
  unsigned int i;
  for (i=0; i<8; i++) {
    extractStencil(rom_w(base+4*i));
  }
}

void extractStencils()
{
  unsigned int stencils = 0x0036F0D0;
  extractStencils(rom_w(stencils+ 1*4));
  extractStencils(rom_w(stencils+ 2*4));
  extractStencils(rom_w(stencils+ 4*4));
  extractStencils(rom_w(stencils+ 8*4));
  extractStencils(rom_w(stencils+16*4));
  extractStencils(rom_w(stencils+32*4));
}

// TODO: extract pictures

/* A bytecode oddysee:

L0062AA24:
 NSObjXBin       16                      @ NewtonScript Bytecode (16 instruction bytes from 0x0062aa30 to 0x0062aa40)
 NSPtr   SYMinstructions                 @ -> 0x00535F6C
 BCPush  0                               @ 0030: BCPush 0       @ "StringP("
 BCGetVar        3                       @ 0173: BCGetVar 3     @ arg0
 BCPush  1                               @ 0031: BCPush 1       @ ")"
 BCPush  2                               @ 0032: BCPush 2       @ 'array
 BCMakeArray     3                       @ 0213: BCMakeArray 3  @
 BCStringer                              @ 0307: BCStringer     @ concatenate array into string
 BCPush  3                               @ 0033: BCPush 3       @ 'undocumented
 BCPush  4                               @ 0034: BCPush 4       @ 'badwickednaughtynoot
 BCCall  2                               @ 0052: BCCall 2       @ call badwickednaughtynoot(string, 'undocumented)
 BCPop                                   @ 0000: BCPop          @ throw away the result
 BCGetVar        3                       @ 0173: BCGetVar 3     @ arg0
 BCPush  5                               @ 0035: BCPush 5       @ 'isstring
 BCCall  1                               @ 0051: BCCall 1       @ call isstring(arg0)
 BCReturn                                @ 0002: BCReturn       @ return function result

L0062AA84:
 NSObjXFrame     5
 NSPtr   L0067EE70    @  -> array of symbols
 .word   0x00000032   @ 'class
 NSPtr   L0062AA24    @ 'instructions -> see above
 NSPtr   L0062AA40    @ 'literals
 NSNil                @ 'argrame
 NSInt   1            @ 'numargs
 
L0062AA40:
 NSObjXArray     6
 NSPtr   SYMliterals      @ six literals
 NSPtr   L0062AA64
 NSPtr   L0049793C
 NSPtr   SYMarray                        @ -> 0x003B0038
 NSPtr   SYMundocumented                 @ -> 0x0063801C
 NSPtr   SYMbadwickednaughtynoot         @ -> 0x003B160C
 NSPtr   SYMisstring                     @ -> 0x003B4F2C

 Great news:
 L00639580  is a frame that associates all pointers to NS functions with their 
            respective symbolic name! (1356 functions, see Rbuiltinfunctions)
 
 
 Special NSRef 0x00000032 -> class 'CodeBlock ?
 Special NSRef 0x00000132 -> native function?
    FGetSerialNumber: @ 0x0020171C: FGetSerialNumber
    ROM_GetSerialNumber is part of the magic pointer table
    ROM_GetSerialNumber:                            @ 0x0064A500: ROM_GetSerialNumber
      NSObjFrame      3
      NSPtr   gROMSoupData                    @ -> 0x003AFDA8
      .word   0x00000132                      @ 0x0064A50C (Unknown NSRef Special)
      NSInt   7048294 = 0x006B8C66 (vector is n<<2 = 0x01AE3198, funcptr)
      NSInt   0
    NSObjArray      4
      NSInt   2
      NSNil
      NSPtr   SYMclass                        @ -> 0x003C31CC
      NSPtr   SYMfuncptr                      @ -> 0x00419564
      NSPtr   SYMnumargs                      @ -> 0x0041957C

This vi macro will copy ID addresses into an array at the end of the file
 :map <f1> mayyGpdf@xwhi, //<esc>8ld$i,<esc>'ajn
 /"[a-zA-Z\.][a-zA-Z\.][a-zA-Z\.][a-zA-Z\.]".*flags_type_arm_word
 
*/


