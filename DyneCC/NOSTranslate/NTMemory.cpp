//
//  NTMemory.cpp
//  NOSTranslate
//
//  Created by Matthias Melcher on 1/13/17.
//  Copyright © 2017 elektriktrick. All rights reserved.
//

#include "NTMemory.hpp"

#include "NTAddress.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


NTMemory::NTMemory()
{
    // ROM from 0x00000000 to 0x00800000 (8MB)
    // possible second ROM for platform vectors
    // jump table from 0x01A00000 to 0x01C1085C
    // Flash Bank 1: 0x02000000 to 0x02400000
    // Flash Bank 2:
    // RAM start: 0x04000000
    // gParamBlockFromImagePhysical, _start+0x04006400
    // gParamBlockFromImage, _start+0x0C008400
    // gADCChargeWait, _start+0x0C100800
    // _end, _start+0x0C107E14
//    .equ    VEC_kHardwareBase, _start+0x0F000000
//    .equ    VEC_kHdWr_DMAChan1Base, _start+0x0F080000
//    .equ    VEC_kHdWr_DMAAssgmnt, _start+0x0F08FC00
//    .equ    VEC_kHdWr_DMAChan2Base, _start+0x0F090000
//    .equ    VEC_kHdWr_DMAEnableStat, _start+0x0F098000
//    .equ    VEC_kHdWr_DMADisable, _start+0x0F098400
//    .equ    VEC_kHdWr_DMAWordStat, _start+0x0F098800
//    .equ    VEC_kHdWr_HighSpeedClck, _start+0x0F110400
//    .equ    VEC_kHdWr_CalendarReg, _start+0x0F181000
//    .equ    VEC_kHdWr_AlarmReg, _start+0x0F181400
//    .equ    VEC_kHdWr_Ticks, _start+0x0F181800
//    .equ    VEC_kHdWr_MatchReg0_FIQ, _start+0x0F182000
//    .equ    VEC_kHdWr_MatchReg1_IRQ, _start+0x0F182400
//    .equ    VEC_kHdWr_MatchReg2_Timer, _start+0x0F182800
//    .equ    VEC_kHdWr_MatchReg3_Sched, _start+0x0F182C00
//    .equ    VEC_kHdWr_IntPresent, _start+0x0F183000
//    .equ    VEC_kHdWr_IntCtrlReg, _start+0x0F183400
//    .equ    VEC_kHdWr_IntClear, _start+0x0F183800
//    .equ    VEC_kHdWr_FIQMaskReg, _start+0x0F183C00
//    .equ    VEC_kHdWr_IntEDReg1, _start+0x0F184000
//    .equ    VEC_kROMSerialChip, _start+0x0F243000
//    .equ    VEC_kFlashBank2, _start+0x10000000
//    .equ    VEC_kFlashBank2End, _start+0x10400000
//    .equ    VEC_kPCMCIA0Base, _start+0x30000000
//    .equ    VEC_kPCMCIA1Base, _start+0x40000000
//    .equ    VEC_kPCMCIA2Base, _start+0x50000000
//    .equ    VEC_kPCMCIA3Base, _start+0x60000000
//    .equ    VEC_kPCMCIA3End, _start+0x70000000

    // allocate memory to hold teh ROM image
    pROMImage = (ubyte*)malloc(0x00800000);
    memset(pROMImage, 0xff, 0x00800000);
}


NTMemory::~NTMemory()
{
    if (pROMImage) free(pROMImage);
}


bool NTMemory::read(uint addr, const char *filename, uint offset, uint size)
{
    if (addr>0x00800000 || addr+size>=0x00800000) {
        fprintf(stderr, "ERROR: reading image data will be out of bounds\n");
        return false;
    }
    FILE *f = fopen(filename, "rb");
    // TODO: check for error
    fseek(f, offset, SEEK_SET);
    fread(pROMImage+addr, size, 1, f);
    // TODO: check for error
    fclose(f);
    return true;
}


NTAddress &NTMemory::at(uint32_t addr)
{
    NTMemoryMap::iterator it = pMap.find(addr);
    if (it==pMap.end()) {
        NTAddress *a = new NTAddress(addr);
        pMap[addr] = a;
        return *a;
    } else {
        return *it->second;
    }
}


void NTMemory::printAll()
{
    NTMemoryMap::iterator it = pMap.begin();
    for ( ; it!=pMap.end(); ++it)
    {
        NTAddress *a = it->second;
        printf("At 0x%08x:\n", it->first);
        a->printAll();
    }
}


