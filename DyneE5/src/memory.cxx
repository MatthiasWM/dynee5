

#ifdef WIN32
# include <windows.h>
# include <winsock2.h>
#else
# include <arpa/inet.h>
#endif

#include "memory.h"
#include "main.h"
#include "dyne.h"
#include "symbols.h"

#include <FL/Fl_Image.H>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cpu.h"


void err() {
  printf("pc=%08x: ", cpu->get_pc());
}

/**
 * Initialize a chunk of memory.
 */
Dn_Memory_Chunk::Dn_Memory_Chunk() 
: pStart(0),
  pSize(0),
  pData(0L)
{
}

/*--------------------------------*/

/**
 * Create a chunk of simple RAM.
 */
Dn_Memory_Chunk::Dn_Memory_Chunk(unsigned int start, unsigned int size) 
: pStart(start),
  pSize(size),
  pData(0L) 
{
  pData = (unsigned char*)malloc(size);
  memset(pData, 0x00, size);
}


/**
 * Free any allocated memory.
 */
Dn_Memory_Chunk::~Dn_Memory_Chunk() {
  if (pData) 
    free(pData);
}

/**
 * Just dump the memory into the file
 */
void Dn_Memory_Chunk::write(FILE *f) {
  fwrite(pData, 1, pSize, f);
}

/**
 * Just slurp the data from the file into memory
 */
void Dn_Memory_Chunk::read(FILE *f) {
  fread(pData, 1, pSize, f);
}

/**
 * Load this segment from a binary dump.
 */
char Dn_Memory_Chunk::load(const char *filename, unsigned int offset) {
  if (!filename || !*filename) 
    return 0;
  FILE *fin = fopen(filename, "rb");
  if (!fin) {
    printf("DyneE5: %s at 0x%08x\n", filename, offset);
    perror("DyneE5: can't open memory file");
    return 0;
  }
  // FIXME more error checking needed!
  if (offset) fseek(fin, offset, SEEK_SET);
  fread(pData, pSize, 1, fin);
  fclose(fin);
  return 1;
}


/**
 * Save this segment as a binary dump.
 */
char Dn_Memory_Chunk::save(const char *filename) {
  if (!filename || !*filename) 
    return 0;
  FILE *fout = fopen(filename, "wb");
  if (!fout) {
    printf("ERROR: Can't save RAM dump to '%s'\n", filename);
    perror("ERROR");
    return 0;
  }
  // FIXME more error checking needed!
  fwrite(pData, pSize, 1, fout);
  fclose(fout);
  return 1;
}


/**
 * Return 1, if the address is available inside this block.
 */
char Dn_Memory_Chunk::has(unsigned int a) {
  if (a>=pStart && a<pStart+pSize) return 1;
  return 0;
}


/**
 * Return a byte at the given address.
 */
unsigned char Dn_Memory_Chunk::get8(unsigned int a, char) {
  return pData[a-pStart];
}


/**
 * Return a short at a given address, even though our CPU 
 * doesn't really support this.
 */
unsigned short Dn_Memory_Chunk::get16(unsigned int a, char) {
  unsigned char *src = pData + (a-pStart);
  return (src[0]<<8)|(src[1]);
}


/**
 * Return a word at the given address.
 */
unsigned int Dn_Memory_Chunk::get32(unsigned int a, char patch) {
  unsigned char *src = pData + (a-pStart);
  return (src[0]<<24)|(src[1]<<16)|(src[2]<<8)|(src[3]);
}


/**
 * Set a byte at the given address.
 */
void Dn_Memory_Chunk::set8(unsigned int a, unsigned char v, char patch) {
  pData[a-pStart] = v;
}


/**
 * Set a short at a given address, even though our CPU 
 * doesn't really support this.
 */
void Dn_Memory_Chunk::set16(unsigned int a, unsigned short v, char patch) {
  unsigned char *dst = pData + (a-pStart);
  *dst++ = v>>8;
  *dst   = v;
}


/**
 * Return a word at the given address.
 */
void Dn_Memory_Chunk::set32(unsigned int a, unsigned int v, char patch) {
  unsigned char *dst = pData + (a-pStart);
  *dst++ = v>>24;
  *dst++ = v>>16;
  *dst++ = v>>8;
  *dst   = v;
}

/*--------------------------------*/


  // ARM File Format: Data offset is 0x80
  //   at 0x24, offset to symbol table (0071a95c)
  //            786d6c: start of ascii table [size]text[00]...
  //            71bd08: ??
  //   at 0x014: 0071A95C 
/*
    UNNA: mpdumper
    +----------------------+
00: | BL DecompressCode    |  NOP if the image is not compressed.
    +----------------------+
04: | BL SelfRelocCode     |  NOP if the image is not self-relocating.
    +----------------------+
08: | BL DBGInit/ZeroInit  |  NOP if the image has none.
    +----------------------+
0C: | BL ImageEntryPoint   |  BL to make header addressable via r14...
    |       or             |  ...but the application shall not return...
    | EntryPoint offset    |  Non-executable AIF uses an offset, not BL
    +----------------------+
10: | <Program Exit Instr> |  ...last ditch in case of return.
    +----------------------+
14: | Image ReadOnly size  |  Includes header size if executable AIF;
    |                      |  excludes headser size if non-executable AIF.
    +----------------------+
18: | Image ReadWrite size |  Exact size - a multiple of 4 bytes
    +----------------------+
1C: | Image Debug size     |  Exact size - a multiple of 4 bytes
    +----------------------+
20: | Image zero-init size |  Exact size - a multiple of 4 bytes
    +----------------------+
24: | Image debug type     |  0, 1, 2, or 3 (see note below).
    +----------------------+
28: | Image base           |  Address the image (code) was linked at.
    +----------------------+ 
2C: | Work Space           |  Min work space - in bytes - to be reserved
    +----------------------+  by a self-moving relocatable image.
30: | Address mode: 26/32  |  LS byte contains 26 or 32;
    | + 3 flag bytes       |  bit 8 set when using a separate data base.
    +----------------------+
34: | Data base            |  Address the image data was linked at.
    +----------------------+
38: | Two reserved words   |
    | ...initially 0...    |
    +----------------------+
40: | <Debug Init Instr>   |  NOP if unused.
    +----------------------+
44: | Zero-init code       | 
    | (14 words as below)  |  Header is 32 words long.
    +----------------------+
http://www.chiark.greenend.org.uk/~theom/riscos/docs/CodeStds/AIF-1993.txt

*/

