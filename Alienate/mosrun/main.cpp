//
//  main.cpp
//  mosrun
//
//  Created by Matthias Melcher on 14.07.13.
//  Copyright (c) 2013 Matthias Melcher. All rights reserved.
//

//
// 30/Jul/2013:
//    ARM6asm seems to run
//    ARM6c crashes in an infinite loop
//    ARMCFront misses traps
//      ERROR: unimplemented trap 0x0000A994: _CurResFile
//      ERROR: unimplemented trap 0x0000A9A4: _HomeResFile
//      ERROR: unimplemented trap 0x0000A80D: _Count1Resources
//      ERROR: unimplemented trap 0x0000A80E: _Get1IxResource
//      ERROR: unimplemented trap 0x0000A04A: _HNoPurge
//    ARMCpp seems to run but misses traps:
//      ERROR: unimplemented trap 0x0000A049: _HPurge
//      ERROR: unimplemented trap 0x0000A9A3: _ReleaseResource
//      ERROR: unimplemented trap 0x0000A23C: _CmpStringMarks
//    ARMLink seems to run but misses traps:
//      ERROR: unimplemented trap 0x0000A049: _HPurge
//      ERROR: unimplemented trap 0x0000A9A3: _ReleaseResource
//    DumpAIF seems to run
//    DumpAOF seems to run
//    Makemake seems to run but misses traps:
//      ERROR: unimplemented trap 0x0000A049: _HPurge
//      ERROR: unimplemented trap 0x0000A9A3: _ReleaseResource
//    Rex fails due to missing traps:
//      ERROR: unimplemented trap 0x0000A994: _CurResFile
//      ERROR: unimplemented trap 0x0000A9A4: _HomeResFile
//      ERROR: unimplemented trap 0x0000A122: _NewHandle
//

// TODO: missing traps:
//    UInt32 CmpStringMarks(BytePtr textPtrA, BytePtr textPtrB, UInt32 lengthAB) ??
//    int CountResources(theType: ResType) Given the type, return the number of resources of that type accessable in ALL open maps.
//    int CurResFile() : Returns the reference number of the current resource file.
//    Get1IxResource not documented
//    void HNoPurge(handle h) : make a resource unpurgeable!
//    int HomeResFile(theResource: Handle) : Given a handle, returns the refnum of the resource file that the resource lives in.
//    void HPurge(handle h) : make a resource purgeable but don;t purge it!
//    handle NewHandle() : allocate memory and a master pointer to it
//    void ReleaseResource(handle) : Given handle, releases the resource and disposes of the handle.

/*
 Mac Path Names:
  - separator is a ':'
  - path names starting with a ':' are relative to the current directory
  - path names without a ':' are absolute - the first word is the disk name
  - starting with a '::' sets the parent directory, more ":" go further up the tree
  - if there are no ":" at all in the name, it is a leaf name and it is relative
  - test.c => test.c
  - :test.c => ./test.c
  - ;;test.c => ../test.c
  - :::test.c => ../../test.c
  - :Emaples: => ./Examples/
  - Examples: => /Examples/
  - Examples => would not refer to a directory, but a file "./Examples"
*/

// Inlcude all the required system headers

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/xattr.h>
#include <fcntl.h>

// Include our own interfaces

#include "main.h"
#include "names.h"
#include "log.h"
#include "rsrc.h"
#include "memory.h"

// Inlcude Musahi's m68k emulator

extern "C" {
#include "musashi331/m68k.h"
#include "musashi331/m68kcpu.h"
#include "musashi331/m68kops.h"
}



void installBreakpoints(unsigned int segment, unsigned int segAddr);
const char *printAddr(unsigned int addr);

typedef unsigned char byte;

typedef unsigned int mosHandle;
typedef unsigned int mosResType;
typedef int mosInteger;


const int MOS_STACK_SIZE = 0x8000;


byte *theApp = 0;
ssize_t theAppSize = 0;
byte *theRsrc = 0;
ssize_t theRsrcSize = 0;
byte *theJumpTable = 0;

unsigned int gMosCurrentA5 = 0;
unsigned int gMosCurrentStackBase = 0;
unsigned int gMosCurJTOffset = 0;
unsigned int gMosResLoad = 1;
unsigned int gMosResErr = 0;
unsigned int gMosMemErr = 0;
unsigned int gMosMPWHandle = 0;

const unsigned int kMosApplLimit      = 0x0130;  // Application heap limit
const unsigned int kMosApplZone       = 0x02aa;  // Address of application heap zone
const unsigned int kMosHeapEnd        = 0x0114;  // Address of end of application heap zone
const unsigned int kMosMemTop         = 0x0108;  // Address of end of RAM (on Macintosh XL, end of RAM available to applications)
const unsigned int kMosSysZone        = 0x02a6;  // Address of system heap zone
const unsigned int kMosBufPtr         = 0x010c;  // Address of end of jump table
const unsigned int kMosCurrentA5      = 0x0904;  // Address of boundary between application globals and application parameters
const unsigned int kMosCurJTOffset    = 0x0934;  // Offset to jump table from location pointed to by A5 (word)
const unsigned int kMosCurStackBase   = 0x0908;  // Address of base of stack; start of application globals

const unsigned int kMosRndSeed        = 0x0156;  // random number seed (long)
const unsigned int kMosTicks          = 0x016A;  // ticks since last boot (unsigned long)
const unsigned int kMosDeskHook       = 0x0A6C;  // hook for painting desktop (pointer)
const unsigned int kMosMBarHeight     = 0x0BAA;  // height of menu bar (integer)

unsigned int gResourceStart[20] = { 0 };
unsigned int gResourceEnd[20] = { 0 };

unsigned int trapDispatchTrap = 0;
unsigned int trapExitApp = 0;


void hexDump(unsigned int a, unsigned int n)
{
  int i = 0;
  for (;;) {
    mosLog("%08X: ", a);
    for (i=0; i<16; i++) {
      if (i<n) {
        byte b = *((byte*)a+i);
        mosLog("%02X ", b);
      } else {
        mosLog("   ");
      }
    }
    for (i=0; i<16; i++) {
      if (i<n) {
        byte b = *((byte*)a+i);
        mosLog("%c", (b>=32 && b<127)?b:'.');
      } else {
        mosLog(" ");
      }
    }
    mosLog("\n");
    fflush(stdout);
    if (n<=16) break;
    a += 16;
    n -= 16;
  }
}


void dumpResourceMap()
{
  unsigned int i = 0, j = 0;
  unsigned int rsrcData = m68k_read_memory_32((unsigned int)(theApp));
  // ---- read the map
  unsigned int rsrcMapTypeList = m68k_read_memory_16((unsigned int)(theRsrc+24));
  // ------ resource map type list
  unsigned int rsrcMapTypeListSize = m68k_read_memory_16((unsigned int)(theRsrc+rsrcMapTypeList)) + 1;
  mosLog("  Rsrc Type list at 0x%08X with %d types.\n", rsrcMapTypeList, rsrcMapTypeListSize);
  for (i=0; i<rsrcMapTypeListSize; i++) {
    unsigned int nRes = m68k_read_memory_16((unsigned int)(theRsrc+rsrcMapTypeList+8*i+6)) + 1;
    unsigned int resTable = m68k_read_memory_16((unsigned int)(theRsrc+rsrcMapTypeList+8*i+8)) + rsrcMapTypeList;
    mosLog("    %d: Rsrc type '%c%c%c%c' has %d resouces listed at 0x%08X\n",
           i,
           m68k_read_memory_8((unsigned int)(theRsrc+rsrcMapTypeList+8*i+2)),
           m68k_read_memory_8((unsigned int)(theRsrc+rsrcMapTypeList+8*i+3)),
           m68k_read_memory_8((unsigned int)(theRsrc+rsrcMapTypeList+8*i+4)),
           m68k_read_memory_8((unsigned int)(theRsrc+rsrcMapTypeList+8*i+5)),
           nRes, resTable
           );
    for (j=0; j<nRes; j++) {
      unsigned int data = (m68k_read_memory_32((unsigned int)(theRsrc+resTable+12*j+4)) & 0xffffff);
      mosLog("      %d: ID=%d, name@%d, data=0x%08x, loaded=0x%0X, flags=0x%02X, %d bytes\n", j,
             m68k_read_memory_16((unsigned int)(theRsrc+resTable+12*j+0)),
             m68k_read_memory_16((unsigned int)(theRsrc+resTable+12*j+2)),
             data,
             m68k_read_memory_32((unsigned int)(theRsrc+resTable+12*j+8)),
             m68k_read_memory_8((unsigned int)(theRsrc+resTable+12*j+4)),
             m68k_read_memory_32((unsigned int)(theApp+rsrcData+data))
             );
      // 0x02: write to resource file
      // 0x04: preload (but when and how?)
      // 0x08: protected
      // 0x10: locked
      // 0x20: purgeable
      // 0x40: system heap
      m68k_write_memory_32((unsigned int)(theRsrc+resTable+12*j+8), 0); // FIXME: ugly hack!
    }
  }
  //unsigned int rsrcMapNameList = m68k_read_memory_16(rsrcMap + 26);

}