uint32_t NTMemory::getWordAt(uint32_t addr)
{
    remapAddress(addr);
    if (addr>0x00800000) return 0xDEADBEEF;
    return (pROMImage[addr]<<24)|(pROMImage[addr+1]<<16)|(pROMImage[addr+2]<<8)|(pROMImage[addr+3]);
}


uint16_t NTMemory::getShortAt(uint32_t addr)
{
    remapAddress(addr);
    if (addr>0x00800000) return 0xDEAD;
    return (pROMImage[addr]<<8)|(pROMImage[addr+1]);
}


uint8_t  NTMemory::getByteAt(uint32_t addr)
{
    remapAddress(addr);
    if (addr>0x00800000) return 0xDB;
    return pROMImage[addr];
}


void NTMemory::remapAddress(uint32_t &addr)
{
    if (addr>=0x01A00000 && addr<0x01D00000) {
        // map the ROM patch table into RAM
        addr = ( ((addr>>5)&0xffffff80) | (addr&0x0000007f) ) - 0xCE000;
    }
}


void NTMemory::setJumpTable(uint32_t start, uint32_t end)
{
    if (end==0x00000000) end = start+4;
    uint32_t i;
    for (i=start; i<end; i+=4) {
        Mem.at(i).setJumpTableBranch();
    }
}



NTMemChunk::NTMemChunk(uint32_t first) :
    pParent(0L),
    pFirst(first),
    pLast(0),
    pLastSet(false)
{
}


NTMemChunk::~NTMemChunk()
{
}


NTMemChunk *NTMemChunk::endsAt(uint32_t last)
{
    if (pParent) throw "ERROR: can't change size while chunk is linked!";
    pLast = last;
    pLastSet = true;
    if (pLast<pFirst) throw "ERROR: last address is smaller than first address!";
    return this;
}


NTMemChunk *NTMemChunk::to(uint32_t next)
{
    if (pParent) throw "ERROR: can't change size while chunk is linked!";
    pLast = next-1;
    pLastSet = true;
    if (pLast<pFirst) throw "ERROR: next address is smaller than first address!";
    return this;
}


NTMemChunk *NTMemChunk::size(uint32_t size)
{
    if (pParent) throw "ERROR: can't change size while chunk is linked!";
    pLast = pFirst+size-1;
    pLastSet = true;
    if (pLast<pFirst) throw "ERROR: chunk extends beyon maximum memory size!";
    return this;
}


bool NTMemChunk::equals(NTMemChunk *other)
{
    if (!pLastSet) throw "ERROR: this chunk has an unknown range";
    if (!other->pLastSet) throw "ERROR: other chunk has an unknown range";
    return ((pFirst==other->pFirst) && (pLast==other->pLast));
}


bool NTMemChunk::contains(NTMemChunk *other)
{
    if (!pLastSet) throw "ERROR: this chunk has an unknown range";
    if (!other->pLastSet) throw "ERROR: other chunk has an unknown range";
    return ((pFirst<=other->pFirst) && (pLast>=other->pLast));
}


bool NTMemChunk::overlaps(NTMemChunk *other)
{
    if (!pLastSet) throw "ERROR: this chunk has an unknown range";
    if (!other->pLastSet) throw "ERROR: other chunk has an unknown range";
    return (   (other->pFirst<pFirst && other->pLast>=pFirst && other->pLast<pLast)
            || (other->pFirst>pFirst && other->pFirst<=pLast && other->pLast>pLast) );
}


bool NTMemChunk::isBefore(NTMemChunk *other)
{
    if (!pLastSet) throw "ERROR: this chunk has an unknown range";
    if (!other->pLastSet) throw "ERROR: other chunk has an unknown range";
    return (pLast<other->pFirst);
}


bool NTMemChunk::isAfter(NTMemChunk *other)
{
    if (!pLastSet) throw "ERROR: this chunk has an unknown range";
    if (!other->pLastSet) throw "ERROR: other chunk has an unknown range";
    return (pFirst>other->pLast);
}