/**
 * Create a ROM chunk. ROM chunks can be patched with a secondary code
 * layer. Write operations flagged DN_MEM_PATCH go into the patch
 * layer. Instructions are always read from the patch layer, but data 
 * is normally read from the unpatched layer.
 */
Dn_ROM_Chunk::Dn_ROM_Chunk(unsigned int start, unsigned int size)
: Dn_Memory_Chunk(start, size),
  pPatchedData(0L)
{ }


/**
 * Release the patch layer, if we used it.
 */
Dn_ROM_Chunk::~Dn_ROM_Chunk() {
  if (pPatchedData)
    free(pPatchedData);
}


/**
 * Write a byte.
 */
void Dn_ROM_Chunk::set8(unsigned int a, unsigned char v, char flags) {
  if (flags&DN_MEM_PATCH) {
    if (!pPatchedData) {
      pPatchedData = (unsigned char*)malloc(pSize);
      memcpy(pPatchedData, pData, pSize);
    }
    pPatchedData[a-pStart] = v;
  } else {
    err(); printf("invalid write access at %08x (%02x) (read only memory)\n", a, v);
  }
}


/**
 * Write a short.
 */
void Dn_ROM_Chunk::set16(unsigned int a, unsigned short v, char flags) {
  if (flags&DN_MEM_PATCH) {
    if (!pPatchedData) {
      pPatchedData = (unsigned char*)malloc(pSize);
      memcpy(pPatchedData, pData, pSize);
    }
    unsigned char *dst = pPatchedData + (a-pStart);
    *dst++ = v>>8;
    *dst   = v;
  } else {
    err(); printf("invalid write access at %08x (%04x) (read only memory)\n", a, v);
  }
}


/**
 * Write a word.
 */
void Dn_ROM_Chunk::set32(unsigned int a, unsigned int v, char flags) {
  if (flags&DN_MEM_PATCH) {
    if (!pPatchedData) {
      pPatchedData = (unsigned char*)malloc(pSize);
      memcpy(pPatchedData, pData, pSize);
    }
    unsigned char *dst = pPatchedData + (a-pStart);
    *dst++ = v>>24;
    *dst++ = v>>16;
    *dst++ = v>>8;
    *dst   = v;
  } else {
    err(); printf("invalid write access at %08x (%08x) (read only memory)\n", a, v);
  }
}


/**
 * Return a byte at the given address.
 */
unsigned char Dn_ROM_Chunk::get8(unsigned int a, char flags) {
  if ( (flags & DN_MEM_INSTR) && pPatchedData) {
    return pPatchedData[a-pStart];
  } else { 
    return pData[a-pStart];
  }
}


/**
 * Return a short at a given address, even though our CPU 
 * doesn't really support this.
 */
unsigned short Dn_ROM_Chunk::get16(unsigned int a, char flags) {
  unsigned char *src;
  if ( (flags & DN_MEM_INSTR) && pPatchedData) {
    src = pPatchedData + (a-pStart);
  } else {
    src = pData + (a-pStart);
  }
  return (src[0]<<8)|(src[1]);
}


/**
 * Return a word at the given address.
 */
unsigned int Dn_ROM_Chunk::get32(unsigned int a, char flags) {
  unsigned char *src;
  if ( (flags & DN_MEM_INSTR) && pPatchedData) {
    src = pPatchedData + (a-pStart);
  } else {
    src = pData + (a-pStart);
  }
  return (src[0]<<24)|(src[1]<<16)|(src[2]<<8)|(src[3]);
}

/**
 * Load this segment from a binary dump or from an ARM .aif file
 */
char Dn_ROM_Chunk::load(const char *filename, unsigned int offset) {
  int i;
  if (!filename || !*filename) 
    return 0;
  FILE *fin = fopen(filename, "rb");
  if (!fin) {
    printf("DyneE5: %s at 0x%08x\n", filename, offset);
    perror("DyneE5: can't open memory file");
    return 0;
  }
  memset(pData, 0, pSize);
  // check if this is an aif file (if it is, "offset" is ignored)
  unsigned int aif[32];
  fread(aif, 32, 4, fin);
  for (i=0; i<32; i++) aif[i] = ntohl(aif[i]);
  if (aif[0]==0xe1a00000) {                   // if it is a NOP instruction, read the uncompressed aif file
    fread(pData, aif[5], 1, fin);
  } else {                                    // read the uncompressed binary image
    fseek(fin, offset, SEEK_SET);
    fread(pData, pSize, 1, fin);
  }
  fclose(fin);
  if (pPatchedData) {
    free(pPatchedData);
    pPatchedData = 0L;
  }
  return 1;
}


/*--------------------------------*/

Dn_GPIO_Chunk::Dn_GPIO_Chunk(unsigned int start, unsigned int size)
: Dn_Memory_Chunk(start, size) {
}