// Finds and loads the given resource, and returns a handle to it
// Resource Data in Memory:
//   Master pointer: 4 bytes
//   Size: 4 bytes
//   Content: n bytes
mosHandle GetResource(unsigned int myResType, unsigned short myId)
{
  unsigned int i = 0, j = 0;
  // ---- read the map
  unsigned int rsrcMapTypeList = m68k_read_memory_16((unsigned int)(theRsrc+24));
  // ------ resource map type list
  unsigned int rsrcMapTypeListSize = m68k_read_memory_16((unsigned int)(theRsrc+rsrcMapTypeList)) + 1;
  for (i=0; i<rsrcMapTypeListSize; i++) {
    unsigned int nRes = m68k_read_memory_16((unsigned int)(theRsrc+rsrcMapTypeList+8*i+6)) + 1;
    unsigned int resTable = m68k_read_memory_16((unsigned int)(theRsrc+rsrcMapTypeList+8*i+8)) + rsrcMapTypeList;
    unsigned int resType = m68k_read_memory_32((unsigned int)(theRsrc+rsrcMapTypeList+8*i+2));
    if (resType==myResType) {
      for (j=0; j<nRes; j++) {
        unsigned int id = m68k_read_memory_16((unsigned int)(theRsrc+resTable+12*j+0));
        if (id==myId) {
          unsigned int handle = m68k_read_memory_32((unsigned int)(theRsrc+resTable+12*j+8));
          if (handle) {
            // resource is already in RAM
            mosLog("Resource already loaded\n");
            return handle;
          } else {
            // resource must be copied from the file into memory
            if (gMosResLoad==0) {
              mosLog("WARNING: Automatic Resource loading is disabled!\n");
            }
            mosLog("Resource found, loading...\n");
            unsigned int rsrcOffset = (m68k_read_memory_32((unsigned int)(theRsrc+resTable+12*j+4)) & 0xffffff);
            unsigned int rsrcData = m68k_read_memory_32((unsigned int)(theApp));
            unsigned int rsrcSize = m68k_read_memory_32((unsigned int)(theApp+rsrcData+rsrcOffset));
            
            mosHandle hdl = mosNewHandle(rsrcSize);
            mosPtr ptr = mosRead32(hdl);
            memcpy((void*)ptr, theApp+rsrcData+rsrcOffset+4, rsrcSize);
            // make the resource map point to the resource handle
            m68k_write_memory_32((unsigned int)(theRsrc+resTable+12*j+8), hdl);
            // set breakpoints
            if (myResType=='CODE') {
              if (m68k_read_memory_16((unsigned int)(theApp+rsrcData+rsrcOffset+4))==0xffff) {
                installBreakpoints(myId, (unsigned int)(ptr+4+0x24)); // 0x24
                gResourceStart[myId] = (unsigned int)(ptr+4+0x24);
                gResourceEnd[myId] = (unsigned int)(ptr+4) + rsrcSize;
              } else {
                installBreakpoints(myId, (unsigned int)(ptr+4)); // 0x24
                gResourceStart[myId] = (unsigned int)(ptr+4);
                gResourceEnd[myId] = (unsigned int)(ptr+4) + rsrcSize;
              }
              mosLog("Resource %d from 0x%08X to 0x%08X\n", myId, gResourceStart[myId], gResourceEnd[myId]);
            }
            return hdl;
          }
        }
      }
    }
  }
  mosLog("ERROR: Resource not found!\n");
  return 0;
}


// Finds and loads the given resource, and returns a handle to it
// Resource Data in Memory:
//   Master pointer: 4 bytes
//   Size: 4 bytes
//   Content: n bytes
mosHandle GetNamedResource(unsigned int myResType, const byte *pName)
{
  unsigned int i = 0, j = 0;
  // ---- read the map
  unsigned int rsrcMapTypeList = m68k_read_memory_16((unsigned int)(theRsrc+24));
  unsigned int rsrcMapNameList = m68k_read_memory_16((unsigned int)(theRsrc+26));
  // ------ resource map type list
  unsigned int rsrcMapTypeListSize = m68k_read_memory_16((unsigned int)(theRsrc+rsrcMapTypeList)) + 1;
  for (i=0; i<rsrcMapTypeListSize; i++) {
    unsigned int nRes = m68k_read_memory_16((unsigned int)(theRsrc+rsrcMapTypeList+8*i+6)) + 1;
    unsigned int resTable = m68k_read_memory_16((unsigned int)(theRsrc+rsrcMapTypeList+8*i+8)) + rsrcMapTypeList;
    unsigned int resType = m68k_read_memory_32((unsigned int)(theRsrc+rsrcMapTypeList+8*i+2));
    if (resType==myResType) {
      for (j=0; j<nRes; j++) {
        unsigned short rsrcNameOffset = m68k_read_memory_16((unsigned int)(theRsrc+resTable+12*j+2));
        if (rsrcNameOffset==0xffff) continue; // unnamed resource
        byte* rsrcName = theRsrc+rsrcMapNameList+rsrcNameOffset;
        mosLog("%*s\n", rsrcName[0], rsrcName+1);
        if (memcmp(pName, rsrcName, pName[0]+1)==0) {
          unsigned int handle = m68k_read_memory_32((unsigned int)(theRsrc+resTable+12*j+8));
          unsigned int id = m68k_read_memory_16((unsigned int)(theRsrc+resTable+12*j+0));
          if (handle) {
            // resource is already in RAM
            mosLog("Resource already loaded\n");
            return handle;
          } else {
            // resource must be copied from the file into memory
            if (gMosResLoad==0) {
              mosLog("WARNING: Automatic Resource loading is disabled!\n");
            }
            mosLog("Resource found, loading...\n");
            unsigned int rsrcOffset = (m68k_read_memory_32((unsigned int)(theRsrc+resTable+12*j+4)) & 0xffffff);
            unsigned int rsrcData = m68k_read_memory_32((unsigned int)(theApp));
            unsigned int rsrcSize = m68k_read_memory_32((unsigned int)(theApp+rsrcData+rsrcOffset));
            
            mosHandle hdl = mosNewHandle(rsrcSize);
            mosPtr ptr = mosRead32(hdl);
            memcpy((void*)ptr, theApp+rsrcData+rsrcOffset+4, rsrcSize);
            // make the resource map point to the resource handle
            m68k_write_memory_32((unsigned int)(theRsrc+resTable+12*j+8), hdl);
            // set breakpoints
            if (myResType=='CODE') {
              if (m68k_read_memory_16((unsigned int)(theApp+rsrcData+rsrcOffset+4))==0xffff) {
                installBreakpoints(id, (unsigned int)(ptr+4+0x24)); // 0x24
                gResourceStart[id] = (unsigned int)(ptr+4+0x24);
                gResourceEnd[id] = (unsigned int)(ptr+4) + rsrcSize;
              } else {
                installBreakpoints(id, (unsigned int)(ptr+4)); // 0x24
                gResourceStart[id] = (unsigned int)(ptr+4);
                gResourceEnd[id] = (unsigned int)(ptr+4) + rsrcSize;
              }
              mosLog("Resource %d from 0x%08X to 0x%08X\n", id, gResourceStart[id], gResourceEnd[id]);
            }
            return hdl;
          }
        }
      }
    }
  }
  mosLog("ERROR: Resource not found!\n");
  return 0;
}


