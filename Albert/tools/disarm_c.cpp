/* Instruction printing code for the ARM
 Copyright (C) 1994, 95, 96, 97, 1998 Free Software Foundation, Inc.
 Contributed by Richard Earnshaw (rwe@pegasus.esprit.ec.org)
 Modification by James G. Smith (jsmith@cygnus.co.uk)
 
 This file is part of libopcodes.
 
 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free
 Software Foundation; either version 2 of the License, or (at your option)
 any later version.
 
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "disarm_c.h"


const char *get_symbol_at(unsigned int addr);
const char *get_plain_symbol_at(unsigned int addr);


static const char* arm_conditional[] = {
  "eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc", "hi", "ls", "ge", "lt", "gt", "le", "", "nv"};

static const char* arm_regnames[] = {
  "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "sp", "lr", "pc"};

static const char* arm_fp_const[] = {
  "0.0", "1.0", "2.0", "3.0", "4.0", "5.0", "0.5", "10.0"};

static const char* arm_shift[] = {
  "lsl", "lsr", "asr", "ror"};

static int print_insn_arm(unsigned int,
                          int,
                          char*);

// ============================================================	//
// void arm_decode_shift(int, char*)
// ============================================================	//

static void arm_decode_shift(int given,
                             char* str)
{
  char tmpStr[512];
  sprintf(tmpStr, "%s", arm_regnames[given & 0xf]);
  str = strcat(str, tmpStr);
  if ((given & 0xff0) != 0)
  {
    if ((given & 0x10) == 0)
    {
      int amount = (given & 0xf80) >> 7;
      int shift = (given & 0x60) >> 5;
      if (amount == 0)
      {
        if (shift == 3)
        {
          sprintf(tmpStr, ", rrx");
          str = strcat(str, tmpStr);
          return;
        }
        amount = 32;
      }
      sprintf(tmpStr, ", %s #%d", arm_shift[shift], amount);
      str = strcat(str, tmpStr);
    }
    else
    {
      sprintf(tmpStr, ", %s %s", arm_shift[(given & 0x60) >> 5], arm_regnames[(given & 0xf00) >> 8]);
      str = strcat(str, tmpStr);
    }
  }
}

// ============================================================	//
// void print_insn_arm (unsigned int, int, char*)
// ============================================================	//

static int print_insn_arm(unsigned int pc,
                          int given,
                          char* str)
{
  struct arm_opcode*     insn;
  char tmpStr[512];
  
  for (insn = arm_opcodes_c; insn->assembler; insn++)
  {
    if ((given & insn->mask) == insn->value)
    {
      const char* c;
      for (c = insn->assembler; *c; c++)
      {
        if (*c == '%')
        {
          switch (*++c)
          {
            case '%':
              str = strcat(str, "%%");
              break;
              
            case 'a':
              if (((given & 0x000f0000) == 0x000f0000) && ((given & 0x02000000) == 0))
              {
                int offset = given & 0xfff;
                if ((given & 0x00800000) == 0)
                  offset = -offset;
                unsigned int addr = offset + pc + 8;
                const char *sym = get_plain_symbol_at(addr);
                if (sym)
                  sprintf(tmpStr, "%s", sym);
                else
                  sprintf(tmpStr, "L%.8X", addr);
                str = strcat(str, tmpStr);
              }
              else
              {
                sprintf(tmpStr, "[%s", arm_regnames[(given >> 16) & 0xf]);
                str = strcat(str, tmpStr);
                if ((given & 0x01000000) != 0)
                {
                  if ((given & 0x02000000) == 0)
                  {
                    int offset = given & 0xfff;
                    if (offset)
                    {
                      sprintf(tmpStr, ", #%s%d", (((given & 0x00800000) == 0) ? "-" : ""), offset);
                      str = strcat(str, tmpStr);
                    }
                  }
                  else
                  {
                    sprintf(tmpStr, ", %s", (((given & 0x00800000) == 0) ? "-" : ""));
                    str = strcat(str, tmpStr);
                    arm_decode_shift(given, str);
                  }
                  
                  sprintf(tmpStr, "]%s", ((given & 0x00200000) != 0) ? "!" : "");
                  str = strcat(str, tmpStr);
                }
                else
                {
                  if ((given & 0x02000000) == 0)
                  {
                    int offset = given & 0xfff;
                    if (offset)
                    {
                      sprintf(tmpStr, "], #%s%d", (((given & 0x00800000) == 0) ? "-" : ""), offset);
                      str = strcat(str, tmpStr);
                    }
                    else
                      str = strcat(str, "]");
                  }
                  else
                  {
                    sprintf(tmpStr, "], %s", (((given & 0x00800000) == 0) ? "-" : ""));
                    str = strcat(str, tmpStr);
                    arm_decode_shift(given, str);
                  }
                }
              }
              break;
              
            case 's':
              if ((given & 0x004f0000) == 0x004f0000)
              {
                /* PC relative with immediate offset */
                int offset = ((given & 0xf00) >> 4) | (given & 0xf);
                if ((given & 0x00800000) == 0)
                  offset = -offset;
                sprintf(tmpStr, "0x%.8X", offset + pc + 8);
                str = strcat(str, tmpStr);
              }
              else
              {
                sprintf(tmpStr, "[%s", arm_regnames[(given >> 16) & 0xf]);
                str = strcat(str, tmpStr);
                if ((given & 0x01000000) != 0)
                {
                  /* pre-indexed */
                  if ((given & 0x00400000) == 0x00400000)
                  {
                    /* immediate */
                    int offset = ((given & 0xf00) >> 4) | (given & 0xf);
                    if (offset)
                    {
                      sprintf(tmpStr, ", #%s%d", (((given & 0x00800000) == 0) ? "-" : ""), offset);
                      str = strcat(str, tmpStr);
                    }
                  }
                  else
                  {
                    /* register */
                    sprintf(tmpStr, ", %s%s", (((given & 0x00800000) == 0) ? "-" : ""), arm_regnames[given & 0xf]);
                    str = strcat(str, tmpStr);
                  }
                  
                  sprintf(tmpStr, "]%s", ((given & 0x00200000) != 0) ? "!" : "");
                  str = strcat(str, tmpStr);
                }
                else
                {
                  /* post-indexed */
                  if ((given & 0x00400000) == 0x00400000)
                  {
                    /* immediate */
                    int offset = ((given & 0xf00) >> 4) | (given & 0xf);
                    if (offset)
                    {
                      sprintf(tmpStr, "], #%s%d", (((given & 0x00800000) == 0) ? "-" : ""), offset);
                      str = strcat(str, tmpStr);
                    }
                    else
                      strcat(str, "]");
                    
                  }
                  else
                  {
                    /* register */
                    sprintf(tmpStr, "], %s%s", (((given & 0x00800000) == 0) ? "-" : ""), arm_regnames[given & 0xf]);
                    str = strcat(str, tmpStr);
                  }
                }
              }
              break;
              
            case 'b':
            {
              unsigned int dst = BDISP(given) * 4 + pc + 8;
              const char *sym = get_plain_symbol_at(dst);
              if (sym) {
                if (dst<0x00800000)
                  sprintf(tmpStr, "%s", sym);
                else
                  sprintf(tmpStr, "VEC_%s", sym);
              } else {
                if (dst<0x00800000)
                  sprintf(tmpStr, "L%08X", dst);
                else
                  sprintf(tmpStr, "0x%08X", dst); // FIXME: wrong offset!
              }
              str = strcat(str, tmpStr);
            }
              break;
              
            case 'c':
              sprintf(tmpStr, "%s", arm_conditional[(given >> 28) & 0xf]);
              str = strcat(str, tmpStr);
              break;
              
            case 'm':
            {
              int started = 0;
              int RangeEnd = -2;
              int RangeFirst = -2;
              int reg;
              
              // Register List.
              
              str = strcat(str, "{");
              for (reg = 0; reg < 16; reg++)
                if ((given & (1 << reg)) != 0)
                {
                  if (RangeEnd + 1 == reg)
                  {
                    // I am in a block.
                    if (reg == 15)
                    {
                      // I should finish it anyway.
                      sprintf(tmpStr, "-%s", arm_regnames[reg]);
                      str = strcat(str, tmpStr);
                    }
                    else
                    {
                      RangeEnd++;
                    }
                  }
                  else
                  {
                    // I am not in a block.
                    // The block has been finished when processing the first reg out of it.
                    if (started)
                      str = strcat(str, ", ");
                    started = 1;
                    // Let's print this register & set both RangeEnd and RangeFirst.
                    RangeEnd = RangeFirst = reg;
                    sprintf(tmpStr, "%s", arm_regnames[reg]);
                    str = strcat(str, tmpStr);
                  }
                }
                else
                {
                  // This register is not here. Hence, I finish the old block.
                  if (started)
                  {
                    if (RangeEnd > RangeFirst)
                    {
                      if (RangeEnd == RangeFirst + 1)
                      {
                        // Two registers: I do comma.
                        sprintf(tmpStr, ", %s", arm_regnames[RangeEnd]);
                      }
                      else
                      {
                        // More: I do dash.
                        sprintf(tmpStr, "-%s", arm_regnames[RangeEnd]);
                      }
                      str = strcat(str, tmpStr);
                    }
                  }
                  RangeEnd = RangeFirst = -2;
                }
              str = strcat(str, "}");
            }
              break;
              
            case 'o':
              if ((given & 0x02000000) != 0)
              {
                int rotate = (given & 0xf00) >> 7;
                int immed = (given & 0xff);
                sprintf(tmpStr, "#%d", ((immed << (32 - rotate)) | (immed >> rotate)) & 0xffffffff);
                str = strcat(str, tmpStr);
              }
              else
                arm_decode_shift(given, str);
              break;
              
            case 'p':
              if ((given & 0x0000f000) == 0x0000f000)
                str = strcat(str, "p");
              break;
              
            case 't':
              if ((given & 0x01200000) == 0x00200000)
                str = strcat(str, "t");
              break;
              
            case 'h':
              if ((given & 0x00000020) == 0x00000020)
                str = strcat(str, "h");
              else
                str = strcat(str, "b");
              break;
              
            case 'A':
              sprintf(tmpStr, "[%s", arm_regnames[(given >> 16) & 0xf]);
              str = strcat(str, tmpStr);
              if ((given & 0x01000000) != 0)
              {
                int offset = given & 0xff;
                if (offset)
                {
                  sprintf(tmpStr, ", #%s%d]%s", ((given & 0x00800000) == 0 ? "-" : ""), offset * 4, ((given & 0x00200000) != 0 ? "!" : ""));
                  str = strcat(str, tmpStr);
                }
                else
                  str = strcat(str, "]");
              }
              else
              {
                int offset = given & 0xff;
                if (offset)
                {
                  sprintf(tmpStr, "], #%s%d", ((given & 0x00800000) == 0 ? "-" : ""), offset * 4);
                  str = strcat(str, tmpStr);
                }
                else
                  str = strcat(str, "]");
              }
              break;
              
            case 'C':
              switch (given & 0x00090000)
            {
              case 0:
                str = strcat(str, "_???");
                break;
              case 0x10000:
                str = strcat(str, "_ctl");
                break;
              case 0x80000:
                str = strcat(str, "_flg");
                break;
            }
              break;
              
            case 'F':
              switch (given & 0x00408000)
            {
              case 0:
                str = strcat(str, "4");
                break;
              case 0x8000:
                str = strcat(str, "1");
                break;
              case 0x00400000:
                str = strcat(str, "2");
                break;
              default:
                str = strcat(str, "3");
            }
              break;
              
            case 'P':
              switch (given & 0x00080080)
            {
              case 0:
                str = strcat(str, "s");
                break;
              case 0x80:
                str = strcat(str, "d");
                break;
              case 0x00080000:
                str = strcat(str, "e");
                break;
              default:
                str = strcat(str, "<illegal precision>");
                break;
            }
              break;
            case 'Q':
              switch (given & 0x00408000)
            {
              case 0:
                str = strcat(str, "s");
                break;
              case 0x8000:
                str = strcat(str, "d");
                break;
              case 0x00400000:
                str = strcat(str, "e");
                break;
              default:
                str = strcat(str, "p");
                break;
            }
              break;
            case 'R':
              switch (given & 0x60)
            {
              case 0:
                break;
              case 0x20:
                str = strcat(str, "p");
                break;
              case 0x40:
                str = strcat(str, "m");
                break;
              default:
                str = strcat(str, "z");
                break;
            }
              break;
              
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
              int bitstart = *c++ - '0';
              int bitend = 0;
              while (*c >= '0' && *c <= '9')
                bitstart = (bitstart * 10) + *c++ - '0';
              
              switch (*c)
              {
                case '-':
                  c++;
                  while (*c >= '0' && *c <= '9')
                    bitend = (bitend * 10) + *c++ - '0';
                  if (!bitend)
                    throw "disarm";
                  switch (*c)
                {
                  case 'r':
                  {
                    int reg;
                    reg = given >> bitstart;
                    reg &= (2 << (bitend - bitstart)) - 1;
                    sprintf(tmpStr, "%s", arm_regnames[reg]);
                    str = strcat(str, tmpStr);
                  }
                    break;
                  case 'd':
                  {
                    int reg;
                    reg = given >> bitstart;
                    reg &= (2 << (bitend - bitstart)) - 1;
                    sprintf(tmpStr, "%d", reg);
                    str = strcat(str, tmpStr);
                  }
                    break;
                  case 'x':
                  {
                    int reg;
                    reg = given >> bitstart;
                    reg &= (2 << (bitend - bitstart)) - 1;
                    sprintf(tmpStr, "0x%08x", reg);
                    str = strcat(str, tmpStr);
                  }
                    break;
                  case 'f':
                  {
                    int reg;
                    reg = given >> bitstart;
                    reg &= (2 << (bitend - bitstart)) - 1;
                    if (reg > 7)
                      sprintf(tmpStr, "#%s", arm_fp_const[reg & 7]);
                    else
                      sprintf(tmpStr, "f%d", reg);
                    str = strcat(str, tmpStr);
                  }
                    break;
                  default:
                    throw "disarm";
                }
                  break;
                case '`':
                  c++;
                  if ((given & (1 << bitstart)) == 0)
                  {
                    sprintf(tmpStr, "%c", *c);
                    str = strcat(str, tmpStr);
                  }
                  break;
                case '\'':
                  c++;
                  if ((given & (1 << bitstart)) != 0)
                  {
                    sprintf(tmpStr, "%c", *c);
                    str = strcat(str, tmpStr);
                  }
                  break;
                case '?':
                  ++c;
                  if ((given & (1 << bitstart)) != 0)
                    sprintf(tmpStr, "%c", *c++);
                  else
                    sprintf(tmpStr, "%c", *++c);
                  str = strcat(str, tmpStr);
                  break;
                default:
                  throw "disarm";
              }
              break;
              
              //default:
              //throw "disarm";
            }
          }
        }
        else
        {
          sprintf(tmpStr, "%c", *c);
          str = strcat(str, tmpStr);
        }
      }
      return 4;
    }
  }
  return 0;
}