unsigned char  Dn_GPIO_Chunk::get8 (unsigned int a, char peek) {
  err(); printf("invalid read access at %08x (%02x) (GPIO byte access)\n", a, 0);
  return 0;
}
unsigned short Dn_GPIO_Chunk::get16(unsigned int a, char peek) {
  err(); printf("invalid read access at %08x (%04x) (GPIO halfword access)\n", a, 0);
  return 0;
}
void Dn_GPIO_Chunk::set8 (unsigned int a, unsigned char v,  char patch) {
  err(); printf("invalid write access at %08x (%02x) (GPIO byte access)\n", a, v);
}
void Dn_GPIO_Chunk::set16(unsigned int a, unsigned short v, char patch) {
  err(); printf("invalid write access at %08x (%04x) (GPIO halfword access)\n", a, v);
}


// Interrupt 0x00000020 jumps to TimerInterruptHandler
/*
const unsigned int GPIO_TMR  = 0x0f181800;
const unsigned int GPIO_TM0  = 0x0f182000;
const unsigned int GPIO_TM1  = 0x0f182400;
const unsigned int GPIO_TM2  = 0x0f182800;
const unsigned int GPIO_TM3  = 0x0f182c00;
const unsigned int GPIO_IR   = 0x0f183000;
const unsigned int GPIO_IACK = 0x0f183800;
*/

char t_match(unsigned int t, unsigned int m) {
  if (m==0) return 0;
  // FIXME: this does not handle timer wrap around!
  if (m<=t && m>t-delta_t) return 1;
  return 0;
}

void Dn_GPIO_Chunk::update_cycle() {
  unsigned int t = get_tmr();
  unsigned int m;
  unsigned int IR = get32(GPIO_IR, DN_MEM_PEEK), IR_prev = IR;
  // check timers
  if (t_match(t, get_tm0()))
    IR |= 0x08;
  if (t_match(t, get_tm1()))
    IR |= 0x10;
  if (t_match(t, get_tm2()))
    IR |= 0x20;
  if (t_match(t, get_tm3()))
    IR |= 0x40;
  // check real time alarm
  // : Does the real-time alarm actually trigger an interrupt? If so, which one?
  // update the IR (Interrupt Request) register
  if (IR!=IR_prev) 
    set32(GPIO_IR, IR, DN_MEM_PEEK);
  // now trigger all interrupts that are permitted by ICR with the CPU in usr mode
  if (IR && ((cpu->state->IFFlags&2)==0)) {
    unsigned int ICR = get32(0x0f183400, DN_MEM_PEEK);
    unsigned int FM  = get32(0x0f183c00, DN_MEM_PEEK);
    if (IR & ICR & FM) {
      cpu->dump_registers();
      //ARMul_Abort(cpu->state, ARMul_FIQV);
      cpu->state->NfiqSig = LOW;
    } else if (IR & ICR & ~FM) { // the "else" makes sure that we won't trigger both interrupts
      cpu->dump_registers();
      //ARMul_Abort(cpu->state, ARMul_IRQV);
      cpu->state->NirqSig = LOW;
    }
  }
}

unsigned int Dn_GPIO_Chunk::get_tmr() { return get32(GPIO_TMR, DN_MEM_PEEK); }
unsigned int Dn_GPIO_Chunk::get_tm0() { return get32(GPIO_TM0, DN_MEM_PEEK); }
unsigned int Dn_GPIO_Chunk::get_tm1() { return get32(GPIO_TM1, DN_MEM_PEEK); }
unsigned int Dn_GPIO_Chunk::get_tm2() { return get32(GPIO_TM2, DN_MEM_PEEK); }
unsigned int Dn_GPIO_Chunk::get_tm3() { return get32(GPIO_TM3, DN_MEM_PEEK); }
unsigned int Dn_GPIO_Chunk::get_ir()  { return get32(GPIO_IR,  DN_MEM_PEEK); }

