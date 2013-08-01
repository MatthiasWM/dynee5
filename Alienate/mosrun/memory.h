//
//  memory.h
//  mosrun
//
//  Created by Matthias Melcher on 31.07.13.
//  Copyright (c) 2013 Matthias Melcher. All rights reserved.
//

#ifndef __mosrun__memory__
#define __mosrun__memory__


#include "main.h"

void mosWrite32(mosPtr addr, unsigned int value);
void mosWrite16(mosPtr addr, unsigned short value);
void mosWrite8(mosPtr addr, unsigned char value);

unsigned int mosRead32(mosPtr addr);
unsigned short mosRead16(mosPtr addr);
unsigned char mosRead8(mosPtr addr);

void mosWriteUnsafe32(mosPtr addr, unsigned int value);
void mosWriteUnsafe16(mosPtr addr, unsigned short value);
void mosWriteUnsafe8(mosPtr addr, unsigned char value);

unsigned int mosReadUnsafe32(mosPtr addr);
unsigned short mosReadUnsafe16(mosPtr addr);
unsigned char mosReadUnsafe8(mosPtr addr);

mosPtr mosNewPtr(unsigned int size);
mosPtr mosNewPtr(const char *text);
void mosDisposePtr(mosPtr);
unsigned int mosPtrSize(mosPtr);

mosHandle mosNewHandle(unsigned int size);
void mosDisposeHandle(mosHandle);

unsigned int mosCheckBounds(mosPtr, unsigned int size);

mosPtr mosToPtr(struct MosBlock *block);
struct MosBlock *mosToBlock(mosPtr ptr);

mosHandle mosRecoverHandle(mosPtr);


#ifdef MOS_UNITTESTS
void mosMemoeryUnittests();
#endif

#endif /* defined(__mosrun__memory__) */
