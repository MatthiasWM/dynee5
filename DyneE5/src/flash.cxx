
#include "flash.h"
#include "cpu.h"
#include "main.h"

#include <stdio.h>
#include <string.h>


Dn_Flash::Dn_Flash() {
}

Dn_Flash::~Dn_Flash() {
}


/* This class emulates the Intel 28F016SA 16-MBit Flash RAM in Word Mode */

//FILE *fl;

Dn_Flash_28F016::Dn_Flash_28F016(const char *name) {
  //fl = fopen("../flashlog.txt", "wb");
  int i;
  state = 0xff;
  cycle = 0;
  GSR = 0x86; // global status register
  CSR = 0x80; // compatible status register
  for (i=0; i<32; i++) BSR[i] = 0x80; // block status register
  WCH = WCL = 0; // word count
  WCswap = 0; // word count swap
  strcpy(pName, name);
  memset(mem, 0xff, flashSize);
}

Dn_Flash_28F016::~Dn_Flash_28F016() {
}

void Dn_Flash_28F016::read(FILE *f) {
  fread(mem, 1, flashSize, f);
}

void Dn_Flash_28F016::write(FILE *f) {
  fwrite(mem, 1, flashSize, f);
}

unsigned short Dn_Flash_28F016::peek(unsigned int addr) {
  return mem[addr];
}

unsigned short Dn_Flash_28F016::read(unsigned int addr) {
  if (addr>=flashSize) {
    printf("ERROR: Dn_Flash_28F016::read: Invalid address!\n");
    cpu->stop();
    return 0;
  }
  unsigned short ret = mem[addr];
  char *msg = 0;
  switch (state) {
    case 0x00:
      msg = "(not me-state)";
      ret = 0;
      cycle = 0;
      break;
    case 0xff:
      msg = "block read";
      cycle = 0;
      break;
    case 0x90: // intelligent identifier
      switch (addr) {
        // 0x89 (ROM:2045d8) -> manufacturer = 66a0
        // 0xb0 (ROM:2045fc) -> manufacturer = 6688
        case 0: ret = 0x0089; msg = "ID"; break;
        case 1: ret = 0x66a0; msg = "manufacturer"; break;
        case 2: ret = 0x0000; msg = "signature"; break;
        default: 
          msg = "ERROR: invalid 'Intelligent Identifier' address";
          break;
      }
      cycle = 0;
      break;
    case 0x71:
      switch (addr&0x7fff) {
        case 1: // BSR[n]
          msg = "get block status register";
          ret = BSR[(addr>>15)&0x1f];
          break;
        case 2: // GSR
          msg = "get global status register";
          ret = GSR;
          break;
        default:
          msg = "ERROR: get status register failed: INVALID ADDRESS";
          cpu->stop();
          cycle = 0;
          break;
      }
    default:
      cycle = 0;
      break;
  }
  if (!msg) {
    msg = "ERROR: UNSUPPORTED STATE";
    cpu->stop();
  }
  const char *pat = "FLASH%s:  read %08x:%04x : %s (c=%d)\n";
  printf(pat, pName, addr, ret, msg, cycle);
  //fprintf(fl, pat, pName, addr, ret, msg, cycle);
  return ret;
}


void Dn_Flash_28F016::write(unsigned int addr, unsigned short v) {
  if (addr>=flashSize) {
    printf("ERROR: Dn_Flash_28F016::write: Invalid address!\n");
    cpu->stop();
    return;
  }
  int i;
  char *msg = 0L;
retry:
  if (cycle==0) {
    unsigned char ts = (unsigned char)v;
    //if (ts==0x00) return; // are we talking to another bank?
    state = v; 
  }
  switch (state) {
    case 0x00: 
      msg = "C0: (not me)";
      cycle = 0; break;
    case 0xff: 
      msg = "C0: Read Array";
      cycle = 0; break;
    case 0xe0:
      switch (cycle) {
        case 0:
          msg = "C0: Sequential load to page buffer";
          cycle++; break;
        case 1:
          msg = "C1: Sequential load to page buffer (count lo)";
          WCL = v & 0xff;
          cycle++; break;
        case 2:
          msg = "C2: Sequential load to page buffer (count hi)";
          WCH = v & 0xff; // WCH is ignored
          cycle++; break;
        default:
          msg = "Cn: store in page buffer";
          page[GSR&1][addr&0x7f] = v;
          if (WCL==0) {
            cycle = 0;
          } else {
            cycle++; WCL--;
          }
          break;
      }
      break;
    case 0x97: 
      if (cycle==0) {
        msg = "C0: Upload Status Bits";
        cycle = 1; break;
      } else if (cycle==1) {
        if ((v&0xff)==0xd0) {
          msg = "C1: Upload Status Bits confirmed"; 
          BSR[(addr>>15)&0x1f] |= 0x40;
        } else {
          msg = "C1: Upload Status NOT confirmed";
        }
      }
      cycle = 0; break;
    case 0x90:
      if (cycle!=0) { cycle=0; goto retry; }
      msg = "C0: Intelligent Identifier";
      cycle = 1; break;
    case 0x80:
      msg = "C0: Abort";
      cycle = 0; state = 0xff;
      break;
    case 0x77: 
      if (cycle==0) {
        msg = "C0: Lock block";
        cycle = 1; break;
      } else if (cycle==1) {
        if ((v&0xff)==0xd0) {
          msg = "C1: Lock block confirmed"; 
          BSR[(addr>>15)&0x1f] &= ~0x40;
        } else {
          msg = "C1: Lock block NOT confirmed";
        }
      }
      cycle = 0; break;
    case 0x71:
      msg = "C0: Read Extended Status Register";
      cycle = 1; break;
    case 0x50:
      msg = "C0: Clear Status Register";
      CSR &= 0xffffffc7;
      GSR &= 0xffffffdf;
      for (i=0; i<32; i++) BSR[i] &= 0xffffffdb;
      cycle = 0; state = 0xff; break;
    case 0x20:
      if (cycle==0) {
        msg = "C0: Block erase";
        cycle++; break;
      } else if (cycle==1) {
        if ((v&0xff)==0xd0) {
          // addr should mask out bits 16-20, depending on mode
          if (BSR[(addr>>15)&0x1f] & 0x40) {
            memset(mem+addr, 0xff, 65536); 
            msg = "C1: Block erase confirmed"; 
          } else {
            msg = "C1: ERROR: Block erase confirmed, but write protected!"; 
          }
        } else {
          msg = "C1: Block erase FAILED";
        }
      }
      cycle = 0; break;
    case 0x0c:
      switch (cycle) {
        case 0:
          msg = "C0: Page buffer write to flash";
          cycle++; break;
        case 1:
          msg = "C1: Page buffer write to flash (count)";
          WCL = v & 0xff;
          cycle++; break;
        default:
          WCH = v & 0xff; // WCH is ignored
          if (BSR[(addr>>15)&0x1f] & 0x40) {
            for (i=0; i<=WCL; i++) {
              mem[addr+i] = page[GSR&1][(addr+i)&0x7f];
            }
            msg = "C2: Page buffer write to flash (count)";
          } else {
            msg = "C2: ERROR: Page buffer write to flash, but write protection set!";
          }
          cycle=0; break;
      }
      break;
    default:
      if (cycle!=0) { cycle=0; goto retry; }
      break;
  }
  if (!msg) {
    msg = "UNSUPPORTED STATE";
    cpu->stop();
  }
  const char *pat = "FLASH%s: write %08x:%04x : %s (c=%d)\n";
  printf(pat, pName, addr, v, msg, cycle);
  //fprintf(fl, pat, pName, addr, v, msg, cycle);
}