int disarm_c_old(char *dst, unsigned int addr, unsigned int cmd)
{
  unsigned int given = cmd;
  char theString[5];
  char theDisasmLine[2048];
  char theChar;
  int i;
  int pc = addr;
  int incr = 0;
  
  theString[4] = '\0';
  {
    for (i = 0; i < 4; i++)
    {
      theChar = ((char*) & given)[3-i];
      if (theChar < '!')
        theChar = '.';
      theString[i] = theChar;
    }
    
    theDisasmLine[0] = '\0';
    incr = print_insn_arm(pc, given, theDisasmLine);
    
    char *t = strchr(theDisasmLine, '\t');
    if (t) {
      int dt = t-theDisasmLine;
      if (dt<8) {
        memmove(theDisasmLine+8, t+1, strlen(t));
        dt = 7;
      }
      memset(t, 32, dt-(t-theDisasmLine)+1);
    }
    
    sprintf(dst, "%-32s  ; 0x%.8X 0x%.8X - %-5s", theDisasmLine, addr, given, theString);
  }
  return incr;
}

const char* GetRegister(unsigned int R)
{
  return arm_regnames[R&15];
}

const char* GetTferSize(unsigned int B)
{
  return (B&1) ? "Byte" : "Word";
}

const char* GetAddSub(unsigned int U)
{
  return (U&1) ? "+" : "-";
}

