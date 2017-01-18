//
//  NTSymbol.hpp
//  NOSTranslate
//
//  Created by Matthias Melcher on 1/16/17.
//  Copyright © 2017 elektriktrick. All rights reserved.
//

#ifndef NTSymbol_hpp
#define NTSymbol_hpp

#include "main.hpp"

#include <map>


class NTSymbol;


typedef std::map<uint32_t, NTSymbol*> NTSymbolMap;


class NTSymbolList
{
public:
    static NTSymbol null;
public:
    NTSymbolList();
    ~NTSymbolList();
    bool readFromAIF(const char *filename);
    bool addSymbol(uint32_t address, const char *name, ubyte type);
    NTSymbol &at(uint32_t address);
private:
    NTSymbolMap pMap;
};


class NTSymbol
{
public:
    NTSymbol(const char *name);
    ~NTSymbol();
    void printAll();
    const char *name();
    void setName(const char *newName);
    void setType(ubyte t);
private:
    char *pName;
    ubyte pType;
};

#endif /* NTSymbol_hpp */
