//
//  memory.cpp
//  mosrun
//
//  Created by Matthias Melcher on 31.07.13.
//  Copyright (c) 2013 Matthias Melcher. All rights reserved.
//

#include "memory.h"
#include "log.h"

#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>


typedef struct MosBlock {
  struct MosBlock *hNext;
  struct MosBlock *hPrev;
  unsigned int hSize;
  unsigned int hFlags;
} MosBlock;


class MosBlockList
{
public:
  MosBlockList();
  ~MosBlockList();
  void link(MosBlock*);
  void unlink(MosBlock*);
  MosBlock *first() { return hFirst; }
  MosBlock *next(MosBlock *me) { return me->hNext; }
  bool contains(MosBlock *me);
  bool contains(mosPtr, unsigned int size);
private:
  MosBlock *hFirst;
  MosBlock *hLast;
};


MosBlockList gMemList;
MosBlockList gHandleList;


MosBlockList::MosBlockList()
{
  hFirst = 0;
  hLast = 0;
}


MosBlockList::~MosBlockList()
{
  MosBlock *me = first();
  while (me) {
    MosBlock *nextMe = next(me);
    free(me);
    me = nextMe;
  }
  hFirst = 0;
  hLast = 0;
}


void MosBlockList::link(MosBlock *me)
{
  MosBlock *prev = hLast;
  if (prev) {
    prev->hNext = me;
    hLast = me;
    me->hPrev = prev;
    me->hNext = 0;
  } else {
    hFirst = me;
    hLast = me;
    me->hPrev = 0;
    me->hNext = 0;
  }
}


void MosBlockList::unlink(MosBlock *me)
{
  MosBlock *prev = me->hPrev;
  MosBlock *next = me->hNext;
  if (prev) {
    prev->hNext = next;
  } else {
    hFirst = next;
  }
  if (next) {
    next->hPrev = prev;
  } else {
    hLast = prev;
  }
  me->hPrev = 0;
  me->hNext = 0;
}


bool MosBlockList::contains(MosBlock *me)
{
  MosBlock *b = first();
  while (b) {
    if (b==me) return true;
    b = next(b);
  }
  return false;
}


bool MosBlockList::contains(mosPtr start, unsigned int size)
{
  MosBlock *me = first();
  while (me) {
    unsigned int bStart = mosToPtr(me);
    unsigned int bSize = me->hSize;
    if ( (start>=bStart) && (start+size<=bStart+bSize))
      return true;
    me = next(me);
  }
  return false;
}


mosPtr mosNewPtr(const char *text)
{
  int size = strlen(text)+1;
  mosPtr p = mosNewPtr(size);
  memcpy((void*)p, text, size);
  return p;
}


mosPtr mosNewPtr(unsigned int size)
{
  MosBlock *me = (MosBlock*)calloc(1, size+sizeof(MosBlock));
  me->hSize = size;
  //printf("    NewPtr 0x%08X = 0x%08X (%d)\n", me, mosToPtr(me), size);
  gMemList.link(me);
  return mosToPtr(me);
}


void mosDisposePtr(mosPtr ptr)
{
  if (!ptr) return;
  
  MosBlock *me = mosToBlock(ptr);
  //printf("DisposePtr 0x%08X = 0x%08X\n", me, ptr);
  if (!gMemList.contains(me)) {
    mosError("Disposing unknown Ptr at 0x%08X!\n", ptr);
    return;
  }
  
  gMemList.unlink(me);
  free(me);
}


unsigned int mosPtrSize(mosPtr ptr)
{
  MosBlock *me = mosToBlock(ptr);
  return me->hSize;
}


mosHandle mosNewHandle(unsigned int size)
{
  mosPtr ptr = 0;
  if (size) ptr = mosNewPtr(size);
  
  MosBlock *me = (MosBlock*)calloc(1, sizeof(MosBlock)+4);
  me->hSize = 4;
  gHandleList.link(me);
  
  mosHandle hdl = mosToPtr(me);
  mosWrite32(hdl, ptr);
  
  return hdl;
}


void mosDisposeHandle(mosHandle hdl)
{
  if (!hdl) return;
  
  mosPtr ptr = mosRead32(hdl);
  if (ptr) {
    mosDisposePtr(ptr);
  }
  
  MosBlock *me = mosToBlock(hdl);
  if (!gHandleList.contains(me)) {
    mosError("Disposing unknown Handle at 0x%08X!\n", hdl);
    return;
  }
  
  gHandleList.unlink(me);
  free(me);
}


void mosWriteUnsafe32(mosPtr addr, unsigned int value)
{
  *((unsigned int*)(addr)) = htonl(value);
}

void mosWrite32(mosPtr addr, unsigned int value)
{
#if MOS_BOUNDS_CHECK
  if (!gHandleList.contains(addr, 4) && !gMemList.contains(addr, 4)) {
    mosError("Attempt to write 4 bytes to unknown address 0x%08X (0x%08x)!\n", addr, value);
  }
#endif
  mosWriteUnsafe32(addr, value);
}