static const char *get_msg(unsigned int addr) {
  const char *ret = 0L;
  switch (addr) {
      // BasicBusControlRegInit, DiagBootStub, TCardSocket::Init
      // u:00000000
    case 0x0f001000: ret = "?? fixed: 00000000"; break; // r w
      // DiagBootStub
      // 40400040
    case 0x0f001800: ret = "?? fixed: 40400040"; break; // r w
      // DiagBootStub
      // 00000000
    case 0x0f001c00: ret = "?? fixed: 00000000"; break; // r w
      // DiagBootStub
      // u:00000000
    case 0x0f002000: ret = "?"; break; // . w
      // u:00000000
    case 0x0f041000: ret = "?"; break; // . w
      // TBIOInterface::BIOFIFOWriteCommand
      // u:00000000
    case 0x0f041800: ret = "?"; break; // . w
      // DiagBootStub
      // u:00000000
    case 0x0f043000: ret = "?"; break; // . w
      // DiagBootStub
      // u:00000000
    case 0x0f043800: ret = "?"; break; // . w
      // DiagBootStub, TBIOInterface::Init, TBIOInterface::BIOWriteCommand,
      // TBIOInterface::BIOWriteCommand, PCirrusSoundDriver::New
      // TBIOInterface::WaitBIOStatus
      // also tested in InitADC
      // u:00000000
    case 0x0f048000: ret = "?"; break; // r w
      // u:00000000
    case 0x0f04a000: ret = "?"; break; // r .
      // u:00000000
    case 0x0f050000: ret = "?"; break; // r w
      // u:00000000
    case 0x0f050800: ret = "?"; break; // r w
      // u:00000000
    case 0x0f052800: ret = "?"; break; // . w
      // TBIOInterface::BIOWriteCommand
      // u:00000000
    case 0x0f052c00: ret = "?"; break; // r w
      // TBIOInterface::BIOReadCommand
      // u:00000000
    case 0x0f053000: ret = "?"; break; // r w
      // TBIOInterface::BIOWriteCommand, TBIOInterface::BIOReadCommand
      // u:00000000
    case 0x0f053400: ret = "?"; break; // r w
      // TBIOInterface::BIOWriteCommand, TBIOInterface::BIOReadCommand
      // u:00000000
    case 0x0f053800: ret = "?"; break; // r w
      // TBIOInterface::BIOReadCommand
      // u:00000000
    case 0x0f053c00: ret = "?"; break; // r .
      // TBIOInterface::BIOWriteCommand
      // u:00000000
    case 0x0f054000: ret = "?"; break; // . w
      // TBIOInterface::BIOWriteCommand
      // u:00000000
    case 0x0f054400: ret = "?"; break; // r w
      // TBIOInterface::BIOWriteCommand
      // u:00000000
    case 0x0f054800: ret = "?"; break; // r w
      // TBIOInterface::BIOWriteCommand
      // u:00000000
    case 0x0f054c00: ret = "?"; break; // r w
      // TBIOInterface::BIOWriteCommand
      // u:00000000
    case 0x0f055000: ret = "?"; break; // . w
      // 00000000 Read DMA Register 1.3 for channel 3
    case 0x0f086c00: ret = "DMA R1.3 Ch3"; break; // . w
      // PCirrusSoundDriver::PowerOutputOff
      // 00000000 Read DMA Register 1.3 for channel 5
    case 0x0f08ac00: ret = "DMA R1.3 Ch5"; break; // . w
      // TDMAManager::Init, TDMAManager::RequestAssignement
      // 0000046c Read DMA Assignment register
    case 0x0f08fc00: ret = "DMA Assignment Register"; break; // r w
      // TSerialDMAEngine::Init
      // 00000000 Read DMA Register 2.0 for channel 0
    case 0x0f090000: ret = "DMA R2.0 Ch0"; break; // . w
      // TSerialDMAEngine::Init
      // 00000000 Read DMA Register 2.0 for channel 1
    case 0x0f091000: ret = "DMA R2.0 Ch1"; break; // . w
      // TSerialDMAEngine::Init
      // 00000000 Read DMA Register 2.0 for channel 2
    case 0x0f092000: ret = "DMA R2.0 Ch2"; break; // . w
      // PCirrusSoundDriver::New
      // 00000000 Read DMA Register 2.0 for channel 3
    case 0x0f093000: ret = "DMA R2.0 Ch3"; break; // . w
      // PCirrusSoundDriver::New
      // 00000000 Read DMA Register 2.3 for channel 3
    case 0x0f093c00: ret = "DMA R2.3 Ch3"; break; // . w
      // 00000000 Read DMA Register 2.3 for channel 4
    case 0x0f094c00: ret = "DMA R2.3 Ch4"; break; // . w
      // PCirrusSoundDriver::New
      // 00000000 Read DMA Register 2.0 for channel 5
    case 0x0f095000: ret = "DMA R2.0 Ch5"; break; // . w
      // PCirrusSoundDriver::New
      // 00000000 Read DMA Register 2.3 for channel 4
    case 0x0f095c00: ret = "DMA R2.3 Ch3"; break; // . w
      // TSerialDMAEngine::Init
      // 00000000 Read DMA Register 2.0 for channel 6
    case 0x0f096000: ret = "DMA R2.0 Ch5"; break; // . w
      // TSerialDMAEngine::Init
      // 00000000 Read DMA Register 2.0 for channel 7
    case 0x0f097000: ret = "DMA R2.0 Ch5"; break; // . w
      // PCirrusSoundDriver::StopOutput
      // 00000000 Read DMA Status Register
    case 0x0f098000: ret = "DMA Status"; break; // r .
      // TDMAManager::Init, PCirrusSoundDriver::StopOutput
      // u:00000000
    case 0x0f098400: ret = "?"; break; // . w
      // BasicBusControlRegInit, DiagBootStub, TVoyagerPlatform::PauseSystem
      //   Boot History:
      // initialized to 0x0881 in BasicBusControlRegInit<-ROMBoot
      //  then set to 0x0800, 0x0fc7, read, 0x0480, read, 0x0800, etc. in DiagBootStub
      //  then the RAM is initialized
      //  then read, 0x0480, read, 0x0800.
      //  then ROM checksum 
      // reading, then writing regularly in TVoyagerPlatform::PauseSystem (bit 0)
      //  then writing 0f110400
      //   called from PauseStstemKernelGlue()
      //    called via SWI from SleepTask()  
      // u:00000000
    case 0x0f110000: ret = "Power (bit0=screen?, bit2=serial?)"; break; // r w
      // BasicBusControlRegInit (0x90), DiagBootStub, TVoyagerPlatform::PauseSystem
      // 00000090
    case 0x0f110400: ret = "?? fixed: 00000090"; break; // r w
      // ROMBoot sets it to 0x07e6, followed by a short timer delay (0x19500), then
      // setting it to 0 again
      // u:00000000
    case 0x0f111400: ret = "?? fixed: 0"; break; // . w
      // u:00000000
    case 0x0f140000: ret = "?"; break; // . w
      // u:00000000
    case 0x0f140400: ret = "?"; break; // . w
      // u:00000000
    case 0x0f140800: ret = "?"; break; // . w
      // u:00000000
    case 0x0f140c00: ret = "?"; break; // . w
      // u:00000000
    case 0x0f141000: ret = "?"; break; // . w
      // u:00000000
    case 0x0f141400: ret = "?"; break; // . w
      // u:00000000
    case 0x0f141800: ret = "?"; break; // . w
      // u:00000000
    case 0x0f141c00: ret = "?"; break; // . w
      // u:00000000
    case 0x0f142000: ret = "?"; break; // . w
      // u:00000000
    case 0x0f142400: ret = "?"; break; // . w
      // u:00000000
    case 0x0f142800: ret = "?"; break; // . w
      // u:00000000
    case 0x0f142c00: ret = "?"; break; // . w
      // u:00000000
    case 0x0f143000: ret = "?"; break; // . w
      // BasicBusControlRegInit
      // u:00000000
    case 0x0f180400: ret = "?? fixed: 0"; break; // . w
      // InitGlobalsThatLiveAcrossReboot, AdjustRealTimeClock, GetRealTimeClock
    case 0x0f181000: ret = 0L; /*"RTC (Real Time)"*/; break; // r .
    case 0x0f181400: ret = 0L; /*"Alm (Alarm Time)"*/; break;
      // Fast Timer, DiagBootStub
    case 0x0f181800: ret = 0L; /*TMR:"Fast Timer";*/; break; // r .
    case 0x0f182000: ret = 0L; /*"TM0"*/; break; // r .
    case 0x0f182400: ret = 0L; /*"TM1"*/; break; // r .
      // SetAlarm1
    case 0x0f182800: ret = 0L; /*"TM2"*/; break; // r w
      // StartScheduler(4b8a7c8f), PreEmptiveTimerInterrruptHandler(4c188df4)
    case 0x0f182c00: ret = 0L; /*"TM3"*/; break; // r w
      // PostCGlobalsHWInit, DispatchIRQInterrupt
    case 0x0f183000: ret = 0L; /*"IR (Pending Interrupts)"*/; break; // r w
      // _EnterFIQAtomicFast(w0c400000), _EnterAtomic
      // _ExitFIQAtomic(w0c400000), PublicExitAtomic, SetAndClearBitsAtomic
    case 0x0f183400: ret = 0L; /*"ICR (Mask of Interrupts that may trigger)"*/; break; // r w
      // DisableAllInterrupts(wffffffff), ClearInterrupt, DispatchIRQInterrupt
    case 0x0f183800: ret = 0L; /*"IRQ Ack (set bits clear IR bits)"*/; break; // . w
      // _EnterAtomic
    case 0x0f183c00: ret = 0L; /*"FM (FIQ Interrupt Mask)"*/; break; // r w
      // one of the registers below might determine, which interrupt wakes up the CPU
      // DisableAllInterrupts(w0c400000), SetAlarm1(rw)
    case 0x0f184000: ret = 0L; /*"IC1 (?)"*/; break; // r w
      // DisableAllInterrupts(w0c000000)
    case 0x0f184400: ret = 0L; /*"IC2 (?)"*/; break; // r w
      // DisableAllInterrupts(w00400000)
    case 0x0f184800: ret = 0L; /*"IC3 (?)"*/; break; // r w
      // u:00000000
    case 0x0f184c00: ret = "?"; break; // r .
      // u:00000000
    case 0x0f185000: ret = "?"; break; // . w
      // DiagBootStub, TGPIOInterface::Init
      // 00000001
    case 0x0f18c400: ret = "? (GPIO Int?)"; break; // . w
      // u:00000000
    case 0x0f18c800: ret = "? (GPIO Int Ack?)"; break; // . w
      //TGPIOInterface::Init
      // u:00000000
    case 0x0f18cc00: ret = "? (GPIO Int?)"; break; // . w
      // TGPIOInterface::Init
      // u:00000000
    case 0x0f18d000: ret = "? (GPIO Int?)"; break; // . w
      // TGPIOInterfaec::ReadGPIOData
      // u:00000000
    case 0x0f18d400: ret = "? (user wants cold boot if 0)"; break; // r .
      // DiagBootStub, TGPIOInterface::Init
      // u:00000000
    case 0x0f18d800: ret = "? (GPIO Int?)"; break; // . w
      // DiagBootStub, TGPIOInterface::Init
      // u:00000000
    case 0x0f18dc00: ret = "? (GPIO Pullups?)"; break; // . w
      // DiagBootStub, TGPIOInterface::Init
      // u:00000000
    case 0x0f18e000: ret = "? (GPIO Polarity?)"; break; // . w
      // DiagBootStub, EarlyIOPowerOn, TGPIOInterface::Init
      // u:00000000
    case 0x0f18e800: ret = "? (GPIO Direction?)"; break; // r w
      // DiagBootStub, EarlyIOPowerOn, EarlyIOPowerOff, TGPIOInterface::Init
      // TGPIOInterface::WriteGPIOData(r), TVoyagerPlatform::PowerOffSrc5V
      // u:00000000
    case 0x0f18ec00: ret = " (GPIO Output Data?)"; break; // r w
      // byte access read: TGeoPortDebugLink::BeaconDetect(long) 
      // ( called from install, InitSerialDebugging. InitializeCommHardware: ea310, 
      //   InitializeCommManager)
      // 8: initialised, 80: can write, 40: can read
    case 0x0f1c4400: ret = "TGeoPortStatus"; break; 
      // TGeoPort::PutByte()
    case 0x0f1c6000: ret = "TGeoPortWriteByte"; break; // writing text debug information here
      // TGeoPort::GetNextByte()
    case 0x0f1c6700: ret = "TGeoPortReadByte"; break; // writing text debug information here
      // BYTE ACCESS WRITE: DiagBootStub
      // u:00000000 "serial bank" 
    case 0x0f1f2800: ret = "[mdem] serial port register 2800"; break; // r w
      // DataAbortHandler (TCardSerevr, TCardDomains)
      // 00000000 
    case 0x0f240000: ret = "?? fixed: 00000000"; break; // r .
      // 00000000
    case 0x0f240800: ret = "?? fixed: 00000000"; break; // r .
      // DiagBootStub, TBankControlRegister::SetBankControlRegister
      // 00000000
    case 0x0f241000: ret = 0L; /*"Flash RAM Bank Control"*/; break; // r w
      // DiagBootStub
      // u:00000000
    case 0x0f241800: ret = "?"; break; // . w
      // One of the lower 24 bits seems to be set on a Warm Reset
      // GetRebootReason, GetUnsuccessfulBootCount, DiagBootStub, SetRebootReason
      // SetUnseccessfulBootCount
      // u:00000000
    case 0x0f242400: ret = "?"; break; // r w : 
      // TSerialNumberROM::Init (reading serial data??)
      // 00000000
    case 0x0f243000: ret = "?? fixed: 00000000"; break; // r w
      // DiagBootStub
      // u:00000000
    case 0x0f247000: ret = "?"; break; // . w
      // BasicBusControlRegInit (set 0xc044, then 0x6043 if StrongARM)), DiagBootStub
      // u:00000000
    case 0x0f280000: ret = "?"; break; // . w
      // BasicBusControlRegInit, DiagBootStub
      // u:00000000
    case 0x0f280400: ret = "?"; break; // . w
      // BasicBusControlRegInit
      // u:00000000
    case 0x0f280800: ret = "?"; break; // . w
      // BasicBusControlRegInit, DiagBootStub
      // u:00000000
    case 0x0f283000: ret = "?"; break; // . w
      // DiagBootStub
      // u:00000000
    case 0x0f283400: ret = "?"; break; // . w
    default: ret = "unknown"; break;
  }
  return ret;
}