unsigned int createA5World(mosHandle hCode0)
{
  // dereference the handle
  unsigned int code0 = m68k_read_memory_32(hCode0);
  // create jump table and space for the app global variables
  unsigned int aboveA5 = m68k_read_memory_32(code0 +  0);
  unsigned int belowA5 = m68k_read_memory_32(code0 +  4);
  unsigned int length  = m68k_read_memory_32(code0 +  8);
  unsigned int offset  = m68k_read_memory_32(code0 + 12);
  // create jump table
  theJumpTable = (byte*)mosNewPtr(aboveA5+belowA5);
  gMosCurJTOffset = offset;
  memcpy(theJumpTable+belowA5+offset, (byte*)(code0+16), length);
  gResourceStart[19] = (unsigned int)(theJumpTable + belowA5);
  gResourceEnd[19] = (unsigned int)(theJumpTable + belowA5 + length);
  return (unsigned int)(theJumpTable + belowA5);
}


void readResourceMap()
{
  unsigned int rsrcMap = m68k_read_memory_32((unsigned int)(theApp+4));
  unsigned int rsrcMapSize = m68k_read_memory_32((unsigned int)(theApp+12));
  mosLog("Rsrc Map %d bytes at 0x%08X\n", rsrcMapSize, rsrcMap);
  theRsrc = (byte*)mosNewPtr(rsrcMapSize);
  theRsrcSize = rsrcMapSize;
  memcpy(theRsrc, theApp+rsrcMap, rsrcMapSize);
  dumpResourceMap();
}


int loadApp(const char *aName)
{
  ssize_t size = getxattr(aName, "com.apple.ResourceFork", 0L, 0, 0, 0);
  if (size==-1) {
    return 0;
  }
  theAppSize = size;
  theApp = (byte*)mosNewPtr(size);
  ssize_t ret = getxattr(aName, "com.apple.ResourceFork", theApp, size, 0, 0);
  if (ret==-1) {
    return 0;
  }
  mosLog("%s has a %ld byte resource fork\n", aName, size);  
  readResourceMap();
  mosHandle code0 = GetResource('CODE', 0);
  if (code0==0) {
    mosLog("CODE 0 not found\n");
    return 0;
  }
  gMosCurrentA5 = createA5World(code0);
  return 1;
}


int loadInternalApp()
{
  if (gAppResource) {
    theAppSize = gAppResourceSize;
    theApp = (byte*)mosNewPtr(theAppSize);
    memcpy(theApp, gAppResource, gAppResourceSize);
    readResourceMap();
    mosHandle code0 = GetResource('CODE', 0);
    if (code0==0) {
      mosLog("CODE 0 not found\n");
      return 0;
    }
    gMosCurrentA5 = createA5World(code0);
    return 1;
  } else {
    return 0;
  }
}

// ----- Breakpoint management


typedef struct Breakpoint {
  struct Breakpoint *next;
  unsigned int segment;
  unsigned int segAddr;
  unsigned int address;
  unsigned short originalCmd;
  const char *text;
} Breakpoint;

Breakpoint *gFirstBreakpoint = 0L;
Breakpoint *gPendingBreakpoint = 0L;


void addBreakpoint(unsigned int segment, unsigned int address, const char *text="")
{
  Breakpoint *bp = (Breakpoint*)calloc(1, sizeof(Breakpoint));
  bp->next = gFirstBreakpoint;
  gFirstBreakpoint = bp;
  bp->segment = segment;
  bp->segAddr = address;
  bp->text = text;
  // set the actual breakpoint when the segment is loaded
}


Breakpoint *findBreakpoint(unsigned int pc)
{
  Breakpoint *bp = gFirstBreakpoint;
  while (bp) {
    if (bp->address==pc)
      break;
    bp = bp->next;
  }
  return bp;
}


void installBreakpoints(unsigned int segment, unsigned int segAddr)
{
  Breakpoint *bp = gFirstBreakpoint;
  while (bp) {
    if (bp->segment == segment) {
      unsigned int dst = segAddr+bp->segAddr;
      bp->address = dst;
      bp->originalCmd = m68k_read_memory_16(dst);
      m68k_write_memory_16(dst, 0xaffe);
    }
    bp = bp->next;
  }
}


// ----- Trap management


typedef void (*TrapNative)(unsigned short);
typedef struct {
  unsigned short cmd;
  TrapNative trapNative;
  unsigned short rts;
} TrapNativeCall;


unsigned short gCurrentTrap = 0;
//extern TrapNativeCall *tncTable[0x1000];
TrapNativeCall **tncTable = 0;


// Load a resource using a fourCC code
//  sp+10 = (old stack)
//  sp+6  = return value
//  sp+2  = rsrc.l
//  sp    = id.w
// TODO: gMosResErr
void trapGetResource(unsigned short instr)
{
  unsigned int sp = m68k_get_reg(0L, M68K_REG_SP);
  unsigned int stack_ret = m68k_read_memory_32(sp); sp += 4;
  unsigned int id   = m68k_read_memory_16(sp); sp+=2;
  unsigned int rsrc = m68k_read_memory_32(sp); sp+=4;
  mosLog("            GetResource('%c%c%c%c', %d)\n",
          rsrc>>24, rsrc>>16, rsrc>>8, rsrc,
          id);
  unsigned int hdl = (unsigned int)GetResource(rsrc, id);
  m68k_write_memory_32(sp, hdl);
  sp-=4; m68k_write_memory_32(sp, stack_ret);
  m68k_set_reg(M68K_REG_D0, 0);
  m68k_set_reg(M68K_REG_SP, sp);
}


// Load a resource using a name
//  sp+12 = (old stack)
//  sp+8  = return value
//  sp+4  = rsrc.l
//  sp    = name.l
// TODO: gMosResErr
void trapGetNamedResource(unsigned short instr)
{
  unsigned int sp = m68k_get_reg(0L, M68K_REG_SP);
  unsigned int stack_ret = m68k_read_memory_32(sp); sp += 4;
  unsigned int name = m68k_read_memory_32(sp); sp+=4;
  unsigned int rsrc = m68k_read_memory_32(sp); sp+=4;
  mosLog("            GetNamedResource('%c%c%c%c', %*s)\n",
          rsrc>>24, rsrc>>16, rsrc>>8, rsrc,
          m68k_read_memory_8(name), (char*)(name+1));
  unsigned int hdl = (unsigned int)GetNamedResource(rsrc, (byte*)name);
  m68k_write_memory_32(sp, hdl);
  m68k_set_reg(M68K_REG_D0, 0);
  sp-=4; m68k_write_memory_32(sp, stack_ret);
  m68k_set_reg(M68K_REG_SP, sp);
}


// 0xa9a5: Size of the resource (on disk?)
// sp    = handle.l
// sp+4  = return value.l
// FIXME: this must return the size of the resource *in the file*, not in memory!
void trapSizeResource(unsigned short instr)
{
  unsigned int sp = m68k_get_reg(0L, M68K_REG_SP);
  unsigned int stack_ret = m68k_read_memory_32(sp); sp += 4;
  unsigned int hdl = m68k_read_memory_32(sp); sp+=4;
  unsigned int ptr = m68k_read_memory_32(hdl);
  unsigned int size = mosPtrSize(ptr);
  
  mosLog("            SizeResource(0x%08X) = %d\n", hdl, size);
  
  m68k_set_reg(M68K_REG_D0, 0);
  m68k_write_memory_32(sp, size);
  sp-=4; m68k_write_memory_32(sp, stack_ret);
  m68k_set_reg(M68K_REG_SP, sp);
}



void trapNewPtrClear(unsigned short) {
  unsigned int size = m68k_get_reg(0L, M68K_REG_D0);
  
  mosLog("            NewPtrClear(%d)\n", size);
  
  unsigned int ptr = mosNewPtr(size);
  
  m68k_set_reg(M68K_REG_A0, ptr);
  m68k_set_reg(M68K_REG_D0, 0);
}


void trapNewPtr(unsigned short) {
  unsigned int size = m68k_get_reg(0L, M68K_REG_D0);
  
  mosLog("            NewPtr(%d)\n", size);
  
  unsigned int ptr = mosNewPtr(size);
  
  m68k_set_reg(M68K_REG_A0, ptr);
  m68k_set_reg(M68K_REG_D0, 0);
}


