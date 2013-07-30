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

byte *theApp = 0;
ssize_t theAppSize;
byte *theRsrc = 0;
ssize_t theRsrcSize;
byte *theJumpTable = 0;

unsigned int gMosCurrentA5 = 0;
unsigned int gMosCurrentStackBase = 0;
unsigned int gMosCurJTOffset = 0;
unsigned int gMosResLoad = 1;
unsigned int gMosResErr = 0;
unsigned int gMosMemErr = 0;

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

void hexDump(unsigned int a, unsigned int n)
{
  int i;
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

void setupSystem(int argc, const char **argv, const char **envp)
{
  // TODO: copy argv and envp into the simulation
  gMosCurrentStackBase = (unsigned int)malloc(0x8000) + 0x8000;
  // push a pointer to an instruction onto the stack that allows the software to exit nicely
}


void dumpResourceMap()
{
  unsigned int i, j;
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
  unsigned int i, j;
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
            // the first four bytes of the memory location become the master pointer,
            // foloowed by the size of the resource and the remainder is the resource itself
            byte *newPtr = (byte*)malloc(rsrcSize+4+4);
            // make the mmaster pointer point to the resource
            m68k_write_memory_32((unsigned int)(newPtr), (unsigned int)(newPtr+8));
            // remember the size of the resource here
            m68k_write_memory_32((unsigned int)(newPtr+4), rsrcSize);
            // copy the resource at offset 8
            memcpy(newPtr+8, theApp+rsrcData+rsrcOffset+4, rsrcSize);
            // make the resource map point to the resource handle
            m68k_write_memory_32((unsigned int)(theRsrc+resTable+12*j+8), (unsigned int)(newPtr));
            // set breakpoints
            if (myResType=='CODE') {
              if (m68k_read_memory_16((unsigned int)(theApp+rsrcData+rsrcOffset+4))==0xffff) {
                installBreakpoints(myId, (unsigned int)(newPtr+8+4+0x24)); // 0x24
                gResourceStart[myId] = (unsigned int)(newPtr+8+4+0x24);
                gResourceEnd[myId] = (unsigned int)(newPtr+8+4) + rsrcSize;
              } else {
                installBreakpoints(myId, (unsigned int)(newPtr+8+4)); // 0x24
                gResourceStart[myId] = (unsigned int)(newPtr+8+4);
                gResourceEnd[myId] = (unsigned int)(newPtr+8+4) + rsrcSize;
              }
              mosLog("Resource %d from 0x%08X to 0x%08X\n", myId, gResourceStart[myId], gResourceEnd[myId]);
            }
            return (mosHandle)(newPtr);
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
  unsigned int i, j;
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
            // the first four bytes of the memory location become the master pointer,
            // foloowed by the size of the resource and the remainder is the resource itself
            byte *newPtr = (byte*)malloc(rsrcSize+4+4);
            // make the mmaster pointer point to the resource
            m68k_write_memory_32((unsigned int)(newPtr), (unsigned int)(newPtr+8));
            // remember the size of the resource here
            m68k_write_memory_32((unsigned int)(newPtr+4), rsrcSize);
            // copy the resource at offset 8
            memcpy(newPtr+8, theApp+rsrcData+rsrcOffset+4, rsrcSize);
            // make the resource map point to the resource handle
            m68k_write_memory_32((unsigned int)(theRsrc+resTable+12*j+8), (unsigned int)(newPtr));
            // set breakpoints
            if (myResType=='CODE') {
              installBreakpoints(id, (unsigned int)(newPtr+8+4));
              gResourceStart[id] = (unsigned int)(newPtr+8+4);
              gResourceEnd[id] = (unsigned int)(newPtr+8+4) + rsrcSize;
            }
            return (mosHandle)(newPtr);
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
  theJumpTable = (byte*)calloc(1, aboveA5+belowA5);
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
  theRsrc = (byte*)malloc(rsrcMapSize);
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
  theApp = (byte*)calloc(1, size);
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
    theApp = (byte*)malloc(theAppSize);
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
extern TrapNativeCall *tncTable[0x1000];


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
  
  unsigned int size = m68k_read_memory_32(hdl+4);
  mosLog("            SizeResource(0x%08X) = %d\n", hdl, size);
  
  m68k_set_reg(M68K_REG_D0, 0);
  m68k_write_memory_32(sp, size);
  sp-=4; m68k_write_memory_32(sp, stack_ret);
  m68k_set_reg(M68K_REG_SP, sp);
}



void trapNewPtrClear(unsigned short) {
  unsigned int size = m68k_get_reg(0L, M68K_REG_D0);
  
  mosLog("            NewPtrClear(%d)\n", size);
  
  unsigned int ptr = (unsigned int)calloc(1, size+4);
  m68k_write_memory_32(ptr, size);
  
  m68k_set_reg(M68K_REG_A0, ptr+4);
  m68k_set_reg(M68K_REG_D0, 0);
}


void trapNewPtr(unsigned short) {
  unsigned int size = m68k_get_reg(0L, M68K_REG_D0);
  
  mosLog("            NewPtr(%d)\n", size);
  
  unsigned int ptr = (unsigned int)malloc(size+4);
  m68k_write_memory_32(ptr, size);
  
  m68k_set_reg(M68K_REG_A0, ptr+4);
  m68k_set_reg(M68K_REG_D0, 0);
}


void trapNewHandle(unsigned short) {
  unsigned int size = m68k_get_reg(0L, M68K_REG_D0);
  unsigned int ptr = 0;
  unsigned int handle = 0;
  
  mosLog("            NewHandle(%d)\n", size);
  
  if (size) {
    ptr = (unsigned int)malloc(size+4);
    m68k_write_memory_32(ptr, size);
    handle = (unsigned int)malloc(4);
    m68k_write_memory_32(handle, ptr+4);
  } else {
    handle = (unsigned int)malloc(4);
    m68k_write_memory_32(handle, 0);
  }

  m68k_set_reg(M68K_REG_A0, handle);
  m68k_set_reg(M68K_REG_D0, 0);
}


/** Find the Maste Pointer that points t some memory location and return a handle to it.
 * FIXME: this one single silly call requires that I keep track of every silly memory allocation. Sigh!
 */
void trapRecoverHandle(unsigned short) {
  unsigned int ptr = m68k_get_reg(0L, M68K_REG_A0);
  unsigned int hdl = 0;
  mosLog("            RecoverHandle(0x%08X)=0x%08X\n", ptr, hdl);
  m68k_set_reg(M68K_REG_A0, hdl);
  // FIXME: set Memerr, but do not set D0!
}


void trapDisposePtr(unsigned short) {
  unsigned int ptr = m68k_get_reg(0L, M68K_REG_A0);
  mosLog("            DisposePtr(0x%08X)\n", ptr);
  free((void*)(ptr-4));
  m68k_set_reg(M68K_REG_D0, 0);
}


void trapDisposeHandle(unsigned short) {
  unsigned int hdl = m68k_get_reg(0L, M68K_REG_A0);
  unsigned int ptr = hdl?m68k_read_memory_32(hdl):0;
  mosLog("            DisposeHandle(0x%08X(->0x%08X))\n", hdl, ptr);
  //free((void*)(ptr-4));
  // FIXME: implement this
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
  
  switch (selector) {
    case 0x1d: { // mfTempNewHandleSel
      unsigned int resultCodePtr = m68k_read_memory_32(sp); sp += 4;
      unsigned int size = m68k_read_memory_32(sp); sp += 4;
      unsigned int handle = 0;
      unsigned int ptr = 0;
      
      handle = (unsigned int)malloc(4);
      if (size==0) {
        ptr = 0;
      } else {
        ptr = (unsigned int)malloc(size+4) + 4;
        m68k_write_memory_32(ptr-4, size);
        mosLog("Allocated %d bytes at 0x%08X\n", size, ptr);
      }
      m68k_write_memory_32(handle, ptr);
      mosLog("Allocated a master pointer at 0x%08X\n", handle);
      if (resultCodePtr) m68k_write_memory_16(resultCodePtr, 0);
      m68k_write_memory_32(sp, handle);
      break; }
    case 0x20: { // mfTempDisposHandleSel
      unsigned int resultCodePtr = m68k_read_memory_32(sp); sp += 4;
      unsigned int handle = m68k_read_memory_32(sp); sp += 4;
      unsigned int ptr = handle?m68k_read_memory_32(handle):0;
      mosLog("TempDisposeHandle(0x%08X(->0x%08X))\n", handle, ptr);
      // FIXME: implement this!
      if (resultCodePtr) m68k_write_memory_16(resultCodePtr, 0);
      m68k_write_memory_32(sp, handle);
      break; }
    default:
      mosLog("ERROR: unimplemented OSDispatch 0x%02X\n", selector);
      return;
  }

  sp -= 4; m68k_write_memory_32(sp, stack_ret);
  m68k_set_reg(M68K_REG_SP, sp);
}

TrapNativeCall tncNewPtrClear = { htons(0xAFFF), &trapNewPtrClear, htons(0x4E75) };
TrapNativeCall tncNewPtr = { htons(0xAFFF), &trapNewPtr, htons(0x4E75) };
TrapNativeCall tncGetTrapAddress = { htons(0xAFFF), &trapGetTrapAddress, htons(0x4E75) };
TrapNativeCall tncSetToolBoxTrapAddress = { htons(0xAFFF), &trapSetToolBoxTrapAddress, htons(0x4E75) };
TrapNativeCall tncLoadSeg = { htons(0xAFFF), &trapLoadSeg, htons(0x4E75) };
TrapNativeCall tncHGetState = { htons(0xAFFF), &trapHGetState, htons(0x4E75) };
TrapNativeCall tncHLock = { htons(0xAFFF), &trapHLock, htons(0x4E75) };
TrapNativeCall tncMoveHHi = { htons(0xAFFF), &trapMoveHHi, htons(0x4E75) };
TrapNativeCall tncStripAddress = { htons(0xAFFF), &trapStripAddress, htons(0x4E75) };
TrapNativeCall tncUnimplemented = { htons(0xAFFF), &trapUninmplemented, htons(0x4E75) };
TrapNativeCall tncGetResource = { htons(0xAFFF), &trapGetResource, htons(0x4E75) };
TrapNativeCall tncSizeResource = { htons(0xAFFF), &trapSizeResource, htons(0x4E75) };
TrapNativeCall tncGetNamedResource = { htons(0xAFFF), &trapGetNamedResource, htons(0x4E75) };
TrapNativeCall tncBlockMove = { htons(0xAFFF), &trapBlockMove, htons(0x4E75) };
TrapNativeCall tncOSDispatch = { htons(0xAFFF), &trapOSDispatch, htons(0x4E75) };
TrapNativeCall tncDisposePtr = { htons(0xAFFF), &trapDisposePtr, htons(0x4E75) };
TrapNativeCall tncRecoverHandle = { htons(0xAFFF), &trapRecoverHandle, htons(0x4E75) };
TrapNativeCall tncDisposeHandle = { htons(0xAFFF), &trapDisposeHandle, htons(0x4E75) };
TrapNativeCall tncNewHandle = { htons(0xAFFF), &trapNewHandle, htons(0x4E75) };

#define TNCUREF     &tncUnimplemented,
#define TNCUREF16   TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF
#define TNCUREF256  TNCUREF16 TNCUREF16 TNCUREF16 TNCUREF16 TNCUREF16 TNCUREF16 TNCUREF16 TNCUREF16 TNCUREF16 TNCUREF16 TNCUREF16 TNCUREF16 TNCUREF16 TNCUREF16 TNCUREF16 TNCUREF16

//        case 0xa820: mosTrapGetNamedResource(instr); break; // TODO: unverified


TrapNativeCall *tncTable[0x1000] = {
  /* A000 */ //TNCUREF256
  /*   A000 */ TNCUREF16
  /*   A010 */ //TNCUREF16
  /*     A010 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF
  /*     A018 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF &tncDisposePtr,
  /*   A020 */ //TNCUREF16
  /*     A020 */ TNCUREF TNCUREF TNCUREF &tncDisposeHandle, TNCUREF TNCUREF TNCUREF TNCUREF
  /*     A028 */ TNCUREF &tncHLock, TNCUREF TNCUREF TNCUREF TNCUREF &tncBlockMove, TNCUREF
  /*   A030 */ TNCUREF16
  /*   A040 */ TNCUREF16
  /*   A050 */ //TNCUREF16
  /*     A050 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF &tncStripAddress, TNCUREF TNCUREF
  /*     A058 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF
  /*   A060 */ //TNCUREF16
  /*     A060 */ TNCUREF TNCUREF TNCUREF TNCUREF &tncMoveHHi, TNCUREF TNCUREF TNCUREF
  /*     A068 */ TNCUREF &tncHGetState, TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF
  /*   A070 */ TNCUREF16
  /*   A080 */ TNCUREF16
  /*   A090 */ TNCUREF16
  /*   A0A0 */ TNCUREF16
  /*   A0B0 */ TNCUREF16
  /*   A0C0 */ TNCUREF16
  /*   A0D0 */ TNCUREF16
  /*   A0E0 */ TNCUREF16
  /*   A0F0 */ TNCUREF16
  /* A100 */ //TNCUREF256
  /*   A100 */ TNCUREF16
  /*   A110 */ //TNCUREF16
  /*     A110 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF
  /*     A118 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF &tncNewPtr, TNCUREF
  /*   A120 */ //TNCUREF16
  /*     A120 */ TNCUREF TNCUREF &tncNewHandle, TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF
  /*     A128 */ &tncRecoverHandle, TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF
  /*   A130 */ TNCUREF16
  /*   A140 */ TNCUREF16
  /*   A150 */ TNCUREF16
  /*   A160 */ TNCUREF16
  /*   A170 */ TNCUREF16
  /*   A180 */ TNCUREF16
  /*   A190 */ TNCUREF16
  /*   A1a0 */ TNCUREF16
  /*   A1b0 */ TNCUREF16
  /*   A1c0 */ TNCUREF16
  /*   A1d0 */ TNCUREF16
  /*   A1e0 */ TNCUREF16
  /*   A1f0 */ TNCUREF16
  /* A200 */ TNCUREF256
  /* A300 */ // TNCUREF256
  /*   A300 */ TNCUREF16
  /*   A310 */ // TNCUREF16
  /*     A310 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF
  /*     A318 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF &tncNewPtrClear, TNCUREF
  /*   A320 */ TNCUREF16
  /*   A330 */ TNCUREF16
  /*   A340 */ //TNCUREF16
  /*     A340 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF &tncGetTrapAddress, TNCUREF
  /*     A348 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF
  /*   A350 */ TNCUREF16
  /*   A360 */ TNCUREF16
  /*   A370 */ TNCUREF16
  /*   A380 */ TNCUREF16
  /*   A390 */ TNCUREF16
  /*   A3A0 */ TNCUREF16
  /*   A3B0 */ TNCUREF16
  /*   A3C0 */ TNCUREF16
  /*   A3D0 */ TNCUREF16
  /*   A3E0 */ TNCUREF16
  /*   A3F0 */ TNCUREF16
  /* A400 */ TNCUREF256
  /* A500 */ TNCUREF256
  /* A600 */ //TNCUREF256
  /*   A600 */ TNCUREF16
  /*   A610 */ TNCUREF16
  /*   A620 */ TNCUREF16
  /*   A630 */ TNCUREF16
  /*   A640 */ //TNCUREF16
  /*     A640 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF &tncSetToolBoxTrapAddress,
  /*     A648 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF
  /*   A650 */ TNCUREF16
  /*   A660 */ TNCUREF16
  /*   A670 */ TNCUREF16
  /*   A680 */ TNCUREF16
  /*   A690 */ TNCUREF16
  /*   A6A0 */ TNCUREF16
  /*   A6B0 */ TNCUREF16
  /*   A6C0 */ TNCUREF16
  /*   A6D0 */ TNCUREF16
  /*   A6E0 */ TNCUREF16
  /*   A6F0 */ TNCUREF16
  /* A700 */ //TNCUREF256
  /*   A700 */ TNCUREF16
  /*   A710 */ TNCUREF16
  /*   A720 */ TNCUREF16
  /*   A730 */ TNCUREF16
  /*   A740 */ //TNCUREF16
  /*     A740 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF &tncGetTrapAddress, TNCUREF
  /*     A748 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF
  /*   A750 */ TNCUREF16
  /*   A760 */ TNCUREF16
  /*   A770 */ TNCUREF16
  /*   A780 */ TNCUREF16
  /*   A790 */ TNCUREF16
  /*   A7A0 */ TNCUREF16
  /*   A7B0 */ TNCUREF16
  /*   A7C0 */ TNCUREF16
  /*   A7D0 */ TNCUREF16
  /*   A7E0 */ TNCUREF16
  /*   A7F0 */ TNCUREF16
  /* A800 */ //TNCUREF256
  /*   A800 */ TNCUREF16
  /*   A810 */ TNCUREF16
  /*   A820 */ //TNCUREF16 
  /*     A820 */ &tncGetNamedResource, TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF
  /*     A828 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF
  /*   A830 */ TNCUREF16
  /*   A840 */ TNCUREF16
  /*   A850 */ TNCUREF16
  /*   A860 */ TNCUREF16
  /*   A870 */ TNCUREF16
  /*   A880 */ //TNCUREF16
  /*     A880 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF
  /*     A888 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF &tncOSDispatch,
  /*   A890 */ TNCUREF16
  /*   A8A0 */ TNCUREF16
  /*   A8B0 */ TNCUREF16
  /*   A8C0 */ TNCUREF16
  /*   A8D0 */ TNCUREF16
  /*   A8E0 */ TNCUREF16
  /*   A8F0 */ TNCUREF16
  /* A900 */ //TNCUREF256
  /*   A900 */ TNCUREF16
  /*   A910 */ TNCUREF16
  /*   A920 */ TNCUREF16
  /*   A930 */ TNCUREF16
  /*   A940 */ TNCUREF16
  /*   A950 */ TNCUREF16
  /*   A960 */ TNCUREF16
  /*   A970 */ TNCUREF16
  /*   A980 */ TNCUREF16
  /*   A990 */ TNCUREF16
  /*   A9A0 */ //TNCUREF16
  /*     A9A0 */ &tncGetResource, TNCUREF TNCUREF TNCUREF TNCUREF &tncSizeResource, TNCUREF TNCUREF
  /*     A9A8 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF
  /*   A9B0 */ TNCUREF16
  /*   A9C0 */ TNCUREF16
  /*   A9D0 */ TNCUREF16
  /*   A9E0 */ TNCUREF16
  /*   A9F0 */ //TNCUREF16
  /*     A9F0 */ &tncLoadSeg, TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF
  /*     A9F8 */ TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF TNCUREF
  /* AA00 */ TNCUREF256
  /* AB00 */ TNCUREF256
  /* AC00 */ TNCUREF256
  /* AD00 */ TNCUREF256
  /* AE00 */ TNCUREF256
  /* AF00 */ TNCUREF256
  //TNCUREF
};


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

unsigned short trapDispatchTrap = htons(0xaffd);
unsigned short trapExitApp = htons(0xaffc);

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
  static char buf[8][32];
  static int currBuf = 0;
  
  // use the next buffer
  currBuf = (currBuf+1) & 7;
  char *dst = buf[currBuf];
  
  int i;
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
    m68k_disassemble(buf, pc, M68K_CPU_TYPE_68000);
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
  m68k_set_reg(M68K_REG_PC, gMosCurrentA5 + gMosCurJTOffset + 2);
  m68k_write_memory_32(gMosCurrentStackBase-4, (unsigned int)(&trapExitApp)); // end of app
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
void trapSyFAccess(unsigned short) {
  mosLog("TODO: trapSyFAccess\n");
  unsigned int sp = m68k_get_reg(0L, M68K_REG_SP);
  const char *filename = (char*)m68k_read_memory_32(sp+4);
  unsigned int flags = m68k_read_memory_8(sp+10);
  unsigned int file = m68k_read_memory_32(sp+12);
  mosLog("Opening file '%s', flags=0x%02X, arg=0x%08X\n", filename, flags, file);
  // TODO: convert filename and path from Mac Format to Unix Format
  char *uxFilename = (char*)malloc(strlen(filename)+2);
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
  int fd = ::open(uxFilename, flags & 0x3f);
  if (fd==-1) { // error
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

// stack: // FIXME: This is Close()
//   fileptr.l
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


TrapNativeCall tncSyFAccess = { htons(0xAFFF), &trapSyFAccess, htons(0x4E75) };
TrapNativeCall tncSyClose   = { htons(0xAFFF), &trapSyClose,   htons(0x4E75) };
TrapNativeCall tncSyRead    = { htons(0xAFFF), &trapSyRead,    htons(0x4E75) };
TrapNativeCall tncSyWrite   = { htons(0xAFFF), &trapSyWrite,   htons(0x4E75) };
TrapNativeCall tncSyIoctl   = { htons(0xAFFF), &trapSyIoctl,   htons(0x4E75) };


unsigned int gSomeTable20[] =
{ //    access       close (long)       Read               Write              Ioctl              FAccess
  // NEW:            Open(?)            Close(!)           Read(!)            Write(!)           IoCtl(!)
  // 'FSYS'          0x00289e1a         0x00289e22         0x00289e2a         0x00289e32         0x00289e3a
  //                 fsClose            fsRead             fsWrite            fsIoctl            syFAccess (huh?)
  htonl('FSYS'), htonl((unsigned int)(&tncSyFAccess)), htonl((unsigned int)(&tncSyClose)), htonl((unsigned int)(&tncSyRead)), htonl((unsigned int)(&tncSyWrite)), htonl((unsigned int)(&tncSyIoctl)), // 'FSYS' co   file jump table?
  // 'CONS'          00 28 9d ea        00 28 9d f2        00 28 9d fa        00 28 9e 02        00 28 9e 0a
  htonl('CONS'), htonl((unsigned int)(&tncSyFAccess)), htonl((unsigned int)(&tncSyClose)), htonl((unsigned int)(&tncSyRead)), htonl((unsigned int)(&tncSyWrite)), htonl((unsigned int)(&tncSyIoctl)), // 'CONS' fs
  // 'SYST'          00 28 9e 42        00 28 9e 4a        00 28 9e 52        00 28 9e 5a        00 28 9e 62
  htonl('SYST'), htonl((unsigned int)(&tncSyFAccess)), htonl((unsigned int)(&tncSyClose)), htonl((unsigned int)(&tncSyRead)), htonl((unsigned int)(&tncSyWrite)), htonl((unsigned int)(&tncSyIoctl)), // 'SYST' sy
  htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), // ??
  htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000), htonl(0x00000000)  // ??
};


unsigned int gSomeTable1c[] =
{ // mode   error       jump table         ???                size               src or dst buffer
  /* stdin */ // htons(1), 0.w,    0.l,      0,
              //  htonl(0xdeaf0000), htonl(0xdeaf0001), htonl(0xdeaf0002), htonl(0xdeaf0003), htonl(0xdeaf0004), // stdin
              //  htonl(0xdeaf0000), htonl(0xdeaf0001), htonl(0xdeaf0002), htonl(0xdeaf0003), htonl(0xdeaf0004), // stdout
              //  htonl(0xdeaf0000), htonl(0xdeaf0001), htonl(0xdeaf0002), htonl(0xdeaf0003), htonl(0xdeaf0004)  // stderr
  htonl(0x00010000), htonl((unsigned int)(&gSomeTable20[0])), htonl((unsigned int)(&stdFiles[0])), 0, 0,
  htonl(0x00020000), htonl((unsigned int)(&gSomeTable20[0])), htonl((unsigned int)(&stdFiles[1])), 0, 0,
  htonl(0x00020000), htonl((unsigned int)(&gSomeTable20[0])), htonl((unsigned int)(&stdFiles[2])), 0, 0,
};



const char *gArgv0 = "m68kAsm";
//const char *gArgv1 = "Unix:test.s:";
const char *gArgv1 = "/Users/matt/dev/Alienate/ARM6asm/test.s";

unsigned int gArgv[] = {
  htonl((unsigned int)(gArgv0)),
  htonl((unsigned int)(gArgv1)),
  0x00000000
};

const char *gEnvp0 = "mosrun\0true";

unsigned int gEnvp[] = {
  htonl((unsigned int)(gEnvp0)),
  0x00000000
};

struct MPWMem {
  unsigned short notNull;
  unsigned int argc;
  unsigned int argv;
  unsigned int envp;
  unsigned int unknown0e;
  unsigned int unknown12;
  unsigned int unknown16;
  unsigned short fileTablesSize;
  unsigned int someTable1c;
  unsigned int someTable20;
  unsigned int unknown24;
}  __attribute__((packed));
struct MPWMem gMPWMem = {
  htons(0x5348),
  htonl(2),
  htonl((unsigned int)(gArgv)),
  htonl((unsigned int)(gEnvp)),
  0, // null
  0, // ptr
  0, // ptr
  htons(400),
  htonl((unsigned int)(&gSomeTable1c)), // some table, size is 60 bytes
  htonl((unsigned int)(&gSomeTable20)), // some Table with 5 entries, 24 bytes each (SADEV entries are 8 bytes for 16 entries, hmmmm)
  0  // ptr
};

struct {
  unsigned int id;
  unsigned int mpwPtr;
} gMacPgm = {
  htonl('MPGM'), htonl((unsigned int)(&gMPWMem))
};

/*
 Find #include <mpw.h> and inside we should find "extern MPWBLOCK * _pMPWBlock;" 
 */

unsigned int m68k_read_memory_8(unsigned int address)
{
  if (address>=0x1000)
    return *((unsigned char*)address);
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
  if (address>=0x1000)
    return ntohs(*((unsigned short*)address));
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
    unsigned int v = ntohl(*((unsigned long*)address));
    return v;
  }
  if (address<0x1e00) {
    const char *rem = "";
    const char *var = gvarName(address, &rem);
    mosLog("Read.l 0x%04x: %s %s\n", address, var, rem);
  }
  switch (address) {
    case 0: return 0;
    case 4: return 0;
    case 0x0028: return (unsigned int)(&trapDispatchTrap);
      //    case 0x0316: return 0; // MacPgm // this is MPW memory with argc and more
    case 0x0316: return (unsigned int)(&gMacPgm); // MacPgm // this is MPW memory with argc and more
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
    *((unsigned char*)address) = value;
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
    *((unsigned short*)address) = htons(value);
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
    *((unsigned long*)address) = htonl(value);
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
}


// TODO: forward argc and argv to the app we are loading
// TODO: run stdout through a Mac-to-Unix filter
// TODO: TripleDash arguments?   ARM6asm ---unix2mac myFile.s -o ---mac2unix myFile.o ---stdout2unix ---stderr2unix
int main(int argc, const char **argv, const char **envp)
{
  FILE *logFile = fopen("/Users/matt/dev/Alienate/log.txt", "wb");
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
  
  return 0;
}




