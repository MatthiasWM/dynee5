//
//  decomp.cpp
//  Albert
//
//  Created by Matthias Melcher on 01.04.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "decomp.h"
#include "db2src.h"


static int indent = 2;
static const char spc[] = "                                 ";


static void translate_single_data_swap(FILE *f, unsigned int addr, unsigned int cmd)
{
    // FIXME:
}


static void translate_multiply_and_accumulate(FILE *f, unsigned int addr, unsigned int cmd)
{
    // FIXME:
}


static void translate_multiply(FILE *f, unsigned int addr, unsigned int cmd)
{
    // FIXME:
}


static void translate_data_processing_PSR_transfer(FILE *f, unsigned int addr, unsigned int cmd)
{
    /*
	const Boolean theFlagS = (inInstruction & 0x00100000) != 0;
	KUInt32 theMode;
	KUInt32 thePushedValue;
	Boolean doPush = true;
	Boolean doPushPC = false;
	if (inInstruction & 0x02000000)
	{
		KUInt32 theImmValue = inInstruction & 0xFF;
		KUInt32 theRotateAmount = ((inInstruction >> 8) & 0xF) * 2;
		if (theRotateAmount != 0)
		{
			theImmValue = 
            (theImmValue >> theRotateAmount)
            | (theImmValue << (32 - theRotateAmount));
			if (theFlagS)
			{
				theMode = ImmC;
			} else {
				theMode = Imm;
			}
		} else {
			theMode = Imm;
		}
		thePushedValue = theImmValue;
	} else if ((inInstruction & 0x00000FFF) >> 4) {
		theMode = Regular;
		thePushedValue = inInstruction;
	} else {
		theMode = NoShift;
		thePushedValue = __Rm;
	}
    
	switch ((inInstruction & 0x01E00000) >> 21)
	{
        case 0x0:	// 0b0000
            // AND
            PUSHFUNC(AND_Func(theMode, theFlagS, __RnRd));
            doPushPC = ShouldPushPC_LogicalOp(theMode, __Rn);
            break;
            
        case 0x1:	// 0b0001
            // EOR
            PUSHFUNC(EOR_Func(theMode, theFlagS, __RnRd));
            doPushPC = ShouldPushPC_LogicalOp(theMode, __Rn);
            break;
            
        case 0x2:	// 0b0010
            // SUB
            PUSHFUNC(SUB_Func(theMode, theFlagS, __RnRd));
            doPushPC = ShouldPushPC_ArithmeticOp(theMode, __Rn);
            break;
            
        case 0x3:	// 0b0011
            // RSB
            PUSHFUNC(RSB_Func(theMode, theFlagS, __RnRd));
            doPushPC = ShouldPushPC_ArithmeticOp(theMode, __Rn);
            break;
            
        case 0x4:	// 0b01000
            // ADD
            PUSHFUNC(ADD_Func(theMode, theFlagS, __RnRd));
            doPushPC = ShouldPushPC_ArithmeticOp(theMode, __Rn);
            break;
            
        case 0x5:	// 0b01010
            // ADC
            PUSHFUNC(ADC_Func(theMode, theFlagS, __RnRd));
            doPushPC = ShouldPushPC_ArithmeticOp(theMode, __Rn);
            break;
            
        case 0x6:	// 0b01100
            // SBC
            PUSHFUNC(SBC_Func(theMode, theFlagS, __RnRd));
            doPushPC = ShouldPushPC_ArithmeticOp(theMode, __Rn);
            break;
            
        case 0x7:	// 0b0111
            // RSC
            PUSHFUNC(RSC_Func(theMode, theFlagS, __RnRd));
            doPushPC = ShouldPushPC_ArithmeticOp(theMode, __Rn);
            break;
            
        case 0x8:	// 0b1000
            // MRS (CPSR) & TST
            if (theFlagS == 0)
            {
                if (theMode != NoShift)
                {
                    // Undefined Instruction (there is no MRS with Imm bit set or low bits set)
                    PUSHFUNC(UndefinedInstruction);
                    doPush = false;
                    doPushPC = true;
                } else {
                    PUSHFUNC(MRS_Func(0, __Rd));
                    doPush = false;
                }
            } else {
                PUSHFUNC(TST_Func(theMode, __Rn));
                doPushPC = ShouldPushPC_LogicalOp(theMode, __Rn);
            }
            break;
            
        case 0x9:	// 0b1001
            // MSR (CPSR) & TEQ
            if (theFlagS == 0)
            {
                if (theMode == Regular)
                {
                    // Software breakpoint
                    KUInt16 theID = inInstruction & 0x0000000F;
                    theID |= (inInstruction & 0x000FFF00) >> 4;
                    thePushedValue = theID;
                    PUSHFUNC(SoftwareBreakpoint);
                    doPushPC = true;
                } else {
                    if (theMode == NoShift)
                    {
                        PUSHFUNC(MSR_NoShift_Func(0, (inInstruction & 0x000F0000) >> 16, __Rm));
                        doPushPC = ShouldPushPC_MSR_NoShift(__Rm);
                        doPush = false;
                    } else {
                        PUSHFUNC(MSR_Imm_Func(0, (inInstruction & 0x000F0000) >> 16));
                        doPushPC = false;
                    }
                }
            } else {
                PUSHFUNC(TEQ_Func(theMode, __Rn));
                doPushPC = ShouldPushPC_TestOp(theMode, __Rn);
            }
            break;
            
        case 0xA:	// 0b1010
            // MRS (SPSR) & CMP
            if (theFlagS == 0)
            {
                if (theMode != NoShift)
                {
                    // Undefined Instruction (there is no MRS with Imm bit set or low bits set)
                    PUSHFUNC(UndefinedInstruction);
                    doPush = false;
                    doPushPC = true;
                } else {
                    PUSHFUNC(MRS_Func(1, __Rd));
                    doPush = false;
                }
            } else {
                PUSHFUNC(CMP_Func(theMode, __Rn));
                doPushPC = ShouldPushPC_TestOp(theMode, __Rn);
            }
            break;
            
        case 0xB:	// 0b1011
            // MSR (SPSR) & CMN
            if (theFlagS == 0)
            {
                if (theMode == Regular)
                {
                    // Undefined Instruction (there is no MSR with shift)
                    PUSHFUNC(UndefinedInstruction);
                    doPush = false;
                    doPushPC = true;
                } else {
                    if (theMode == NoShift)
                    {
                        PUSHFUNC(MSR_NoShift_Func(1, (inInstruction & 0x000F0000) >> 16, __Rm));
                        doPush = false;
                    } else {
                        PUSHFUNC(MSR_Imm_Func(1, (inInstruction & 0x000F0000) >> 16));
                    }
                }
            } else {
                PUSHFUNC(CMN_Func(theMode, __Rn));
                doPushPC = ShouldPushPC_TestOp(theMode, __Rn);
            }
            break;
            
        case 0xC:	// 0b1100
            // ORR
            PUSHFUNC(ORR_Func(theMode, theFlagS, __RnRd));
            doPushPC = ShouldPushPC_LogicalOp(theMode, __Rn);
            break;
            
        case 0xD:	// 0b11010
            // MOV
            PUSHFUNC(MOV_Func(theMode, theFlagS, __Rd));
            doPushPC = ShouldPushPC_MoveOp(theMode);
            break;
            
        case 0xE:	// 0b1110
            // BIC
            PUSHFUNC(BIC_Func(theMode, theFlagS, __RnRd));
            doPushPC = ShouldPushPC_LogicalOp(theMode, __Rn);
            break;
            
        case 0xF:	// 0b11110
            // MVN
            PUSHFUNC(MVN_Func(theMode, theFlagS, __Rd));
            doPushPC = ShouldPushPC_MoveOp(theMode);
            break;
	}
	
	if (doPush)
	{
		PUSHVALUE(thePushedValue);
	}
	if (doPushPC)
	{
		PUSHVALUE(inVAddr + 8);
	}
    */
}


