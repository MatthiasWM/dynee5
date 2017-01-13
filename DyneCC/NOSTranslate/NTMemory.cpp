//
//  NTMemory.cpp
//  NOSTranslate
//
//  Created by Matthias Melcher on 1/13/17.
//  Copyright © 2017 elektriktrick. All rights reserved.
//

#include "NTMemory.hpp"


NTMemory::NTMemory()
{
    // ROM from 0x00000000 to 0x00800000 (8MB)
    // possible second ROM for platform vectors
    // jump table from 0x01A00000 to 0x01C1085C
    // Flash Bank 1: 0x02000000 to 0x02400000
    // Flash Bank 2:
    // RAM start: 0x04000000
}


NTMemory::~NTMemory()
{
}


bool NTMemory::read(uint addr, const char *filename, uint offset, uint size)
{
    return false;
}


