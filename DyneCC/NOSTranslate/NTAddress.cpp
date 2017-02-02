//
//  NTAddress.cpp
//  NOSTranslate
//
//  Created by Matthias Melcher on 1/16/17.
//  Copyright © 2017 elektriktrick. All rights reserved.
//

#include "NTAddress.hpp"

#include "NTSymbol.hpp"
#include "NTMemory.hpp"

#include <stdarg.h>


uint32_t branchAddress(uint32_t addr, uint32_t cmd=0xFFFFFFFF)
{
    if (cmd==0xffffffff)
        cmd = Mem.getWordAt(addr);
    if (cmd&0x00800000) { // jump backwards
        return (((cmd&0x00ffffff)|0xff000000)<<2)+addr+8;
    } else { // jump forward
        return ((cmd&0x007fffff)<<2)+addr+8;
    }
}

uint32_t branchAddressInROM(unsigned int addr, unsigned int cmd)
{
    uint32_t dest = branchAddress(addr);
    if (dest>=0x01A00000 && dest<0x1D00000) {
        dest = branchAddress(dest);
    }
    return dest;
}



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
    if (isJumpTableBranch()) {
        printf("  jump table brach instruction\n");
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


bool NTAddress::disassemble(char *buf)
{
    return handle(kAddrDisassemble, buf);
}


uint32_t NTAddress::handle(int cmd, ...)
{
    char *buf = 0;

    va_list ap;
    va_start(ap, cmd);
    switch (cmd) {
        case kAddrDisassemble: buf = va_arg(ap, char *); break;
        default: break;
    }
    va_end(ap);

    uint32_t addr = pAddr;
    uint32_t data = Mem.getWordAt(addr);
    uint32_t cond = data>>28;
    uint32_t code =  data & 0x0FFFFFFF;

    if (cond!=15) {
        if ( (code&0x0E000000)==0x0A000000 ) {  // b, bl
            uint32_t dest = branchAddressInROM(addr, data);
            ubyte L = (cmd>>24)&1;
            if (L) {
                switch (cmd) {
                    case kAddrDisassemble: sprintf(buf, "bl      0x%08X", dest); break; // TODO: print the symbol as well
                }
            } else {
                switch (cmd) {
                    case kAddrDisassemble: sprintf(buf, "b       0x%08X", dest); break; // TODO: print the symbol as well
                }
            }
            return 1;
        }
    }

    switch (cmd) {
        case kAddrDisassemble:
            sprintf(buf, "Unknown instruction 0x%08x at 0x%08x", data, addr);
            break;
        default:
            break;
    }
    return 0;
}