void trapNewHandle(unsigned short) {
  unsigned int size = m68k_get_reg(0L, M68K_REG_D0);
  unsigned int hdl = 0;
  
  mosLog("            NewHandle(%d)\n", size);

  hdl = mosNewHandle(size);

  m68k_set_reg(M68K_REG_A0, hdl);
  m68k_set_reg(M68K_REG_D0, 0);
}


/** Find the Maste Pointer that points to some memory location and return a handle to it.
 */
void trapRecoverHandle(unsigned short) {
  unsigned int ptr = m68k_get_reg(0L, M68K_REG_A0);
  unsigned int hdl = mosRecoverHandle(ptr);
  mosLog("            RecoverHandle(0x%08X)=0x%08X\n", ptr, hdl);
  m68k_set_reg(M68K_REG_A0, hdl);
  // FIXME: set Memerr, but do not set D0!
}


void trapDisposePtr(unsigned short) {
  unsigned int ptr = m68k_get_reg(0L, M68K_REG_A0);
  mosLog("            DisposePtr(0x%08X)\n", ptr);
  mosDisposePtr(ptr);
  m68k_set_reg(M68K_REG_D0, 0);
}


void trapDisposeHandle(unsigned short) {
  unsigned int hdl = m68k_get_reg(0L, M68K_REG_A0);
  unsigned int ptr = hdl?m68k_read_memory_32(hdl):0;
  mosLog("            DisposeHandle(0x%08X(->0x%08X))\n", hdl, ptr);
  mosDisposeHandle(hdl);
  m68k_set_reg(M68K_REG_D0, 0);
}


/** Move a block of memory.
 * Blocks may overlap.
 * A0 = source
 * A1 = destination
 * D0 = number of bytes to move
 */
void trapBlockMove(unsigned short) {
  unsigned int src = m68k_get_reg(0L, M68K_REG_A0);
  unsigned int dst = m68k_get_reg(0L, M68K_REG_A1);
  unsigned int size = m68k_get_reg(0L, M68K_REG_D0);
  
  mosLog("            BlockMove(src:%s, dst:%s, %d)\n", printAddr(src), printAddr(dst), size);
  if (src<0x1000 || dst<0x1000) {
    mosLog("WARNING: the addresses seem highly unlikely\n");

  }
  memmove((void*)dst, (void*)src, size);
  
  m68k_set_reg(M68K_REG_D0, 0);
}


void trapGetTrapAddress(unsigned short) {
  unsigned int trap = m68k_get_reg(0L, M68K_REG_D0);
  unsigned int addr = (unsigned int)tncTable[trap&0x0fff];
  mosLog("            GetTrapAddress(0x%04X=%s) = 0x%08X\n", trap, trapName(trap), addr);
  m68k_set_reg(M68K_REG_A0, addr);
}


void trapSetToolBoxTrapAddress(unsigned short) {
  unsigned int trap = m68k_get_reg(0L, M68K_REG_D0);
  unsigned int addr = m68k_get_reg(0L, M68K_REG_A0);
  // 1010.1a0x.xxxx.xxxx: Toolbox call: x = trap #, if a is set, pop the extra return address from the stack
  trap = (trap & 0x0dff) | 0xa800;
  mosLog("            SetToolBoxTrapAddress(0x%04X=%s, 0x%08X)\n", trap, trapName(trap), addr);
  tncTable[trap&0x0fff] = (TrapNativeCall*)addr;
  m68k_set_reg(M68K_REG_A0, addr);
}


void trapLoadSeg(unsigned short instr) {
  unsigned int sp = m68k_get_reg(0L, M68K_REG_SP);
  //unsigned short stack_sr = m68k_read_memory_16(sp); sp += 2;
  unsigned int stack_ret = m68k_read_memory_32(sp); sp += 4;
  // pop the ID from the stack
  unsigned int id = m68k_read_memory_16(sp);
  sp += 2;
  mosLog("            LoadSeg(%d)\n", id);
  // now load the resource
  mosHandle hCode = GetResource('CODE', id);
  if (!hCode) {
    mosLog("Code Resource %d not found!\n", id);
  } else {
    unsigned int code = m68k_read_memory_32(hCode);
    // fix the jump table entry
    // FIXME: actually we need to fix all jump table entries for this particular ID!
    hexDump(hCode, 64);
    unsigned int offset = m68k_read_memory_16(stack_ret-8);
    m68k_write_memory_16(stack_ret-8, id);           // save the block id
    m68k_write_memory_16(stack_ret-6, 0x4ef9);       // 'jmp nnnnnnnn' instruction
    m68k_write_memory_32(stack_ret-4, code+offset+4);  // +4 -> skip the entry that gives the number of jump table entries?
  }
  sp -= 4; m68k_write_memory_32(sp, stack_ret-6);
  //sp -= 2; m68k_write_memory_16(sp, stack_sr);
  m68k_set_reg(M68K_REG_SP, sp);
}


void trapHGetState(unsigned short instr) {
  m68k_set_reg(M68K_REG_D0, 0x80); // locked memory // TODO: may need more flags!
}


void trapUninmplemented(unsigned short instr) {
  // FIXME: $a01f; opcode 1010 (_DisposePtr)
  mosLog("ERROR: unimplemented trap 0x%08X: %s\n", gCurrentTrap, trapName(gCurrentTrap));
  fprintf(stderr, "ERROR: unimplemented trap 0x%08X: %s\n", gCurrentTrap, trapName(gCurrentTrap));
}


void trapMoveHHi(unsigned short instr) {
  // nothing to do here
}


void trapHLock(unsigned short instr) {
  // nothing to do here
}

void trapStripAddress(unsigned short instr) {
  // nothing to do here
}

/** Dispatches into multiple new traps.
 * A7+00.l = return address
 * A7+04.w = selector
 * A7+06.l = A3 ??
 * A7+0A.l = D7 ?? 
 * A7+0E.l = return value
 *
 * selectors:
 *    mfMaxMemSel           21
 *    mfFreeMemSel          24
 *    mfTempNewHandleSel    29 = 0x1d  D7 = size, A3 = address to store resut code at (MemErr)
 *    mfTempHLockSel        30
 *    mfTempHUnLockSel      31
 *    mfTempDisposHandleSel 32 = 0x20
 *    0x37:
 *    0x3A:
 */
void trapOSDispatch(unsigned short instr) {
  unsigned int sp = m68k_get_reg(0L, M68K_REG_SP);
  unsigned int stack_ret = m68k_read_memory_32(sp); sp += 4;
  unsigned short selector = m68k_read_memory_16(sp); sp += 2;
  mosLog("trapOSDispatch: %d (0x%04x)\n", selector);
  
  switch (selector) {
    case 0x1d: { // mfTempNewHandleSel
      unsigned int resultCodePtr = m68k_read_memory_32(sp); sp += 4;
      unsigned int size = m68k_read_memory_32(sp); sp += 4;
      unsigned int handle = 0;
      
      handle = mosNewHandle(size);
      
      mosLog("Allocated a master pointer at 0x%08X\n", handle);
      if (resultCodePtr) m68k_write_memory_16(resultCodePtr, 0);
      m68k_write_memory_32(sp, handle);
      break; }
    case 0x20: { // mfTempDisposHandleSel
      unsigned int resultCodePtr = m68k_read_memory_32(sp); sp += 4;
      unsigned int handle = m68k_read_memory_32(sp); sp += 4;
      mosLog("TempDisposeHandle(0x%08X)\n", handle);
      
      mosDisposeHandle(handle);
      
      if (resultCodePtr) m68k_write_memory_16(resultCodePtr, 0);
      break; }
    default:
      mosLog("ERROR: unimplemented OSDispatch 0x%02X\n", selector);
      return;
  }

  sp -= 4; m68k_write_memory_32(sp, stack_ret);
  m68k_set_reg(M68K_REG_SP, sp);
}



void trapGoNative(unsigned short instr) {
  unsigned int pc = m68k_get_reg(0L, M68K_REG_PC);
  TrapNativeCall *tnc = (TrapNativeCall*)(pc);
  TrapNative callTrap = tnc->trapNative;
  callTrap(gCurrentTrap);
  pc = (unsigned int)(&(tnc->rts));
  m68k_set_reg(M68K_REG_PC, pc);
}