unsigned int Dn_GPIO_Chunk::get32(unsigned int addr, char flags) {
  unsigned int ret = Dn_Memory_Chunk::get32(addr, flags);
  add_read(addr); // log this access
  const char *msg = get_msg(addr);
  switch (addr) {
    case GPIO_TMR: ret = cpu->nInstr() * delta_t; break;
    case 0x0f001000: ret = 0x00000000; break;
    //case 0x0f001800: ret = 0x40400040; break;
    case 0x0f110000: ret = 0x00000000; break;
    case 0x0f242400: ret = 0x00000000; break;
    case 0x0f181000: ret = cpu->nInstr()/delta_s + 2082844854; break;
    //case 0x0f18d400: ret = 0x00000001; break; // avoid cold boot?
    case 0x0f18d400: ret = 0x00000000; break; // do cold boot and erase all RAM
  }
  if (msg && !(flags&DN_MEM_PEEK)) {
    err(); printf("read access at %08x:%08x (GPIO: %s)\n", addr, ret, msg);
  }
  return ret;
}

void Dn_GPIO_Chunk::set32(unsigned int addr, unsigned int v, char flags) {
  Dn_Memory_Chunk::set32(addr, v, flags);
  add_write(addr); // log this access
  unsigned int t;
  const char *msg = get_msg(addr);
  switch (addr) {
    case 0x0f183800:
      t = Dn_Memory_Chunk::get32(GPIO_IR, DN_MEM_PEEK); // IRQ Pending
      Dn_Memory_Chunk::set32(GPIO_IR, t&~v, DN_MEM_PEEK);
      break;
  }
  if (msg && !(flags&DN_MEM_PEEK)) {
    err(); printf("write access at %08x:%08x (GPIO: %s)\n", addr, v, msg);
  }
}