const char* GetOffset(unsigned int I, unsigned int off, int &isZero)
{
  static char str[80];
  
  isZero = 0;
  if (I&1) { // 0-3 = offset register, 4-11 = shift
    const char* offsetRegister = GetRegister(off);
    const char* shiftRegister = GetRegister(off>>8);
    unsigned int amount = (off>>7)&0x1f;
    switch ((off>>4)&7) {
      case 0:
        if (amount)
          sprintf(str, "(%s<<%d)", offsetRegister, amount);
        else
          sprintf(str, "%s", offsetRegister);
        break; // LSL n
      case 1: sprintf(str, "(%s>>%d)", offsetRegister, amount); break; // LSR n
      case 2: sprintf(str, "(((KSInt32)%s)>>%d)", offsetRegister, amount); break; // ASR n
      case 3: sprintf(str, "(ROR(%s, %d))", offsetRegister, amount); break; // ROR n
      case 4: sprintf(str, "(%s<<(%s&0xff))", offsetRegister, shiftRegister); break; // LSL n
      case 5: sprintf(str, "(%s>>(%s&0xff))", offsetRegister, shiftRegister); break; // LSR n
      case 6: sprintf(str, "(((KSInt32)%s)>>(%s&0xff))", offsetRegister, shiftRegister); break; // ASR n
      case 7: sprintf(str, "(ROR(%s, (%s&0xff)))", offsetRegister, shiftRegister); break; // ROR n
    }
    return str;
  } else {
    if ((off & 0xfff)==0) isZero = 1;
    sprintf(str, "%d", off & 0xfff);
    return str;
  }
}