void trapBreakpoint(unsigned short instr) {
  unsigned int pc = m68k_get_reg(0L, M68K_REG_PC);
  Breakpoint *bp = findBreakpoint(pc);
  if (bp) {
    mosLog("BREAKPOINT: %s\n", bp->text);
    gPendingBreakpoint = bp;
  } else {
    mosLog("BREAKPOINT UNLISTED!\n");
  }
  bp = 0;
}

void trapDispatch(unsigned short)
{
  // the trap vector 0x00000028 points to a 0xaffd instruction, leading here
  // The stack contains the processor specific exception information and the
  // CPU is in supervisor mode. However, we want to run the trap code in user
  // mode, havng only the return address on the stack.
  unsigned int sp = m68k_get_reg(0L, M68K_REG_SP);
  unsigned int new_sr = m68k_read_memory_16(sp); sp+=2; // pop the status register
  unsigned int ret_addr = m68k_read_memory_32(sp); sp+=4;
  /*unsigned int vec =*/ m68k_read_memory_16(sp); sp+=2;
  sp-=4; m68k_write_memory_32(sp, ret_addr);
  m68ki_set_sr_noint(new_sr);
  // just leave the return address on the stack
  m68k_set_reg(M68K_REG_SP, sp);
  // set the new PC according to our jump table to allow patched jump tables
  m68k_set_reg(M68K_REG_PC, (unsigned int)tncTable[gCurrentTrap&0x0fff]);
}


// ----- End Traps


const char *printAddr(unsigned int addr)
{
  static char buf[8][32] = { 0 };
  static int currBuf = 0;
  
  // use the next buffer
  currBuf = (currBuf+1) & 7;
  char *dst = buf[currBuf];
  
  int i = 0;
  for (i=0; i<20; i++) {
    if (addr>=gResourceStart[i] && addr<gResourceEnd[i]) {
      sprintf(dst, "%02d.%05X", i, addr-gResourceStart[i]);
      return dst;
    }
  }
  sprintf(dst, "%08X", addr);
  return dst;
}


void m68k_instruction_hook()
{
  char buf[2048];
  for (;;) {
    gPendingBreakpoint = 0L;
  afterBreakpoint:
    unsigned int pc = m68k_get_reg(0L, M68K_REG_PC);
    unsigned short instr = m68k_read_memory_16(pc);
//    mosLog("\n");
//    mosLog("D0:%08X D1:%08X D2:%08X D3:%08X D4:%08X D5:%08X D6:%08X D7:%08X\n",
//           m68k_get_reg(0L, M68K_REG_D0),
//           m68k_get_reg(0L, M68K_REG_D1),
//           m68k_get_reg(0L, M68K_REG_D2),
//           m68k_get_reg(0L, M68K_REG_D3),
//           m68k_get_reg(0L, M68K_REG_D4),
//           m68k_get_reg(0L, M68K_REG_D5),
//           m68k_get_reg(0L, M68K_REG_D6),
//           m68k_get_reg(0L, M68K_REG_D7)
//           );
//    mosLog("A0:%08X A1:%08X A2:%08X A3:%08X A4:%08X A5:%08X A6:%08X A7:%08X\n",
//           m68k_get_reg(0L, M68K_REG_A0),
//           m68k_get_reg(0L, M68K_REG_A1),
//           m68k_get_reg(0L, M68K_REG_A2),
//           m68k_get_reg(0L, M68K_REG_A3),
//           m68k_get_reg(0L, M68K_REG_A4),
//           m68k_get_reg(0L, M68K_REG_A5),
//           m68k_get_reg(0L, M68K_REG_A6),
//           m68k_get_reg(0L, M68K_REG_A7)
//           );
    m68k_disassemble(buf, pc, M68K_CPU_TYPE_68020);
    if ( (instr & 0xf000) == 0xa000 ) {
      mosLog("0x%s: %s (%s)\n", printAddr(pc), buf, trapName(instr));
    } else {
      mosLog("0x%s: %s\n", printAddr(pc), buf);
    } // if/else
// ---> space for command breakpoint ;-)
    if ( (instr & 0xf000) == 0xa000 ) {
      // p (char*)(m68k_get_reg(0, (m68k_register_t)8)) // gdnb dump string at (A0)
      // 1010.1a0x.xxxx.xxxx: Toolbox call: x = trap #, if a is set, pop the extra return address from the stack
      // 1010.0ffa.xxxx.xxxx: OS call: x = trap #, ff are extra flags that can be used by the traps
      // 01f3 = a9f3
      switch (instr) {
        case 0xaffc: mosLog("End Of Emulation\n"); exit(m68k_get_reg(0, M68K_REG_D0)); break;
        case 0xaffd: trapDispatch(instr); break;
        case 0xaffe: trapBreakpoint(instr); goto afterBreakpoint;
        case 0xafff: trapGoNative(instr); break; // TODO: unverified
        default:
          gCurrentTrap = instr;
          return; // execute it!
      } // case
      // m68k_op_tst_16_d(); // tst.w d0
    } else {
      return;
    } // if/else
  } // for
}


int runApp()
{
  m68k_set_cpu_type(M68K_CPU_TYPE_68020);
  m68k_pulse_reset();
  m68k_set_cpu_type(M68K_CPU_TYPE_68020);
  m68k_set_reg(M68K_REG_PC, gMosCurrentA5 + gMosCurJTOffset + 2);
  m68k_write_memory_32(gMosCurrentStackBase-4, trapExitApp); // end of app
  m68k_set_reg(M68K_REG_SP, gMosCurrentStackBase-4);
  m68k_set_reg(M68K_REG_A5, gMosCurrentA5);
  m68k_set_instr_hook_callback(m68k_instruction_hook);
  
  //MosGetResource...
  
  char done = 0;
  while(!done) {
    m68k_execute(1);
  }
  return 0;
}


typedef struct
{
  int fd;
  const char *filename;
  bool open;
  bool allocated;
} MosFile;


MosFile stdFiles[] = {
  { STDIN_FILENO,  "/dev/stdin", true, false },
  { STDOUT_FILENO, "/dev/stdout", true, false },
  { STDERR_FILENO, "/dev/stderr", true, false }
};


/** Open a file.
 * Stack
 *   +00.l = return address
 *   +04.l = filename
 *   +08.l = command
 *   +0c.l = ptr to return value?
 */
///* 'd' => "directory" ops */
//#define F_DELETE        (('d'<<8)|0x01)
//#define F_RENAME        (('d'<<8)|0x02)
//#define F_OPEN          (('d'<<8)|0x00)     /* reserved for operating system use  */
///* 'e' => "editor" ops */
//#define F_GTABINFO      (('e'<<8)|0x00)     /* get tab offset for file            */
//#define F_STABINFO      (('e'<<8)|0x01)     /* set  "   "      "   "              */
//#define F_GFONTINFO     (('e'<<8)|0x02)     /* get font number and size for file  */
//#define F_SFONTINFO     (('e'<<8)|0x03)     /* set  "     "     "   "    "   "    */
//#define F_GPRINTREC     (('e'<<8)|0x04)     /* get print record for file          */
//#define F_SPRINTREC     (('e'<<8)|0x05)     /* set   "     "     "   "            */
//#define F_GSELINFO      (('e'<<8)|0x06)     /* get selection information for file */
//#define F_SSELINFO      (('e'<<8)|0x07)     /* set     "          "       "   "   */
//#define F_GWININFO      (('e'<<8)|0x08)     /* get current window position        */
//#define F_SWININFO      (('e'<<8)|0x09)     /* set    "      "       "            */
//#define F_GSCROLLINFO   (('e'<<8)|0x0A)     /* get scroll information             */
//#define F_SSCROLLINFO   (('e'<<8)|0x0B)     /* set    "        "                  */
//#define F_GMARKER       (('e'<<8)|0x0D)     /* Get Marker                         */
//#define F_SMARKER       (('e'<<8)|0x0C)     /* Set   "                            */
//#define F_GSAVEONCLOSE  (('e'<<8)|0x0F)     /* Get Save on close                  */
//#define F_SSAVEONCLOSE  (('e'<<8)|0x0E)     /* Set  "   "    "                    */