static void translate_00(FILE *f, unsigned int addr, unsigned int cmd)
{
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 0  0  I  --Opcode--- S  --Rn--- --Rd--- ----------Operand 2-------- Data Processing PSR Transfer
	// -Cond-- 0  0  0  0  0  0  A  S  --Rd--- --Rn--- --Rs--- 1  0  0  1  --Rm--- Multiply
	// -Cond-- 0  0  0  1  0  B  0  0  --Rn--- --Rd--- 0 0 0 0 1  0  0  1  --Rm---
	if ((cmd & 0x020000F0) == 0x90)	{
		if (cmd & 0x01000000) {
			translate_single_data_swap(f, addr, cmd);
		} else {
			if (cmd & 0x00200000) {
                translate_multiply_and_accumulate(f, addr, cmd);
			} else {
				translate_multiply(f, addr, cmd);
			}
		}
	} else {
		translate_data_processing_PSR_transfer(f, addr, cmd);
	}
}


static void translate_01(FILE *f, unsigned int addr, unsigned int cmd)
{    
}


static void translate_10(FILE *f, unsigned int addr, unsigned int cmd)
{
}


static void translate_swi_copro(FILE *f, unsigned int addr, unsigned int cmd)
{
}


static void translate(FILE *f, unsigned int addr, unsigned int cmd)
{
    unsigned int cond = cmd>>28;
    if (cond<14) {
        // FIXME: which condition?
        fprintf(f, "%.*sif (cond) {\n", indent, spc); 
        indent+=2;
    }
    
	if (cond != 15)	{ // Condition "never"
        switch ((cmd >> 26) & 0x3) {
			case 0x00: translate_00(f, addr, cmd); break;
			case 0x01: translate_01(f, addr, cmd); break;
			case 0x02: translate_10(f, addr, cmd); break;
			case 0x03: translate_swi_copro(f, addr, cmd); break;
		}
	}
    
    
    if (cond<14) {
        indent-=2;
        fprintf(f, "%.*s}\n", indent, spc);
    }
}

int decompile_function(FILE *f, unsigned int start, unsigned int end)
{
    if (!f) return -1;
    
    fprintf(f, "/*\n * File auto generated by Albert\n */\n\n");
    fprintf(f, "Fixed FixedMultiply(Fixed a, Fixed b) {\n");
    indent = 2;
    
    int addr;
    for (addr=start; addr<end; addr+=4 ) {
        unsigned int cmd = rom_w(addr);
        
        char dst[250];
        disarm(dst, addr, cmd);
        fprintf(f, "%.*s// %s\n", indent, spc, dst);
        
        translate(f, addr, cmd);
    }

    fprintf(f, "}\n");

    return 0;
}

