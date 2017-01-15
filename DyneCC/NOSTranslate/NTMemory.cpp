//
//  NTMemory.cpp
//  NOSTranslate
//
//  Created by Matthias Melcher on 1/13/17.
//  Copyright © 2017 elektriktrick. All rights reserved.
//

#include "NTMemory.hpp"

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
    fread(pROMImage+addr, size, 1, f);
    // TODO: check for error
    fclose(f);
    return true;
}