NTMemChunk *NTMemChunk::add(NTMemChunk *child, int where)
{
    // make sure that all sizes are known
    if (!pLastSet) throw "ERROR: this chunk has an unknown range";
    if (!child->pLastSet) throw "ERROR: other chunk has an unknown range";
    // the new range must not partially overlap an existing entry; this should throw an exception
    if (!this->contains(child)) throw "ERROR: proposed child chunk is outside of this chunk.";
    // add a chunk inside this chunk as a child
    // - easy case: no chunks listed yet
    if (pMap.empty()) {
        pMap[child->pFirst] = child;
        return child;
    }
    // - complex case: make sure we do not overlap an existing chunk
    for (auto i=pMap.begin(); i!=pMap.end(); ) {
        NTMemChunk *other = i->second;
        if (other->isBefore(child)) {
            ++i; continue; // no need to look at this chunk at all
        } else if (other->isAfter(child)) {
            break; // all cases tested; end the loop and add the new child
        } else if (other->equals(child)) {
            if (where==kAddAbove) {
                i->second = child;
                child->add(other);
            } else if (where==kAddBelow) {
                other->add(child);
            } else {
                throw "ERROR: equals size chunks stacking order not defined";
            }  // TODO: if 'child' or 'other' contains children, the required order becomes clear
        } else if (other->overlaps(child)) {
            throw "ERROR: child chunk must not overlap any other chunk";
        } else if (child->contains(other)) {
            auto j = i; ++i;
            pMap.erase(j);
            child->add(other);
            continue;
        } else {
            throw "ERROR: unexpected condition!";
        }
    }
    pMap[child->pFirst] = child;
    return child;
}


void NTMemChunk::explore()
{
    // nothing to do
}


void NTMemChunk::print(int indent)
{
    printIndent(indent);
    printf("%s from 0x%08X to 0x%08X\n", type(), pFirst, pLast+1);
    printChildren(indent);
}


void NTMemChunk::printChildren(int indent)
{
    if (pMap.empty()) {
        printIndent(indent);
        printf("    -- empty --\n");
    } else {
        printIndent(indent);
        printf("{\n");
        for (auto i=pMap.begin(); i!=pMap.end(); ++i) {
            i->second->print(indent+4);
        }
        printIndent(indent);
        printf("}\n");
    }
}


void NTMemChunk::printIndent(int i)
{
    for ( ;i>0;--i) {
        putchar(' ');
    }
}


NTMemData_RExHeader::NTMemData_RExHeader(uint32_t first)
:   NTMemChunk(first),
    nEntries(0)
{
    if (Mem.getWordAt(first)!='RExB' || Mem.getWordAt(first+4)!='lock')
        throw "ERROR: NTMemData_RExHeader not pointing at a REx Block.";
    nEntries = Mem.getWordAt( first + 4*9 );
    size(10*4 + nEntries*12);
}


void NTMemData_RExHeader::explore()
{
    // TODO: create the config entries and explore those...
}


void NTMemData_RExHeader::print(int indent)
{
    printIndent(indent); printf("%s from 0x%08X to 0x%08X (%d entries)\n", type(), pFirst, pLast+1, nEntries);

    printIndent(indent+4); printf("ULong signatureA;    // signature so we can scan for them\n");
    printIndent(indent+4); printf("ULong signatureB;\n");
    printIndent(indent+4); printf("ULong checksum;			// checksum of this block, or kUndefined\n");
    printIndent(indent+4); printf("ULong headerVersion;\n");
    printIndent(indent+4); printf("ULong manufacturer;		// for id'ing patches\n");
    printIndent(indent+4); printf("Fixed version;			// for id'ing patches\n");
    printIndent(indent+4); printf("ULong length;			// size of the rom extension block (including header)\n");
    printIndent(indent+4); printf("ULong id;				// extension ID (0..kMaxROMExtensions-1)\n");
    printIndent(indent+4); printf("VAddr start;			// virtual address of the top of this block\n");
    printIndent(indent+4); printf("ULong count;			// number of config entries\n");
    printIndent(indent+4); printf("ConfigEntry table[1];	// table of config entries, will be more than one\n");

    for (auto i=0; i<nEntries; ++i) {
        auto a = pFirst + 4*10 + 12*i;
        printIndent(indent+4); printf("ConfigEntry[%d] = {\n", i);
        printIndent(indent+8); printf("ULong tag = '%c%c%c%c'\n", Mem.getByteAt(a), Mem.getByteAt(a+1), Mem.getByteAt(a+2), Mem.getByteAt(a+3));
        printIndent(indent+8); printf("ULong offset = %d\n", Mem.getWordAt(a+4));
        printIndent(indent+8); printf("ULong length = %d\n", Mem.getWordAt(a+8));
        printIndent(indent+4); printf("}\n");
    }
}


