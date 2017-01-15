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


class NTMemory
{
public:
    NTMemory();
    ~NTMemory();
    bool read(uint addr, const char *filename, uint offset, uint size);
private:
    ubyte *pROMImage;
};


#endif /* NTMemory_hpp */
