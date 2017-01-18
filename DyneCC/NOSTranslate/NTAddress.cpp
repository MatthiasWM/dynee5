//
//  NTAddress.cpp
//  NOSTranslate
//
//  Created by Matthias Melcher on 1/16/17.
//  Copyright © 2017 elektriktrick. All rights reserved.
//

#include "NTAddress.hpp"

#include "NTSymbol.hpp"


NTAddress::NTAddress(uint32_t addr)
:   pAddr(addr),
    pHasSymbol(false)
{
}


void NTAddress::printAll()
{
    if (hasSymbol()) {
        printf("  has a symbol:\n");
        SymbolList.at(pAddr).printAll();
    }
}


bool NTAddress::hasSymbol()
{
    return pHasSymbol;
}


void NTAddress::hasSymbol(bool v)
{
    pHasSymbol = v;
}
