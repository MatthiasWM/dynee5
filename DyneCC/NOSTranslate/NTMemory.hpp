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
    bool        read(uint addr, const char *filename, uint offset, uint size);
    NTAddress  &at(uint32_t addr);
    void        printAll();
    uint32_t    getWordAt(uint32_t addr);
    uint16_t    getShortAt(uint32_t addr);
    uint8_t     getByteAt(uint32_t addr);
    void        setJumpTable(uint32_t start, uint32_t end=0x00000000);
private:
    void        remapAddress(uint32_t &addr);
    ubyte      *pROMImage;
    NTMemoryMap pMap;
};


class NTMemChunk;

typedef std::map<uint32_t, NTMemChunk*> NTMemChunkMap;

enum {
    kAddThrow = 0,
    kAddAbove,
    kAddBelow
};

/**
 Marks a chunk of memory that has a fixed address and ist at least one byte in size.
 
 An NTMemChunk can contain any number of other chunks. Chunks are always sorted
 by their start address. Chunks within a chunk may never overlap each other. 
 They may also never leave the address range of their parent.
 */
class NTMemChunk
{
public:
    // -- create or destroy a chunk
    NTMemChunk(uint32_t first);
    virtual ~NTMemChunk();

    // -- set the memory range of a chunk
    NTMemChunk *endsAt(uint32_t);
    NTMemChunk *to(uint32_t);
    NTMemChunk *size(uint32_t);

    // -- compare chunks
    bool equals(NTMemChunk*);
    bool contains(NTMemChunk*);
    bool overlaps(NTMemChunk*);
    bool isBefore(NTMemChunk*);
    bool isAfter(NTMemChunk*);

    // -- add a new chunk as a child to this chunk
    NTMemChunk *add(NTMemChunk*, int where=kAddThrow);

    // -- exploration
    virtual void explore();

    //void setName(const char *name);
    //void setComment(const char *comment);
    //void addComment(const char *comment);
    //virtual int explore();
    virtual const char *type() { return "Generic Memory Chunk"; }
    virtual void print(int indent=0);
protected:
    void printIndent(int i);
    void printChildren(int indent);
protected:
    NTMemChunk *pParent;
    uint32_t pFirst;
    uint32_t pLast;
    ubyte pLastSet:1;
    NTMemChunkMap pMap;
};


class NTMemData_RExHeader : public NTMemChunk
{
public:
    NTMemData_RExHeader(uint32_t first);
    virtual void explore();
    virtual const char *type() { return "Data: struct RExHeader"; }
    virtual void print(int indent=0);
protected:
    uint32_t nEntries;
};


#endif /* NTMemory_hpp */