void Dn_GPIO_Chunk::add_read(unsigned int addr) {
  Dn_GPIO_Access::iterator ac = access.find(addr);
  if (ac==access.end()) {
    access.insert(std::make_pair(addr, 1));
  } else {
    ac->second |= 1;
  }
}

void Dn_GPIO_Chunk::add_write(unsigned int addr) {
  Dn_GPIO_Access::iterator ac = access.find(addr);
  if (ac==access.end()) {
    access.insert(std::make_pair(addr, 2));
  } else {
    ac->second |= 2;
  }
}

char Dn_GPIO_Chunk::save(const char *filename) {
  FILE *f = fopen(filename, "wb");
  Dn_GPIO_Access::iterator ac = access.begin();
  for ( ; ac != access.end(); ++ac) {
    char r = (ac->second&1) ? 'r' : '.';
    char w = (ac->second&2) ? 'w' : '.';
    fprintf(f, "%08x : %c %c\n", ac->first, r, w);
  }
  fclose(f);
  return 0;
}

/*--------------------------------*/


/**
 * Initialize Flash based memory.
 */
Dn_Flash_Chunk::Dn_Flash_Chunk(unsigned int start, unsigned int size)
: Dn_Memory_Chunk() {
  pStart = start;
  pSize = size;
  chip_hi = chip_lo = 0L;
  // Our current implementation emulates two 28F016 Flash RAMs
  // (currently only one RAM for simplicity and speed)
  //chip_hi = new Dn_Flash_28F016("_hi"); 
  chip_lo = new Dn_Flash_28F016("_lo");
}


/**
 * Return the resources.
 */
Dn_Flash_Chunk::~Dn_Flash_Chunk() {
  delete chip_hi;
  delete chip_lo;
}


/**
 * Disallow byte access
 */
unsigned char  Dn_Flash_Chunk::get8 (unsigned int a, char flags) {
  err(); printf("invalid read access at %08x (%02x) (Flash byte access)\n", a, 0);
  return 0;
}


/**
 * Disallow short access
 */
unsigned short Dn_Flash_Chunk::get16(unsigned int a, char flags) {
  err(); printf("invalid read access at %08x (%04x) (Flash halfword access)\n", a, 0);
  return 0;
}


/**
 * Disallow byte access
 */
