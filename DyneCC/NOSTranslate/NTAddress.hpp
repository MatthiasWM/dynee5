//
//  NTAddress.hpp
//  NOSTranslate
//
//  Created by Matthias Melcher on 1/16/17.
//  Copyright © 2017 elektriktrick. All rights reserved.
//

#ifndef NTAddress_hpp
#define NTAddress_hpp

#include "main.hpp"

class NTAddress
{
public:
    NTAddress(uint32_t a);
    bool hasSymbol();
    void hasSymbol(bool v);
    void printAll();
    
    bool isFunction();
    bool isCFunction();
    bool isCppFunction();
    bool isScript(); //...
private:
    uint32_t pAddr;
    int pHasSymbol:1;
};

#endif /* NTAddress_hpp */