void trapSyFAccess(unsigned short) {
  mosLog("TODO: trapSyFAccess\n");
  unsigned int sp = m68k_get_reg(0L, M68K_REG_SP);
  const char *filename = (char*)m68k_read_memory_32(sp+4);
  unsigned int cmd = m68k_read_memory_32(sp+8);
  unsigned int file = m68k_read_memory_32(sp+12);
  unsigned short flags = m68k_read_memory_16(file);
  mosLog("Opening file '%s', cmd=0x%08X, arg=0x%08X, flags=0x%04X\n", filename, cmd, file, flags);
  if (cmd!=0x00006400) { // '..d.'
    mosError("Unknown file access command 0x%08X\n", cmd);
    m68k_set_reg(M68K_REG_D0, EINVAL); // no error
    return;
  }
  // TODO: convert filename and path from Mac Format to Unix Format
  char *uxFilename = (char*)calloc(1, strlen(filename)+2);
  const char *src = filename;
  char *dst = uxFilename;
  if (*src!=':') *dst++='/';
  for (;;) {
    char c = *src++;
    if (c==':') c = '/'; // TODO: what about the trailing ':'
    *dst++ = c;
    if (c==0) break;
  }
  // TODO: add our MosFile reference for internal data management
  // TODO: find the actual file and open it
  // open the file
  // TODO: what if the file is already open?
  // FIXME: do we need to convert the flags?
  int fd = -1;
  unsigned short mode = ((flags&3)-1);  // convert O_RDRW, O_RDONLY and O_WRONLY
  if ( flags & MOS_O_APPEND ) mode |= O_APPEND;
  if ( flags & MOS_O_APPEND ) mode |= O_APPEND;
  if ( flags & MOS_O_CREAT ) mode |= O_CREAT;
  if ( flags & MOS_O_TRUNC ) mode |= O_TRUNC;
  if ( flags & MOS_O_EXCL ) mode |= O_EXCL;
  if ( flags & MOS_O_NRESOLVE ) mode |= O_NOFOLLOW;
  // MOS_O_BINARY not tested
  if ( flags & MOS_O_RSRC ) {
    mosError("Open File %s: no resource fork support yet!\n", uxFilename);
    errno = 2;
  } else if ( flags & MOS_O_ALIAS ) {
      mosError("Open File %s: no alias support yet!\n", uxFilename);
      errno = 2;
  } else {
    fd = ::open(uxFilename, mode, 0644);
  }
  if (fd==-1) { // error
    fprintf(stderr, "Can't open file %s (mode 0x%04X): %s\n", uxFilename, mode, strerror(errno));
    m68k_set_reg(M68K_REG_D0, errno); // just return the error code
    free(uxFilename);
  } else {
    MosFile *mf = (MosFile*)calloc(1, sizeof(MosFile));
    m68k_write_memory_32(file+8, (unsigned int)mf);
    mf->fd = fd;
    mf->filename = uxFilename;
    mf->open = true;
    mf->allocated = true;
    m68k_set_reg(M68K_REG_D0, 0); // no error
  }
}

void trapSyClose(unsigned short) {
  unsigned int sp = m68k_get_reg(0L, M68K_REG_SP);
  unsigned int file = m68k_read_memory_32(sp+4);
  MosFile *mosFile = (MosFile*)m68k_read_memory_32(file+8);
  int ret = close(mosFile->fd);
  if (ret==-1) {
    m68k_set_reg(M68K_REG_D0, errno);
  } else {
    m68k_set_reg(M68K_REG_D0, 0);
  }
  if (mosFile->allocated) {
    m68k_write_memory_32(file+8, 0);
    if (mosFile->filename) {
      free((char*)mosFile->filename);
    }
    free(mosFile);
  }
}

void trapSyRead(unsigned short) {
  // file identifier is on the stack
  unsigned int sp = m68k_get_reg(0L, M68K_REG_SP);
  unsigned int file = m68k_read_memory_32(sp+4);
  MosFile *mosFile = (MosFile*)m68k_read_memory_32(file+8);
  void *buffer = (void*)m68k_read_memory_32(file+16);
  unsigned int size = m68k_read_memory_32(file+12);
  int ret = read(mosFile->fd, buffer, size);
  if (ret==-1) {
    m68k_set_reg(M68K_REG_D0, errno);
  } else {
    m68k_write_memory_32(file+12, size-ret);
    m68k_set_reg(M68K_REG_D0, 0); // no error
  }
}

// stack: // FIXME: this seems to be the write() call!
//   fileptr.l
void trapSyWrite(unsigned short) {
  unsigned int sp = m68k_get_reg(0L, M68K_REG_SP);
  unsigned int file = m68k_read_memory_32(sp+4);
  MosFile *mosFile = (MosFile*)m68k_read_memory_32(file+8);
  void *buffer = (void*)m68k_read_memory_32(file+16);
  unsigned int size = m68k_read_memory_32(file+12);
  // FIXME: convert to Unix line endings
  int i;
  char *s = (char*)buffer;
  for (i=size; i>0; --i) {
    if (*s=='\r') *s = '\n';
    s++;
  }
  //
  int ret = write(mosFile->fd, buffer, size);
  if (ret==-1) {
    m68k_set_reg(M68K_REG_D0, errno);
  } else {
    m68k_write_memory_32(file+12, size-ret);
    m68k_set_reg(M68K_REG_D0, 0); // no error
  }
}

// stack: // ioctl
//   0.l
//   0x6601.w = FIOCLEX _IO('f', 1) /* set exclusive use on fd */
//   fileptr.l
//

//# define FIOLSEEK               (('f'<<8)|0x00)  /* Apple internal use only */
//# define FIODUPFD               (('f'<<8)|0x01)  /* Apple internal use only */
//
//# define FIOINTERACTIVE (('f'<<8)|0x02)  /* If device is interactive */
//# define FIOBUFSIZE             (('f'<<8)|0x03)  /* Return optimal buffer size */
//# define FIOFNAME               (('f'<<8)|0x04)  /* Return filename */
//# define FIOREFNUM              (('f'<<8)|0x05)  /* Return fs refnum */
//# define FIOSETEOF              (('f'<<8)|0x06)  /* Set file length */
//
///*
// *   IOCTLs which begin with "TIO" are for TTY (i.e., console or
// *               terminal-related) device control requests.
// */
//
//# define TIOFLUSH   (('t'<<8)|0x00)             /* discard unread input.  arg is ignored */
//# define TIOSPORT   (('t'<<8)|0x01)             /* Obsolete -- do not use */
//# define TIOGPORT   (('t'<<8)|0x02)             /* Obsolete -- do not use */

void trapSyIoctl(unsigned short) {
  unsigned int sp = m68k_get_reg(0L, M68K_REG_SP);
  unsigned int file = m68k_read_memory_32(sp+4);
  unsigned int cmd = m68k_read_memory_32(sp+8);
  unsigned int param = m68k_read_memory_32(sp+12);
  MosFile *mosFile = (MosFile*)m68k_read_memory_32(file+8);
  mosLog("IOCTL of file at 0x%08X, cmd=0x%04X = '%c'<<8+%d, param=%d (0x%08X)\n",
         file, cmd, (cmd>>8)&0xff, cmd&0xff, param, param);
  switch (cmd) {
    case 0x6600: { // FIOLSEEK
      // Parameter points to two longs, the first is the offset type, the second is the offset itself
      // SEEK_SET=0 SEEK_CUR=1 SEEK_END=2
      // lseek returns -1 on fail and the previous position on success
      // ioctl return erroro in D0, and result in A6-4 (where the offset was originally)
      // TODO: more error checking
      unsigned int whence = m68k_read_memory_32(param);
      unsigned int offset = m68k_read_memory_32(param+4);
      int ret = (unsigned int)lseek(mosFile->fd, offset, whence);
      if (ret==-1) {
        m68k_set_reg(M68K_REG_D0, errno);
      } else {
        m68k_write_memory_32(param+4, ret);
        m68k_set_reg(M68K_REG_D0, 0); // no error
      }
      break; }
    case 0x6601: // FIODUPFD
      // TODO: more error checking
      // param is 0 in my tests
      // file is the current fd
      // TODO: I do not know what we must return here! Do I need to write the code to allocate a new fileStruct? Probably not!
      m68k_set_reg(M68K_REG_D0, 0); // no error
      break;
    case 0x6602: // FIOINTERACTIVE, return if device is interactive
      // TODO: more error checking
      m68k_set_reg(M68K_REG_D0, 0); // no error
      break;
    case 0x6603: // FIOBUFSIZE, Return optimal buffer size
      m68k_write_memory_16(param+2, 4096); // random value
      m68k_set_reg(M68K_REG_D0, 0); // no error
      break;
    case 0x6604: // FIOFNAME, Return filename
    case 0x6605: // FIOREFNUM, Return fs refnum
    case 0x6606: // FIOSETEOF, Set file length
    default:
      mosLog("ERROR: unsupported ioctrl on file operation\n");
      m68k_set_reg(M68K_REG_D0, EINVAL);
      break;
  }
}


