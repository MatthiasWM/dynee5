//
//  NTMemory.hpp
//  NOSTranslate
//
//  Created by Matthias Melcher on 1/13/17.
//  Copyright © 2017 elektriktrick. All rights reserved.
//

#ifndef NTMemory_hpp
#define NTMemory_hpp


#include "main.hpp"

#include <map>


class NTAddress;
typedef std::map<uint32_t, NTAddress*> NTMemoryMap;


class NTMemory
{
public:
    NTMemory();
    ~NTMemory();
    bool read(uint addr, const char *filename, uint offset, uint size);
    NTAddress &at(uint32_t addr);
private:
    ubyte *pROMImage;
    NTMemoryMap pMap;
};


#endif /* NTMemory_hpp */