const char *GetOp2(unsigned int I, unsigned int off)
{
  static char str[80];
  if (I&1) {
    // rot
    unsigned int r = (off>>7) & 30;
    unsigned int x = off & 0xff;
    unsigned int v = (x>>r) | (x<<(32-r));
    sprintf(str, "%d", (signed int)v);
    return str;
  } else {
    // shift
    int isZero;
    return GetOffset(1, off, isZero);
  }
}

int disarm_c(char *buf, unsigned int addr, unsigned int cmd)
{
  unsigned int cf = 0;
  char *dst = buf;
  
  *dst = 0;
  
  if ((cmd & 0x0fc000f0)==0x00000090) { // MUL  CCCC.0000.00AS.RRRd.RRRn.RRRs.1001.RRRm
    const char *Rd = GetRegister(cmd>>16);
    const char *Rn = GetRegister(cmd>>12);
    const char *Rs = GetRegister(cmd>>8);
    const char *Rm = GetRegister(cmd>>0);
    cf = (cmd>>20);
    if ((cmd>>21)&1) {
      dst += sprintf(dst, "%s = %s * %s + %s;", Rd, Rm, Rs, Rn);
    } else {
      dst += sprintf(dst, "%s = %s * %s;", Rd, Rm, Rs);
    }
  } else if ((cmd & 0x0cb00ff0)==0x01000090) { // SWP  CCCC.0001.0B00.RRRn.RRRd0000.1001.RRRm
    const char *size = GetTferSize(cmd>>22);          // B
    const char *Rn = GetRegister(cmd>>16);
    const char *Rd = GetRegister(cmd>>12);
    const char *Rm = GetRegister(cmd>>0);
    if (strcmp(Rd, Rm)==0) {
      dst += sprintf(dst, "{ KUInt32 tmp = NewtRead%s(%s); NewtWrite%s(%s, %s); %s = tmp; }", size, Rn, size, Rn, Rm, Rd);
    } else {
      dst += sprintf(dst, "%s = NewtRead%s(%s);\nNewtWrite%s(%s, %s);", Rd, size, Rn, size, Rn, Rm);
    }
  } else if ((cmd & 0x0c000000)==0x00000000) { // Data Processing  CCCC.00IO.OOOS.RRRn.RRRd.oooo.oooo.ooo2
    const char *Rn = GetRegister(cmd>>16);
    const char *Rd = GetRegister(cmd>>12);
    const char *op2 = GetOp2(cmd>>25, cmd);
    cf = (cmd>>20);
    switch ((cmd>>21)&15) {
      case  0: dst += sprintf(dst, "%s = %s & %s;", Rd, Rn, op2); break;
      case  1: dst += sprintf(dst, "%s = %s ^ %s;", Rd, Rn, op2); break;
      case  2: dst += sprintf(dst, "%s = %s - %s;", Rd, Rn, op2); break;
      case  3: dst += sprintf(dst, "%s = %s - %s;", Rd, op2, Rn); break;
      case  4: dst += sprintf(dst, "%s = %s + %s;", Rd, Rn, op2); break;
      case  5: dst += sprintf(dst, "%s = %s + %s + carry;", Rd, Rn, op2); break;
      case  6: dst += sprintf(dst, "%s = %s - %s + carry - 1;", Rd, Rn, op2); break;
      case  7: dst += sprintf(dst, "%s = %s - %s + carry - 1;", Rd, op2, Rn); break;
      case  8: dst += sprintf(dst, "cond = %s & %s;", Rn, op2); cf=1; break;
      case  9: dst += sprintf(dst, "cond = %s ^ %s;", Rn, op2); cf=1; break;
      case 10: dst += sprintf(dst, "cond = %s - %s;", Rn, op2); cf=1; break;
      case 11: dst += sprintf(dst, "cond = %s + %s;", Rn, op2); cf=1; break;
      case 12: dst += sprintf(dst, "%s = %s | %s;", Rd, Rn, op2); break;
      case 13: dst += sprintf(dst, "%s = %s;", Rd, op2); break;
      case 14: dst += sprintf(dst, "%s = %s & ~%s;", Rd, Rn, op2); break;
      case 15: dst += sprintf(dst, "%s = ~%s;", Rd, op2); break;
    }
  } else if ((cmd & 0x0e000010)==0x06000010) {
    dst += sprintf(dst, "undefined();");
  } else if ((cmd & 0x0c000000)==0x04000000) { // LDR,STR  CCCC.01IP.UBWL.RRRn.RRRd.oooo.oooo.oooo
    int isZero;
    const char *baseRegister = GetRegister(cmd>>16);  // Rn
    const char *dataRegister = GetRegister(cmd>>12);  // Rd
    const char *size = GetTferSize(cmd>>22);          // B
    const char *addSub = GetAddSub(cmd>>23);          // U
    const char *offset = GetOffset(cmd>>25, cmd, isZero); // I
    if (isZero) addSub = offset = "";
    if (cmd & 0x00100000) { // LDR                    // L
      if ((cmd>>24)&1) {                              // P
        if ((cmd>>21)&1) {                            // W
          // pre add, write back
          if (!isZero) dst += sprintf(dst, "%s = %s%s%s;\n", baseRegister, baseRegister, addSub, offset);
          dst += sprintf(dst, "%s = NewtRead%s(%s);", dataRegister, size, baseRegister);
        } else {
          // pre add, don't write back
          dst += sprintf(dst, "%s = NewtRead%s(%s%s%s);", dataRegister, size, baseRegister, addSub, offset);
        }
      } else {
        // post add, always write back
        dst += sprintf(dst, "%s = NewtRead%s(%s);", dataRegister, size, baseRegister);
        if (!isZero) dst += sprintf(dst, "\n%s = %s%s%s;", baseRegister, baseRegister, addSub, offset);
      }
    } else { // STR
      if ((cmd>>24)&1) {                              // P
        if ((cmd>>21)&1) {                            // W
          // pre add, write back
          if (!isZero) dst += sprintf(dst, "%s = %s%s%s;\n", baseRegister, baseRegister, addSub, offset);
          dst += sprintf(dst, "NewtWrite%s(%s, %s);", size, baseRegister, dataRegister);
        } else {
          // pre add, don't write back
          dst += sprintf(dst, "NewtWrite%s(%s%s%s, %s);", size, baseRegister, addSub, offset, dataRegister);
        }
      } else {
        // post add, always write back
        dst += sprintf(dst, "NewtWrite%s(%s, %s);", size, baseRegister, dataRegister);
        if (!isZero) dst += sprintf(dst, "\n%s = %s%s%s;\n", baseRegister, baseRegister, addSub, offset);
      }
    }
  } else if ((cmd & 0x0e000000)==0x08000000) { // block tfer  CCCC.100P.USWL.RRRn.LLLL.LLLL.LLLL.LLLL
                                               // tfer lowest to highest
                                               // lowest reg is always at lowest addr
    const char *baseRegister = GetRegister(cmd>>16);  // Rn
    const char *addSub = GetAddSub(cmd>>23);          // U
    int i, nReg = 0, n = 0;
    for (i=0; i<16; i++) {
      if (cmd & (1<<i)) nReg++;
    }
    dst += sprintf(dst, "{\n\tKUInt32 tmp = %s;", baseRegister);
    for (i=0; i<16; i++) {
      if (cmd & (1<<i)) {
        const char *dataRegister = GetRegister(i);
        if ((cmd>>20)&1) { // LDM
          if ((cmd>>24)&1) { // pre
            dst += sprintf(dst, "\n\t%s = NewtReadWord(tmp%s%d);", dataRegister, addSub, 4*(n+1));
          } else { // post
            dst += sprintf(dst, "\n\t%s = NewtReadWord(tmp%s%d);", dataRegister, addSub, 4*n);
          }
        } else { // STM
          if ((cmd>>24)&1) { // pre
            dst += sprintf(dst, "\n\tNewtWriteWord(tmp%s%d, %s);", addSub, 4*(nReg-n), dataRegister);
          } else { // post
            dst += sprintf(dst, "\n\tNewtWriteWord(tmp%s%d, %s);", addSub, 4*(nReg-n-1), dataRegister);
          }
        }
        n++;
      }
    }
    if ((cmd>>21)&1) dst += sprintf(dst, "\n\t%s = tmp %s %d;", baseRegister, addSub, 4*n);
    dst += sprintf(dst, "\n}");
    if ((cmd>>22)&1) dst += sprintf(dst, "\n#warn the instructions above should force user mode");
  } else if ((cmd & 0x0e000000)==0x0a000000) { // B, BL
    unsigned int dstAddr = BDISP(cmd&0x00ffffff) * 4 + addr + 8;
    const char *sym = get_plain_symbol_at(dstAddr);
    if (sym) {
      if ((cmd>>24)&1) { // BL
        dst += sprintf(dst, "r0 = %s(...);", sym); // FIXME: we can improve this a lot!
      } else { // BL
        dst += sprintf(dst, "return %s(...);", sym);
      }
    } else {
      if ((cmd>>24)&1) { // BL
        dst += sprintf(dst, "r0 = L%08X(...);", dstAddr);
      } else { // BL
        dst += sprintf(dst, "goto L%08X;", dstAddr);
      }
    }
  }
  if (cf&1) dst += sprintf(dst, "\n#warn the instruction above should set condition flags");
  
  unsigned int cond = cmd>>28;
  if (cond<14) {
    char *tmp = strdup(buf);
    dst = buf;
    if (strchr(tmp, '\n')) { // multi line
      dst += sprintf(dst, "if (%s) {\n%s\n}", arm_conditional[cond], tmp);
    } else { // single line
      dst += sprintf(dst, "if (%s) %s", arm_conditional[cond], tmp);
    }
    free(tmp);
  }

  return 0;
}