mosPtr createGlue(mosTrap trap, unsigned short index = 0)
{
  // FIXME: unaligned format
  mosPtr p = mosNewPtr(12);
  mosWrite16(p,   0xAFFF);              // trap native
  *((unsigned int*)(p+4)) = (unsigned int)trap;  // function pointer
  mosWrite16(p+8, 0x4E75);              // rts
  
  if (index) {
    tncTable[index&0x0FFF] = (TrapNativeCall*)p;
  }
  
  return p;
}


void setupSystem(int argc, const char **argv, const char **envp)
{
  int i;
  
  // allocate a stack
  gMosCurrentStackBase = mosNewPtr(MOS_STACK_SIZE) + MOS_STACK_SIZE;
  
  // create other memory that will be accessed by the emulation
  trapDispatchTrap = mosNewPtr(2);
  mosWrite16(trapDispatchTrap, 0xaffd);
  trapExitApp = mosNewPtr(2);
  mosWrite16(trapExitApp, 0xaffc);
  
  // create supported trap glue
  mosPtr tncUnimplemented = createGlue(trapUninmplemented);
  tncTable = (TrapNativeCall**)mosNewPtr(0x0fff*4);
  for (i=0; i<0x0FFF; i++) {
    tncTable[i] = (TrapNativeCall*)tncUnimplemented;
  }
  // TODO: fill the entire array!
  createGlue(trapNewPtrClear, 0xA31E);
  createGlue(trapNewPtr, 0xA11E);
  createGlue(trapGetTrapAddress, 0xA146);
  tncTable[0x0746] = tncTable[0x0146];
  tncTable[0x0346] = tncTable[0x0146];
  createGlue(trapSetToolBoxTrapAddress, 0xA647);
  createGlue(trapLoadSeg, 0xA9F0);
  createGlue(trapHGetState, 0xA069);
  createGlue(trapHLock, 0xA029);
  createGlue(trapMoveHHi, 0xA064);
  createGlue(trapStripAddress, 0xA055);
  createGlue(trapGetResource, 0xA9A0);
  createGlue(trapSizeResource, 0xA9A5);
  createGlue(trapGetNamedResource, 0xA9A1);
  tncTable[0x0820] = tncTable[0x09A1];
  createGlue(trapBlockMove, 0xA02E);
  createGlue(trapOSDispatch, 0xA88F);
  createGlue(trapDisposePtr, 0xA01F);
  createGlue(trapRecoverHandle, 0xA128);
  createGlue(trapDisposeHandle, 0xA023);
  createGlue(trapNewHandle, 0xA122);

  
  // create native calls
  mosPtr tncSyFAccess = createGlue(trapSyFAccess);
  mosPtr tncSyClose   = createGlue(trapSyClose);
  mosPtr tncSyRead    = createGlue(trapSyRead);
  mosPtr tncSyWrite   = createGlue(trapSyWrite);
  mosPtr tncSyIoctl   = createGlue(trapSyIoctl);
  
  // create the IO function table
  mosPtr ioGlue = mosNewPtr(4*6*5); // only three entries are set, two entries are 0'd
  //    FSYS entry
  mosWrite32(ioGlue+0x0000+0x0000, 'FSYS');
  mosWrite32(ioGlue+0x0000+0x0004, tncSyFAccess);
  mosWrite32(ioGlue+0x0000+0x0008, tncSyClose);
  mosWrite32(ioGlue+0x0000+0x000C, tncSyRead);
  mosWrite32(ioGlue+0x0000+0x0010, tncSyWrite);
  mosWrite32(ioGlue+0x0000+0x0014, tncSyIoctl);
  //    CONS entry
  mosWrite32(ioGlue+0x0018+0x0000, 'CONS');
  mosWrite32(ioGlue+0x0018+0x0004, tncSyFAccess);
  mosWrite32(ioGlue+0x0018+0x0008, tncSyClose);
  mosWrite32(ioGlue+0x0018+0x000C, tncSyRead);
  mosWrite32(ioGlue+0x0018+0x0010, tncSyWrite);
  mosWrite32(ioGlue+0x0018+0x0014, tncSyIoctl);
  //    SYST entry
  mosWrite32(ioGlue+0x0030+0x0000, 'SYST');
  mosWrite32(ioGlue+0x0030+0x0004, tncSyFAccess);
  mosWrite32(ioGlue+0x0030+0x0008, tncSyClose);
  mosWrite32(ioGlue+0x0030+0x000C, tncSyRead);
  mosWrite32(ioGlue+0x0030+0x0010, tncSyWrite);
  mosWrite32(ioGlue+0x0030+0x0014, tncSyIoctl);
  
  // create the file descriptor entries for stdin, stdout, and stderr
  mosPtr fdEntries = mosNewPtr(5*4*3);
  // stdin
  mosWrite16(fdEntries+0x0000+0x0000, 1); // input
  mosWrite16(fdEntries+0x0000+0x0002, 0); // status OK
  mosWrite32(fdEntries+0x0000+0x0004, ioGlue+0x0000); // FSYS
  mosWrite32(fdEntries+0x0000+0x0008, (unsigned int)(&stdFiles[0])); // back to host environment
  mosWrite32(fdEntries+0x0000+0x000C, 0); // transfer size
  mosWrite32(fdEntries+0x0000+0x0010, 0); // transfer buffer address
  // stdout
  mosWrite16(fdEntries+0x0014+0x0000, 2); // output
  mosWrite16(fdEntries+0x0014+0x0002, 0); // status OK
  mosWrite32(fdEntries+0x0014+0x0004, ioGlue+0x0000); // FSYS
  mosWrite32(fdEntries+0x0014+0x0008, (unsigned int)(&stdFiles[1])); // back to host environment
  mosWrite32(fdEntries+0x0014+0x000C, 0); // transfer size
  mosWrite32(fdEntries+0x0014+0x0010, 0); // transfer buffer address
  // stderr
  mosWrite16(fdEntries+0x0028+0x0000, 2); // output
  mosWrite16(fdEntries+0x0028+0x0002, 0); // status OK
  mosWrite32(fdEntries+0x0028+0x0004, ioGlue+0x0000); // FSYS
  mosWrite32(fdEntries+0x0028+0x0008, (unsigned int)(&stdFiles[2])); // back to host environment
  mosWrite32(fdEntries+0x0028+0x000C, 0); // transfer size
  mosWrite32(fdEntries+0x0028+0x0010, 0); // transfer buffer address
  
  // create the argv array
  // FIXME: do this dynamically
  mosPtr argv0 = mosNewPtr("ARM6asm");
  mosPtr argv1 = mosNewPtr("/Users/matt/dev/Alienate/ARM6asm/test.s");
  mosPtr mpwArgv = mosNewPtr(12);
  mosWrite32(mpwArgv+0, argv0);
  mosWrite32(mpwArgv+4, argv1);
  mosWrite32(mpwArgv+8, 0);
  
  // TODO: envp support
  
  // create the MPW memory table that allows tools to connect back to MPW
  mosPtr mpwMem = mosNewPtr(0x0028);
  mosWrite16(mpwMem+0x0000, 0x5348);
  mosWrite32(mpwMem+0x0002, 2); // argc
  mosWrite32(mpwMem+0x0006, mpwArgv); // argv
  mosWrite32(mpwMem+0x000A, 0); // envp
  mosWrite32(mpwMem+0x000E, 0); // NULL
  mosWrite32(mpwMem+0x0012, 0); // unknown
  mosWrite32(mpwMem+0x0016, 0); // unknown
  mosWrite16(mpwMem+0x001A, 400); // file table size
  mosWrite32(mpwMem+0x001C, fdEntries); // table of file descriptor
  mosWrite32(mpwMem+0x0020, ioGlue);    // table of file functions
  mosWrite32(mpwMem+0x0024, 0); // unknown
  
  // create the MPW master pointer
  mosPtr mpwHandle = mosNewPtr(8);
  mosWrite32(mpwHandle+0x0000, 'MPGM'); // ID
  mosWrite32(mpwHandle+0x0004, mpwMem); // data
  
  gMosMPWHandle = mpwHandle;
}


