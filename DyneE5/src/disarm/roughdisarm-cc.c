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
#include <stdlib.h>
#include <string.h>

#define DEFINE_TABLE
#include "arm-opc-cc.h"
#include "../mainc.h"

static char* arm_conditional[] = {
  "eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc", "hi", "ls", "ge", "lt", "gt", "le", "", "nv"
};

static char* arm_regnames[] = {
  "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "sp", "lr", "pc"
};

static char* arm_fp_const[] = {
  "0.0", "1.0", "2.0", "3.0", "4.0", "5.0", "0.5", "10.0"
};

static char* arm_shift[] = {
  "lsl", "lsr", "asr", "ror"
};

static int print_insn_arm(unsigned long,
              long,
              char*);

static const char *swi_lut[] = {
  "GetPortSWI", "PortSendSWI", "PortReceiveSWI", "EnterAtomicSWI", "ExitAtomicSWI",
  "GenericSWI", "GenerateMessageIRQ", "PurgeMMUTBLEntry", "FlushMMU", "FlushCache",
  "GetCPUVersion", "SemaphoreOpGlue", "SetDomainRegister", "SMemSetBufferSWI",
  "SMemGetSizeSWI", "SMemCopyToSharedSWI", "SMemCopyFromSharedSWI",
  "SMemMsgSetTimerParmsSWI", "SMemMsgSetMsgAvailPortSWI",
  "SMemMsgGetSenderTaskIdSWI", "SMemMsgSetUserRefConSWI", "SMemMsgGetUserRefConSWI",
  "SMemMsgCheckForDoneSWI", "SMemMsgMsgDoneSWI", "TurnOffCache",
  "TurnOnCache", "??", "MonitorDispatchSWI", "MonitorExitSWI", "MonitorThrowSWI",
  "EnterFIQAtomicSWI", "ExitFIQAtomicSWI", "MonitorFlushSWI", "PortResetFilterSWI",
  "DoSchedulerSWI"
};

// ============================================================  //
// void arm_decode_shift(long, char*)
// ============================================================  //

static void arm_decode_shift(long given,
               char* str)
{
  char tmpStr[2048];
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

// ============================================================  //
// void print_insn_arm (unsigned long, long, char*)
// ============================================================  //

static int print_insn_arm(unsigned long pc,
              long given,
              char* str)
{
  struct arm_opcode*     insn;
  char tmpStr[2048];
  int bitend;

  memset(tmpStr, 0, 2048);

  for (insn = arm_opcodes; insn->assembler; insn++)
  {
    if ((given & insn->mask) == insn->value)
    {
      char* c;
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
                sprintf(tmpStr, "0x%.8X", offset + pc + 8);
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
                      sprintf(tmpStr, ", %s#%d", (((given & 0x00800000) == 0) ? "-" : ""), offset);
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
                      sprintf(tmpStr, "], %s#%d", (((given & 0x00800000) == 0) ? "-" : ""), offset);
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
                      sprintf(tmpStr, ", %s#%d", (((given & 0x00800000) == 0) ? "-" : ""), offset);
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
                      sprintf(tmpStr, "], %s#%d", (((given & 0x00800000) == 0) ? "-" : ""), offset);
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
                const char *sym = dn_get_symbol(dst);
                if (sym) {
                  const char *s = sym;
                  char *d = tmpStr, bb=0, hb=0;
                  while (*s) {
                    char c = *s++;
                    if (c==')') bb--;
                    if (bb<1) *d++ = c;
                    if (c=='(') { hb = 1; bb++; }
                  }
                  if (!hb) { strcpy(d, "()"); d+=2; }
                  sprintf(d, "; // 0x%.8X %s", dst, sym);
                } else
                  sprintf(tmpStr, "(*0x%.8X)()", dst);
                str = strcat(str, tmpStr);
              }
              break;

            case 'c':
              sprintf(tmpStr, "%s", arm_conditional[(given >> 28) & 0xf]);
              str = strcat(str, tmpStr);
              break;

            case '!':
              if (((given >> 28) & 0xf) != 0x0e) {
                sprintf(tmpStr, "if (%s) ", arm_conditional[(given >> 28) & 0xf]);
                str = strcat(str, tmpStr);
              }
              break;

            case 'w':
              if ((given&0xffffff)<0x22) {
                sprintf(tmpStr, " (%s)", swi_lut[given&0xffffff]);
                str = strcat(str, tmpStr);
              }
              break;

            case 'n': // DyneE5 native call
              { 
                int n = (given&0xf)|((given>>1)&0x70)|((given>>5)&0x7f80);
                sprintf(tmpStr, "%d", n);
                str = strcat(str, tmpStr);
              }
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
                sprintf(tmpStr, "#0x%08x", ((immed << (32 - rotate)) | (immed >> rotate)) & 0xffffffff);
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
                  sprintf(tmpStr, ", %s#%d]%s", ((given & 0x00800000) == 0 ? "-" : ""), offset * 4, ((given & 0x00200000) != 0 ? "!" : ""));
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
                  sprintf(tmpStr, "], %s#%d", ((given & 0x00800000) == 0 ? "-" : ""), offset * 4);
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
                bitend = 0;
                while (*c >= '0' && *c <= '9')
                  bitstart = (bitstart * 10) + *c++ - '0';

                switch (*c)
                {
                  case '-':
                    c++;
                    while (*c >= '0' && *c <= '9')
                      bitend = (bitend * 10) + *c++ - '0';
                    if (!bitend)
                      abort();
                    switch (*c)
                    {
                      case 'r':
                        {
                          long reg;
                          reg = given >> bitstart;
                          reg &= (2 << (bitend - bitstart)) - 1;
                          sprintf(tmpStr, "%s", arm_regnames[reg]);
                          str = strcat(str, tmpStr);
                        }
                        break;
                      case 'd':
                        {
                          long reg;
                          reg = given >> bitstart;
                          reg &= (2 << (bitend - bitstart)) - 1;
                          sprintf(tmpStr, "%d", reg);
                          str = strcat(str, tmpStr);
                        }
                        break;
                      case 'x':
                        {
                          long reg;
                          reg = given >> bitstart;
                          reg &= (2 << (bitend - bitstart)) - 1;
                          sprintf(tmpStr, "0x%08x", reg);
                          str = strcat(str, tmpStr);
                        }
                        break;
                      case 'f':
                        {
                          long reg;
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
                        abort();
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
                    abort();
                }
                break;

              default:
                abort();
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
  abort();

  return 0;
}


int disarm_cc(char *dst, unsigned int addr, unsigned int cmd)
{
  long given = cmd;
  char theString[5];
  char theDisasmLine[2048];
  char theChar;
  char theMnemonic[128];
  char theOperand[256];
  int i;
  long pc = addr;
  long incr = 0;

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

    if (sscanf(theDisasmLine, "%s\t", theMnemonic))
    {
      // Let's get the operand string.
      sprintf(theOperand, "%s", &theDisasmLine[strlen(theMnemonic) + 1]);
      sprintf(dst, "%s %s\n", theMnemonic, theOperand);
    }
    else
    {
      sprintf(dst, "%s\n", theDisasmLine);
    }
  }
  return incr;
}