void mosWriteUnsafe16(mosPtr addr, unsigned short value)
{
  *((unsigned short*)(addr)) = htons(value);
}

void mosWrite16(mosPtr addr, unsigned short value)
{
#if MOS_BOUNDS_CHECK
  if (!gHandleList.contains(addr, 2) && !gMemList.contains(addr, 2)) {
    mosError("Attempt to write 2 bytes to unknown address 0x%08X (0x%04x)!\n", addr, value);
  }
#endif
  mosWriteUnsafe16(addr, value);
}


void mosWriteUnsafe8(mosPtr addr, unsigned char value)
{
  *((unsigned char*)(addr)) = value;
}

void mosWrite8(mosPtr addr, unsigned char value)
{
#if MOS_BOUNDS_CHECK
  if (!gHandleList.contains(addr, 1) && !gMemList.contains(addr, 1)) {
    mosError("Attempt to write 1 byte to unknown address 0x%08X (0x%02x)!\n", addr, value);
  }
#endif
  mosWriteUnsafe8(addr, value);
}



unsigned int mosReadUnsafe32(mosPtr addr)
{
  return htonl(*((unsigned int*)(addr)));
}

unsigned int mosRead32(mosPtr addr)
{
#if MOS_BOUNDS_CHECK
  if (!gHandleList.contains(addr, 4) && !gMemList.contains(addr, 4)) {
    mosError("Attempt to read 4 bytes from unknown address 0x%08X!\n", addr);
  }
#endif
  return mosReadUnsafe32(addr);
}


unsigned short mosReadUnsafe16(mosPtr addr)
{
  return htons(*((unsigned short*)(addr)));
}

unsigned short mosRead16(mosPtr addr)
{
#if MOS_BOUNDS_CHECK
  if (!gHandleList.contains(addr, 2) && !gMemList.contains(addr, 2)) {
    mosError("Attempt to read 2 bytes from unknown address 0x%08X!\n", addr);
  }
#endif
  return mosReadUnsafe16(addr);
}


unsigned char mosReadUnsafe8(mosPtr addr)
{
  return *((unsigned char*)(addr));
}

unsigned char mosRead8(mosPtr addr)
{
#if MOS_BOUNDS_CHECK
  if (!gHandleList.contains(addr, 1) && !gMemList.contains(addr, 1)) {
    mosError("Attempt to read 2 bytes from unknown address 0x%08X!\n", addr);
  }
#endif
  return mosReadUnsafe8(addr);
}


mosPtr mosToPtr(struct MosBlock *block)
{
  return ((unsigned int)(block)) + sizeof(MosBlock);
}

struct MosBlock *mosToBlock(mosPtr ptr)
{
  return ((MosBlock*)(ptr - sizeof(MosBlock)));
}


#ifdef MOS_UNITTESTS
void mosMemoeryUnittests()
{
  mosPtr p1 = mosNewPtr(100);
  mosPtr p2 = mosNewPtr(200);
  MosBlock *me = gMemList.first();
  if (mosToPtr(me)!=p1) printf("ERROR: expected p1 to be the first block\n");
  if (me->hPrev!=0L) printf("ERROR: p1->prev should be NULL\n");
  if (mosToPtr(me->hNext)!=p2) printf("ERROR: p1->next should be p2\n");
  me = gMemList.next(me);
  if (mosToPtr(me)!=p2) printf("ERROR: expected p2 to be the second block\n");
  if (mosToPtr(me->hPrev)!=p1) printf("ERROR: p2->prev should be p1\n");
  if (me->hNext!=0L) printf("ERROR: p2->next should be NULL\n");
  if (!gMemList.contains(mosToBlock(p1))) printf("ERROR: List does not contain p1\n");
  if (!gMemList.contains(mosToBlock(p2))) printf("ERROR: List does not contain p2\n");
  if (gMemList.contains(mosToBlock(42))) printf("ERROR: List should not contain 42\n");
  mosPtr p3 = mosNewPtr(200);
  mosDisposePtr(p2);
  me = gMemList.first();
  if (mosToPtr(me)!=p1) printf("ERROR: 2 - expected p1 to be the first block\n");
  if (me->hPrev!=0L) printf("ERROR: 2 - p1->prev should be NULL\n");
  if (mosToPtr(me->hNext)!=p3) printf("ERROR: 2 - p1->next should be p3\n");
  me = gMemList.next(me);
  if (mosToPtr(me)!=p3) printf("ERROR: 2 - expected p3 to be the second block\n");
  if (mosToPtr(me->hPrev)!=p1) printf("ERROR: 2 - p3->prev should be p1\n");
  if (me->hNext!=0L) printf("ERROR: p3->next should be NULL\n");
}
#endif