unsigned int m68k_read_memory_8(unsigned int address)
{
  if (address>=0x1000) {
    return mosRead8(address);
  }
  if (address<0x1e00) {
    const char *rem = "";
    const char *var = gvarName(address, &rem);
    mosLog("Read.b 0x%04x: %s %s\n", address, var, rem);
  }
  switch (address) {
    case 0x012d: return 0; // LoadTrap [GLOBAL VAR]  trap before launch? [byte]
    default:
      mosLog("Accessing unsupported RAM.b address 0x%08X\n", address);
      break;
  }
  return 0;
}

unsigned int m68k_read_memory_16(unsigned int address)
{
  if (gPendingBreakpoint && gPendingBreakpoint->address==address) {
    return gPendingBreakpoint->originalCmd;
  }
  if (address>=0x1000) {
    return mosRead16(address);
  }
  if (address<0x1e00) {
    const char *rem = "";
    const char *var = gvarName(address, &rem);
    mosLog("Read.w 0x%04x: %s %s\n", address, var, rem);
  }
  switch (address) {
    case 0x0070: return 0xa9f4; // ExitToShell() -> debug trap quite the app
    case 0x028e: return 0; // >0 if 128k ROM mac or later
    case 0x0934: return gMosCurJTOffset; // offset from A5 to first entry in jump table
    case 0x0a60: return gMosResErr; // Resource Manager error code
    case 0x0220: return gMosMemErr; // Memory Operation Error Code
    // Read.w 0x0220: MemErr [GLOBAL VAR]  last memory manager error [word]
    case 0x0930: return 0; // FIXME: SaveSegHandle [GLOBAL VAR]  seg 0 handle [handle]
    default:
      mosLog("Accessing unsupported RAM.w address 0x%08X\n", address);
      fprintf(stderr, "Accessing unsupported RAM.w address 0x%08X\n", address);
      break;
  }
  return 0;
}

unsigned int m68k_read_memory_32(unsigned int address)
{
  if (address>=0x1000) {
    return mosRead32(address);
  }
  if (address<0x1e00) {
    const char *rem = "";
    const char *var = gvarName(address, &rem);
    mosLog("Read.l 0x%04x: %s %s\n", address, var, rem);
  }
  switch (address) {
    case 0: return 0;
    case 4: return 0;
    case 0x0028: return trapDispatchTrap;
    //case 0x0316: return 0; // MacPgm // this is MPW memory with argc and more
    //case 0x0316: return (unsigned int)(&gMacPgm); // MacPgm // this is MPW memory with argc and more
    case 0x0316: return gMosMPWHandle;
    case 0x0910: // CurApName [GLOBAL VAR] Name of current application (length byte followed by up to 31 characters) name of application [STRING[31]]
    case 0x0914:
    case 0x0918:
    case 0x091c:
    case 0x0920:
    case 0x0924:
    case 0x0928:
    case 0x092c:
      return 0;
    default:
      mosLog("Accessing unsupported RAM.l address 0x%08X\n", address);
      break;
  }
  return 0;
}

unsigned int m68k_read_disassembler_8(unsigned int address)
{
  return m68k_read_memory_8(address);
}

unsigned int m68k_read_disassembler_16(unsigned int address)
{
  return m68k_read_memory_16(address);
}

unsigned int m68k_read_disassembler_32(unsigned int address)
{
  return m68k_read_memory_32(address);
}

void m68k_write_memory_8(unsigned int address, unsigned int value)
{
  if (address>=0x1000) {
    mosWriteUnsafe8(address, value);
    return;
  }
  if (address<0x1e00) {
    const char *rem = "";
    const char *var = gvarName(address, &rem);
    mosLog("Write.b 0x%04x = 0x%02X: %s %s\n", address, value & 0xff, var, rem);
  }
  switch (address) {
    case 0x0a5e: gMosResLoad = value; break; // ResLoad       0A5E  word  Auto-load feature
    default:
      mosLog("Writing unsupported RAM.b address 0x%08X\n", address);
      break;
  }
}

void m68k_write_memory_16(unsigned int address, unsigned int value)
{
  if (address>=0x1000) {
    mosWriteUnsafe16(address, value);
    return;
  }
  if (address<0x1e00) {
    const char *rem = "";
    const char *var = gvarName(address, &rem);
    mosLog("Write.w 0x%04x = 0x%04X: %s %s\n", address, value & 0xffff, var, rem);
  }
  switch (address) {
    default:
      mosLog("Writing unsupported RAM.w address 0x%08X\n", address);
      break;
  }
}

void m68k_write_memory_32(unsigned int address, unsigned int value)
{
  if (address>=0x1000) {
    mosWriteUnsafe32(address, value);
    return;
  }
  if (address<0x1e00) {
    const char *rem = "";
    const char *var = gvarName(address, &rem);
    mosLog("Write.l 0x%04x = 0x%08X: %s %s\n", address, value, var, rem);
  }
  switch (address) {
    default:
      mosLog("Writing unsupported RAM.l address 0x%08X\n", address);
      break;
  }
}
  

void setBreakpoints()
{
  // Breakpoints for ARM6asm only
  //  addBreakpoint(2, 0x0000003C, "SADEV: _coWrite");
  //  addBreakpoint(2, 0x000000B2, "SADEV: _fsWrite");
  //  addBreakpoint(2, 0x00000116, "SADEV: _syWrite");
  //  addBreakpoint(4, 0x00000185, "STDIO print");
  //  addBreakpoint(1, 0x0001DC14, "RTInit");
  //  addBreakpoint(8, 0x000002AE, "Patched LoadSeg");
  //  addBreakpoint(8, 0x0000032A, "Just before Load_Code");
  //  addBreakpoint(8, 0x00000004);
  
  //  addBreakpoint(7, 0x000007E6, "_faccess()");
  //  addBreakpoint(7, 0x00000000, "faccess()");
  //  addBreakpoint(7, 0x000004BA, "Pre BlockMove in _initIOPtable");
  //  addBreakpoint(7, 0x000004F2, "_initIOPtable");
  //  addBreakpoint(7, 0x00000838, "_faccess");
  //  addBreakpoint(7, 0x00000026, "open");
  //  addBreakpoint(7, 0x0000003C, "open2");
  //  addBreakpoint(7, 0x0000005A, "open3");
  
  //  addBreakpoint(1, 0x00003E70, "Usage");
  //  addBreakpoint(7, 0x000000DA, "Exit");

  //addBreakpoint(8, 0x00000000, "Launch");
}


// TODO: forward argc and argv to the app we are loading
// TODO: run stdout through a Mac-to-Unix filter
// TODO: TripleDash arguments?   ARM6asm ---unix2mac myFile.s -o ---mac2unix myFile.o ---stdout2unix ---stderr2unix
int main(int argc, const char **argv, const char **envp)
{
#ifdef MOS_UNITTESTS
  mosMemoeryUnittests();
#else
  
  
  FILE *logFile = 0L;
  // logFile = fopen("/Users/matt/dev/Alienate/log.txt", "wb");
  // logFile = stdout;
  if (logFile)
    mosLogTo(logFile);
  
  setBreakpoints();
  
  setupSystem(argc, argv, envp);
  
  int appLoaded = 0;
  
  if (!appLoaded) {
    appLoaded = loadInternalApp();
  }
  
  if (!appLoaded) {
    appLoaded = loadApp(argv[1]);
  }
  
  if (!appLoaded) {
    fprintf(stderr, "MOSRUN - FATAL ERROR: can't find application data\n");
    exit(9);
  }
  
  runApp();

  if (logFile)
    fclose(logFile);
#endif
  return 0;
}




