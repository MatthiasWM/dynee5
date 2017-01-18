//
//  NTSymbol.cpp
//  NOSTranslate
//
//  Created by Matthias Melcher on 1/16/17.
//  Copyright © 2017 elektriktrick. All rights reserved.
//

#include "NTSymbol.hpp"

#include "NTMemory.hpp"
#include "NTAddress.hpp"

#include <stdlib.h>


NTSymbol NTSymbolList::null("<NULL>");


NTSymbolList::NTSymbolList()
{
}


NTSymbolList::~NTSymbolList()
{
}


bool NTSymbolList::readFromAIF(const char *filename)
{
    typedef struct { uint32_t offset; uint32_t value; } Sym;
    // 71FCEC -> nSym
    // 71FCF0 sym[0] offset, value, 8 byten

    FILE *f = fopen(filename, "rb");
    fseek(f, 0x71FCEC, SEEK_SET);
    int nSym = readWord(f);
    Sym *sym = (Sym*)malloc(nSym*sizeof(Sym));
    fread(sym, nSym*sizeof(Sym), 1, f);
    size_t base = ftell(f);
    for (int i=0; i<nSym; ++i)
    {
        // type=1, value<0x1000, numeric constants
        // type=1, value<0x800000, ROM size constants
        // type=3, ROM resources (functions, data, jump tables)
        // type=5, global variables, 0x0c1...
        // type=7: global variables 0x0c1....
        uint32_t offset = htonl(sym[i].offset) & 0xffffff;
        uint32_t type = htonl(sym[i].offset) >> 24;
        uint32_t value =  htonl(sym[i].value);

        fseek(f, base+offset, SEEK_SET);
        int n = fgetc(f);
        char name[1024];
        fread(name, n, 1, f);
        name[n] = 0;
        //fprintf(s, "%d %08x %s\n", type, value, name);
        if (type!=1) {
            SymbolList.addSymbol(value, name, type);
            Mem.at(value).hasSymbol(true);
        }
    }
    free(sym);
    fclose(f);

    return true;
}


bool NTSymbolList::addSymbol(uint32_t address, const char *name, ubyte type)
{
    NTSymbolMap::iterator it = pMap.find(address);
    NTSymbol *sym = 0L;
    if (it==pMap.end()) {
        sym = new NTSymbol(name);
        pMap[address] = sym;
        sym->setType(type);
    } else {
        // We will encounter duplicate symbols. Which one should we keep?
        const char *oldName = it->second->name();
        // Difference is a leading underscor: keep the one without the underscore.
        if (oldName[0]=='_' && strcmp(oldName+1, name)==0) {
            it->second->setName(name);
        } else if (name[0]=='_' && strcmp(oldName, name+1)==0) {
            // keep the old name
        } else if (strcmp(oldName, "memcpy")==0 && strcmp(name, "memmove")==0) {
            it->second->setName(name); // memmove is a superset of memcpy
        } else if (   strncmp(oldName, "PublicFiller", 12)==0
                   || strncmp(oldName, "MP0", 3)==0) {
            // we don't care
        } else {
            if (gWarnLevel>=9)
                fprintf(stderr, "WARNING: ignoring duplicate sybol at 0x%08X: keeping '%s', discarding '%s'\n",
                        address, it->second->name(), name);
            return false;
        }
    }
    return true;
}


NTSymbol &NTSymbolList::at(uint32_t addr)
{
    NTSymbolMap::iterator it = pMap.find(addr);
    if (it==pMap.end()) {
        return NTSymbolList::null;
    } else {
        return *it->second;
    }

}



NTSymbol::NTSymbol(const char *name)
:   pName(strdup(name)),
    pType(0)
{
}


NTSymbol::~NTSymbol()
{
    if (pName) free(pName);
}


void NTSymbol::printAll()
{
    printf("    Symbol '%s', type %d\n", pName, pType);
}


const char *NTSymbol::name()
{
    return pName;
}


void NTSymbol::setName(const char *newName)
{
    if (pName) free(pName);
    pName = 0L;
    if (newName && *newName) pName = strdup(newName);
}


void NTSymbol::setType(ubyte t)
{
    pType = t;
}