void Dn_Flash_Chunk::set8 (unsigned int a, unsigned char v,  char flags) {
  err(); printf("invalid write access at %08x (%02x) (Flash byte access)\n", a, v);
}


/**
 * Disallow short access
 */
void Dn_Flash_Chunk::set16(unsigned int a, unsigned short v, char flags) {
  err(); printf("invalid write access at %08x (%04x) (Flash halfword access)\n", a, v);
}


/**
 * Word access is a bit more complicated.
 */
unsigned int Dn_Flash_Chunk::get32(unsigned int addr, char flags) {
  unsigned int bankControl = mem->get32(0x0f241000, 1);
  //printf("Bank control = %08x\n", bankControl);
  unsigned int chip_addr;
  unsigned int hi;
  unsigned int lo;
  switch (bankControl & 0x0700) {
    case 0x0300:
      chip_addr = (addr-pStart)>>1;
      hi = chip_lo ? chip_lo->read(chip_addr)<<16 : 0;
      lo = chip_lo ? chip_lo->read(chip_addr+1) : 0;
      break;
    default:
      chip_addr = (addr-pStart)>>2;
      hi = chip_hi ? chip_hi->read(chip_addr)<<16 : 0;
      lo = chip_lo ? chip_lo->read(chip_addr) : 0;
      break;
  }
  return hi | lo;
}


/**
 * Word access is a bit more complicated.
 */
void Dn_Flash_Chunk::set32(unsigned int addr, unsigned int v, char flags) {
  unsigned int chip_addr = (addr-pStart)>>2;
  if (chip_hi) chip_hi->write(chip_addr, v>>16);
  if (chip_lo) chip_lo->write(chip_addr, v);
}


/**
 * Load the content of the Flash from an external file.
 */
char Dn_Flash_Chunk::load(const char *filename, unsigned int offset) {
  if (!filename || !*filename) 
    return 0;
  FILE *f = fopen(filename, "rb");
  if (f) {
    read(f);
    fclose(f);
  } else {
    printf("ERROR: Can't load flash file %s\n", filename);
  }
  return 1;
}


/**
 * Save the content of the Flash to an external file.
 */
char Dn_Flash_Chunk::save(const char *filename) {
  FILE *f = fopen(filename, "wb");
  if (f) {
    write(f);
    fclose(f);
  } else {
    printf("ERROR: Can't save flash memory dump to '%s'\n", filename);
    perror("ERROR");
  }
  return 1;
}


/**
 * Call every RAM chip and have it write its contents.
 */
void Dn_Flash_Chunk::write(FILE *f) {
  if (chip_hi) chip_hi->write(f);
  if (chip_lo) chip_lo->write(f);
}


/**
 * Call every RAM chip and have it read its contents.
 */
void Dn_Flash_Chunk::read(FILE *f) {
  if (chip_hi) chip_hi->read(f);
  if (chip_lo) chip_lo->read(f);
}

/*--------------------------------*/



Dn_Screen_Chunk::Dn_Screen_Chunk(unsigned int start, unsigned int size)
: Dn_Memory_Chunk(start, size) 
{
  pBitmap = (unsigned char*)calloc(320, 480*3);
  pImage = new Fl_RGB_Image(pBitmap, 320, 480, 3);
}

// Screen is 320x480 pixels
unsigned char *Dn_Screen_Chunk::base(unsigned int addr, unsigned int flags) {
  addr -= pStart;
  int x = 319 - addr/240;
  int y = (addr%240)*2;
  //err(); printf("SCREEN write access at 0x%08x (%dx%d, %08x)) ----\n", addr, x, y, v);
  if (x<0||x>=320) return 0L;
  if (y<0||y>=473) return 0L;
  int offset = 3 * (x+320*y);
  if (offset<0 || offset>320*480*3) return 0L;
  return pBitmap + offset;
}

void Dn_Screen_Chunk::set8(unsigned int addr, unsigned char v, char flags) {
  Dn_Memory_Chunk::set8(addr, v, flags);
  unsigned char b, *dst = base(addr, v);
  int d = 3*320;
  if (!dst) return;
  v = v^0xff;
  b = (v    )&0xf0; dst[0] = dst[1] = dst[2] = b; dst += d;
  b = (v<< 4)&0xf0; dst[0] = dst[1] = dst[2] = b; dst += d;
  redraw();
}

void Dn_Screen_Chunk::set16(unsigned int addr, unsigned short v, char flags) {
  Dn_Memory_Chunk::set16(addr, v, flags);
  unsigned char b, *dst = base(addr, v);
  int d = 3*320;
  if (!dst) return;
  v = v^0xffff;
  b = (v>> 8)&0xf0; dst[0] = dst[1] = dst[2] = b; dst += d;
  b = (v>> 4)&0xf0; dst[0] = dst[1] = dst[2] = b; dst += d;
  b = (v    )&0xf0; dst[0] = dst[1] = dst[2] = b; dst += d;
  b = (v<< 4)&0xf0; dst[0] = dst[1] = dst[2] = b; dst += d;
  redraw();
}

void Dn_Screen_Chunk::set32(unsigned int addr, unsigned int v, char flags) {
  Dn_Memory_Chunk::set32(addr, v, flags);
  unsigned char b, *dst = base(addr, v);
  int d = 3*320;
  if (!dst) return;
  v = v^0xffffffff;
  b = (v>>24)&0xf0; dst[0] = dst[1] = dst[2] = b; dst += d;
  b = (v>>20)&0xf0; dst[0] = dst[1] = dst[2] = b; dst += d;
  b = (v>>16)&0xf0; dst[0] = dst[1] = dst[2] = b; dst += d;
  b = (v>>12)&0xf0; dst[0] = dst[1] = dst[2] = b; dst += d;
  b = (v>> 8)&0xf0; dst[0] = dst[1] = dst[2] = b; dst += d;
  b = (v>> 4)&0xf0; dst[0] = dst[1] = dst[2] = b; dst += d;
  b = (v    )&0xf0; dst[0] = dst[1] = dst[2] = b; dst += d;
  b = (v<< 4)&0xf0; dst[0] = dst[1] = dst[2] = b; dst += d;
  redraw();
}

