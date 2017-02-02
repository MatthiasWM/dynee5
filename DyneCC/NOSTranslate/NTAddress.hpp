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
    void setJumpTableBranch() { pIsARMCode = 1; pIsJumpTableBranch = 1; }
    bool isJumpTableBranch() { return pIsJumpTableBranch; }
    void setARMCode() { pIsARMCode = 1; }
    bool isARMCode() { return pIsARMCode; }
    bool disassemble(char*);
public:
    enum {
        kAddrDisassemble    // char[]
    };
    uint32_t handle(int cmd, ...);
private:
    uint32_t pAddr;
    unsigned pHasSymbol:1;
    unsigned pIsJumpTableBranch:1;
    unsigned pIsARMCode:1;
};

#endif /* NTAddress_hpp */