void Dn_Screen_Chunk::redraw() {
  pImage->uncache();
  if (wScreen)
    wScreen->redraw();
}


/*--------------------------------*/


/**
 * Create a memory manager.
 *
 * This class represents system memory. It can handle mutiple blocks
 * of RAM, ROM, and Flash memory. Access can go through an optional
 * MMU.
 */
Dn_Memory::Dn_Memory() 
: pNChunk(0), pInstr(0) {
}


/**
 * Add a new chunk of memory to the manager.
 */
void Dn_Memory::add_chunk(Dn_Memory_Chunk *mc) {
  pChunk[pNChunk] = mc;
  pNChunk++;
}


/**
 * Add a new chunk of memory to the manager.
 */
Dn_Memory_Chunk *Dn_Memory::add_chunk(unsigned int start, unsigned int size) {
  Dn_Memory_Chunk *mc = new Dn_Memory_Chunk(start, size);
  add_chunk(mc);
  return mc;
}


/**
 * Find the chunk that contains this address.
 */
Dn_Memory_Chunk *Dn_Memory::find(unsigned int a) {
  for (int i=0; i<pNChunk; i++) {
    if (pChunk[i]->has(a)) {
      return pChunk[i];
    }
  }
  return 0L;
}


/**
 * fix up the flags and adress according to internal settings.
 */
void Dn_Memory::fixup(unsigned int &a, char &flags) {
  if (flags&DN_MEM_VIRT) {
    a = v2p(a);
    flags &= ~DN_MEM_VIRT;
  }
  if (pInstr) {
    flags |= DN_MEM_INSTR;
  }
  if (((flags&DN_MEM_PEEK)==0) && symbols->is_watchpoint(a) && cpu->watchpoints) {
    err(); printf("memory access at watchpoint %08xp\n", a);
    cpu->stop();
  }
}


/**
 * Set a byte at the specified address.
 */
void Dn_Memory::set8(unsigned int a, unsigned char v, char flags) {
  fixup(a, flags);
  Dn_Memory_Chunk *mc = find(a);
  if (mc) {
    mc->set8(a, v, flags);
  } else if ((flags&DN_MEM_PEEK)==0) {
    err(); printf("invalid write access at %08x (%02x) (no memory)\n", a, v);
  }
}


/**
 * Set a halfword at the specified address.
 */
void Dn_Memory::set16(unsigned int a, unsigned short v, char flags) {
  fixup(a, flags);
  Dn_Memory_Chunk *mc = find(a);
  if (mc) {
    mc->set16(a, v, flags);
  } else if ((flags&DN_MEM_PEEK)==0) {
    err(); printf("invalid write access at %08x (%04x) (no memory)\n", a, v);
  }
}


/**
 * Set a word at the specified address.
 */
void Dn_Memory::set32(unsigned int a, unsigned int v, char flags) {
  fixup(a, flags);
  Dn_Memory_Chunk *mc = find(a);
  if (mc) {
    mc->set32(a, v, flags);
  } else if ((flags&DN_MEM_PEEK)==0) {
    err(); printf("invalid write access at %08x (%08x) (no memory)\n", a, v);
  }
}



/**
 * Get a byte at the specified address.
 */
unsigned char Dn_Memory::get8(unsigned int a, char flags) {
  fixup(a, flags);
  Dn_Memory_Chunk *mc = find(a);
  if (mc) {
    return mc->get8(a, flags);
  } else if ((flags&DN_MEM_PEEK)==0) {
    err(); printf("invalid read access at %08x (no memory)\n", a);
  }
  return 0;
  return 0xdeadc0de>>((3-(a&3))*8);
}


/**
 * Get a halfword at the specified address.
 */
unsigned short Dn_Memory::get16(unsigned int a, char flags) {
  fixup(a, flags);
  Dn_Memory_Chunk *mc = find(a);
  if (mc) {
    return mc->get16(a, flags);
  } else if ((flags&DN_MEM_PEEK)==0) {
    err(); printf("invalid read access at %08x (no memory)\n", a);
  }
  return 0;
  return 0xdeadc0de>>((1-(a&1))*16);
}


/**
 * Get a word at the specified address.
 */
unsigned int Dn_Memory::get32(unsigned int a, char flags) {
  fixup(a, flags);
  Dn_Memory_Chunk *mc = find(a);
  if (mc) {
    return mc->get32(a, flags);
  } else if ((flags&DN_MEM_PEEK)==0) {
    err(); printf("invalid read access at %08x (no memory)\n", a);
  }
  return 0;
  return 0xdeadc0de;
}


/**
 * Convert a virtual address into a physical address.
 */
unsigned int Dn_Memory::v2p(unsigned int a, char peek) {
  ARMword pa;
  mmu_v2p_dbct(cpu->state, a&0xfffffffc, &pa);
  return pa + (a&3);
}

void Dn_Memory::write(FILE *f) {
  for (int i=0; i<pNChunk; i++) {
    pChunk[i]->write(f);
  }
}


void Dn_Memory::read(FILE *f) {
  for (int i=0; i<pNChunk; i++) {
    pChunk[i]->read(f);
  }
}


void Dn_Memory::begin_instr_fetch() {
  pInstr = 1;
}


void Dn_Memory::end_instr_fetch() {
  pInstr = 0;
}

