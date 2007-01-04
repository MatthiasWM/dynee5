/*  armdefs.h -- ARMulator common definitions:  ARM6 Instruction Emulator.
    Copyright (C) 1994 Advanced RISC Machines Ltd.
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#ifndef _ARMDEFS_H_
#define _ARMDEFS_H_

#define MEMTYPE_IO              0
#define MEMTYPE_RAM             1
#define MEMTYPE_ROM             2
#define MEMTYPE_FLASH           3

#define ATTRIBUTE_UNUSED

#define SA1110        0x6901b110
#define SA1100        0x4401a100
#define PXA250        0x69052100
#define PXA270        0x69054110

// koodailar add this for mingw 2005.12.18--------------------------------------
#ifdef __MINGW32__
#define fd_set int
#define FD_ZERO(x) ((*x)=0)
#define FD_SET(a,b) (1)
#define FD_ISSET(a,b) (1)

#endif
//end --------------------------------------------------------------------------

		


		
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef WIN32
#else
# include <sys/mman.h>
# include <termios.h>
# include <unistd.h>
# include <sys/time.h>
# include <stdint.h>
#endif

//AJ2D--------------------------------------------------------------------------

//teawater add DBCT_TEST_SPEED 2005.10.04---------------------------------------
#include <signal.h>
//#define DBCT_TEST_SPEED
#define DBCT_TEST_SPEED_SEC	10
//AJ2D--------------------------------------------------------------------------

//teawater add compile switch for DBCT GDB RSP function 2005.10.21--------------
//#define DBCT_GDBRSP
//AJ2D--------------------------------------------------------------------------

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif
#define LOW 0
#define HIGH 1
#define LOWHIGH 1
#define HIGHLOW 2

#define ARM_BYTE_TYPE 		0
#define ARM_HALFWORD_TYPE 	1
#define ARM_WORD_TYPE 		2

//the define of cachetype
#define NONCACHE  0
#define DATACACHE  1
#define INSTCACHE  2

#ifndef __STDC__
typedef char *VoidStar;
#endif

#ifdef WIN32
  typedef unsigned __int64 ARMdword;	/* must be 64 bits wide */
  typedef   signed __int64 ARMsdword;	/* must be 64 bits wide */
  typedef unsigned __int32 ARMword;	/* must be 32 bits wide */
  typedef   signed __int32 ARMsword;	/* must be 32 bits wide */
  typedef unsigned __int8 ARMbyte;	/* must be 8 bits wide */
  typedef unsigned __int16 ARMhword;	/* must be 16 bits wide */
  typedef unsigned __int32 uint32_t;
# define ULL(a) ((ARMdword)(a))
#else
  typedef unsigned long long ARMdword;	/* must be 64 bits wide */
  typedef   signed long long ARMsdword;	/* must be 64 bits wide */
  typedef unsigned int ARMword;	/* must be 32 bits wide */
  typedef   signed int ARMsword;	/* must be 32 bits wide */
  typedef unsigned char ARMbyte;	/* must be 8 bits wide */
  typedef unsigned short ARMhword;	/* must be 16 bits wide */
# define ULL(a) a##ULL
#endif

typedef struct ARMul_State ARMul_State;
typedef struct ARMul_io ARMul_io;
typedef struct ARMul_Energy ARMul_Energy;

//teawater add for arm2x86 2005.06.24-------------------------------------------
//AJ2D--------------------------------------------------------------------------
/*
//chy 2005-05-11
#ifndef __CYGWIN__
//teawater add for arm2x86 2005.02.14-------------------------------------------
typedef unsigned char           uint8_t;
typedef unsigned short          uint16_t;
typedef unsigned int            uint32_t;
#if defined (__x86_64__)
typedef unsigned long           uint64_t;
#else
typedef unsigned long long      uint64_t;
#endif
////AJ2D--------------------------------------------------------------------------
#endif
*/

//koodailar add it for mingw -----------------------------------------------------
#ifdef __MINGW32__
#include "armmmu.h"
#include "armmem.h"
#include "armio.h"
#include "device/lcd/skyeye_lcd.h"


#include "skyeye.h"
#include "device/skyeye_device.h"
#include "device/net/skyeye_net.h"
#include "skyeye_config.h"
#else
#include "armmmu.h"
#include "armmem.h"
#include "armio.h"
#endif
//end koodailar ----------------------------------------------------------------

typedef unsigned ARMul_CPInits (ARMul_State * state);
typedef unsigned ARMul_CPExits (ARMul_State * state);
typedef unsigned ARMul_LDCs (ARMul_State * state, unsigned type,
			     ARMword instr, ARMword value);
typedef unsigned ARMul_STCs (ARMul_State * state, unsigned type,
			     ARMword instr, ARMword * value);
typedef unsigned ARMul_MRCs (ARMul_State * state, unsigned type,
			     ARMword instr, ARMword * value);
typedef unsigned ARMul_MCRs (ARMul_State * state, unsigned type,
			     ARMword instr, ARMword value);
typedef unsigned ARMul_CDPs (ARMul_State * state, unsigned type,
			     ARMword instr);
typedef unsigned ARMul_CPReads (ARMul_State * state, unsigned reg,
				ARMword * value);
typedef unsigned ARMul_CPWrites (ARMul_State * state, unsigned reg,
				 ARMword value);


//added by ksh,2004-3-5
struct ARMul_io
{
	ARMword *instr;		//to display the current interrupt state
	ARMword *net_flag;	//to judge if network is enabled
	ARMword *net_int;	//netcard interrupt

	//ywc,2004-04-01
	ARMword *ts_int;
	ARMword *ts_is_enable;
	ARMword *ts_addr_begin;
	ARMword *ts_addr_end;
	ARMword *ts_buffer;
};

/*added by ksh,2004-11-26,some energy profiling*/
struct ARMul_Energy
{
	int energy_prof;	/* <tktan>  BUG200103282109 : for energy profiling */
	int enable_func_energy;	/* <tktan> BUG200105181702 */
	char *func_energy;
	int func_display;	/* <tktan> BUG200103311509 : for function call display */
	int func_disp_start;	/* <tktan> BUG200104191428 : to start func profiling */
	char *start_func;	/* <tktan> BUG200104191428 */

	FILE *outfile;		/* <tktan> BUG200105201531 : direct console to file */
	ARMsdword tcycle, pcycle;
	float t_energy;
	void *cur_task;		/* <tktan> BUG200103291737 */
	ARMsdword t_mem_cycle, t_idle_cycle, t_uart_cycle;
	ARMsdword p_mem_cycle, p_idle_cycle, p_uart_cycle;
	ARMsdword p_io_update_tcycle;
	/*record CCCR,to get current core frequency */
	ARMword cccr;
};
typedef struct
{
	const char *cpu_arch_name;	/*cpu architecture version name.e.g. armv4t */
	const char *cpu_name;	/*cpu name. e.g. arm7tdmi or arm720t */
	ARMword cpu_val;	/*CPU value; also call MMU ID or processor id;see
				   ARM Architecture Reference Manual B2-6 */
	ARMword cpu_mask;	/*cpu_val's mask. */
	ARMword cachetype;	/*this cpu has what kind of cache */
} cpu_config_t;

#define MAX_BANK 8
#define MAX_STR  1024

typedef struct mem_bank
{
	ARMword (*read_byte) (ARMul_State * state, ARMword addr);
	void (*write_byte) (ARMul_State * state, ARMword addr, ARMword data);
	  ARMword (*read_halfword) (ARMul_State * state, ARMword addr);
	void (*write_halfword) (ARMul_State * state, ARMword addr,
				ARMword data);
	  ARMword (*read_word) (ARMul_State * state, ARMword addr);
	void (*write_word) (ARMul_State * state, ARMword addr, ARMword data);
	unsigned int addr, len;
	char filename[MAX_STR];
	unsigned type;		//chy 2003-09-21: maybe io,ram,rom
} mem_bank_t;

typedef struct
{
	int bank_num;
	int current_num;	/*current num of bank */
	mem_bank_t mem_banks[MAX_BANK];
} mem_config_t;
struct ARMul_State
{
	ARMword Emulate;	/* to start and stop emulation */
	unsigned EndCondition;	/* reason for stopping */
	unsigned ErrorCode;	/* type of illegal instruction */
	ARMword Reg[16];	/* the current register file */
	ARMword RegBank[7][16];	/* all the registers */
	ARMword Cpsr;		/* the current psr */
	ARMword Spsr[7];	/* the exception psr's */
	//chy:2003-08-19, used in arm xscale
	/* 40 bit accumulator.  We always keep this 64 bits wide,
	   and move only 40 bits out of it in an MRA insn.  */
	ARMdword Accumulator;

	ARMword NFlag, ZFlag, CFlag, VFlag, IFFlags;	/* dummy flags for speed */
	//chy:2003-08-19, used in arm v5e|xscale
	ARMword SFlag;
#ifdef MODET
	ARMword TFlag;		/* Thumb state */
#endif
	ARMword Bank;		/* the current register bank */
	ARMword Mode;		/* the current mode */
	ARMword instr, pc, temp;	/* saved register state */
	ARMword loaded, decoded;	/* saved pipeline state */
	//chy 2006-04-12 for ICE breakpoint
	ARMword loaded_addr, decoded_addr;	/* saved pipeline state addr*/
	unsigned int NumScycles, NumNcycles, NumIcycles, NumCcycles, NumFcycles;	/* emulated cycles used */
	ARMdword NumInstrs;	/* the number of instructions executed */
	unsigned NextInstr;
	unsigned VectorCatch;	/* caught exception mask */
	unsigned CallDebug;	/* set to call the debugger */
	unsigned CanWatch;	/* set by memory interface if its willing to suffer the
				   overhead of checking for watchpoints on each memory
				   access */
	unsigned MemReadDebug, MemWriteDebug;
	unsigned int StopHandle;

	unsigned char *MemInPtr;	/* the Data In bus */
	unsigned char *MemOutPtr;	/* the Data Out bus (which you may not need */
	unsigned char *MemSparePtr;	/* extra space */
	ARMword MemSize;

	unsigned char *OSptr;	/* OS Handle */
	char *CommandLine;	/* Command Line from ARMsd */

	ARMul_CPInits *CPInit[16];	/* coprocessor initialisers */
	ARMul_CPExits *CPExit[16];	/* coprocessor finalisers */
	ARMul_LDCs *LDC[16];	/* LDC instruction */
	ARMul_STCs *STC[16];	/* STC instruction */
	ARMul_MRCs *MRC[16];	/* MRC instruction */
	ARMul_MCRs *MCR[16];	/* MCR instruction */
	ARMul_CDPs *CDP[16];	/* CDP instruction */
	ARMul_CPReads *CPRead[16];	/* Read CP register */
	ARMul_CPWrites *CPWrite[16];	/* Write CP register */
	unsigned char *CPData[16];	/* Coprocessor data */
	unsigned char const *CPRegWords[16];	/* map of coprocessor register sizes */

	unsigned EventSet;	/* the number of events in the queue */
	unsigned int Now;	/* time to the nearest cycle */
	struct EventNode **EventPtr;	/* the event list */

	unsigned Debug;		/* show instructions as they are executed */
	unsigned NresetSig;	/* reset the processor */
	unsigned NfiqSig;
	unsigned NirqSig;

	unsigned abortSig;
	unsigned NtransSig;
	unsigned bigendSig;
	unsigned prog32Sig;
	unsigned data32Sig;

/* 2004-05-09 chy
----------------------------------------------------------
read ARM Architecture Reference Manual
2.6.5 Data Abort
There are three Abort Model in ARM arch.

Early Abort Model: used in some ARMv3 and earlier implementations. In this
model, base register wirteback occurred for LDC,LDM,STC,STM instructions, and
the base register was unchanged for all other instructions. (oldest)

Base Restored Abort Model: If a Data Abort occurs in an instruction which
specifies base register writeback, the value in the base register is
unchanged. (strongarm, xscale)

Base Updated Abort Model: If a Data Abort occurs in an instruction which
specifies base register writeback, the base register writeback still occurs.
(arm720T)

read PART B
chap2 The System Control Coprocessor  CP15
2.4 Register1:control register
L(bit 6): in some ARMv3 and earlier implementations, the abort model of the
processor could be configured:
0=early Abort Model Selected(now obsolete)
1=Late Abort Model selceted(same as Base Updated Abort Model)

on later processors, this bit reads as 1 and ignores writes.
-------------------------------------------------------------
So, if lateabtSig=1, then it means Late Abort Model(Base Updated Abort Model)
    if lateabtSig=0, then it means Base Restored Abort Model
*/
	unsigned lateabtSig;

	ARMword Vector;		/* synthesize aborts in cycle modes */
	ARMword Aborted;	/* sticky flag for aborts */
	ARMword Reseted;	/* sticky flag for Reset */
	ARMword Inted, LastInted;	/* sticky flags for interrupts */
	ARMword Base;		/* extra hand for base writeback */
	ARMword AbortAddr;	/* to keep track of Prefetch aborts */

	const struct Dbg_HostosInterface *hostif;

	int verbose;		/* non-zero means print various messages like the banner */

	mmu_state_t mmu;
	mem_state_t mem;
	/*remove io_state to skyeye_mach_*.c files */
	//io_state_t io;
	/* point to a interrupt pending register. now for skyeye-ne2k.c
	 * later should move somewhere. e.g machine_config_t*/


	//chy: 2003-08-11, for different arm core type
	unsigned is_v4;		/* Are we emulating a v4 architecture (or higher) ?  */
	unsigned is_v5;		/* Are we emulating a v5 architecture ?  */
	unsigned is_v5e;	/* Are we emulating a v5e architecture ?  */
	unsigned is_XScale;	/* Are we emulating an XScale architecture ?  */
	unsigned is_iWMMXt;	/* Are we emulating an iWMMXt co-processor ?  */
	unsigned is_ep9312;	/* Are we emulating a Cirrus Maverick co-processor ?  */
	//chy 2005-09-19
	unsigned is_pxa27x;	/* Are we emulating a Intel PXA27x co-processor ?  */
	//chy: seems only used in xscale's CP14
	unsigned int LastTime;	/* Value of last call to ARMul_Time() */
	ARMword CP14R0_CCD;	/* used to count 64 clock cycles with CP14 R0 bit 3 set */


//added by ksh:for handle different machs io 2004-3-5
	ARMul_io mach_io;

/*added by ksh,2004-11-26,some energy profiling*/
	ARMul_Energy energy;

//teawater add for next_dis 2004.10.27-----------------------
	int disassemble;
//AJ2D------------------------------------------

//teawater add for arm2x86 2005.02.15-------------------------------------------
	ARMword trap;
	ARMword tea_break_addr;
	ARMword tea_break_ok;
	int tea_pc;
//AJ2D--------------------------------------------------------------------------
//teawater add for arm2x86 2005.07.03-------------------------------------------
//koodailar remove it 2005.12.18 ----------
//end koodailar --------------------------

//AJ2D--------------------------------------------------------------------------
//teawater add for arm2x86 2005.07.05-------------------------------------------
	//arm_arm A2-18
	int abort_model;	//0 Base Restored Abort Model, 1 the Early Abort Model, 2 Base Updated Abort Model 
//AJ2D--------------------------------------------------------------------------
//teawater change for return if running tb dirty 2005.07.09---------------------
	void *tb_now;
//AJ2D--------------------------------------------------------------------------

//teawater add for record reg value to ./reg.txt 2005.07.10---------------------
	FILE *tea_reg_fd;
//AJ2D--------------------------------------------------------------------------

/*added by ksh in 2005-10-1*/
	cpu_config_t *cpu;
	mem_config_t *mem_bank;

//teawater add DBCT_TEST_SPEED 2005.10.04---------------------------------------
#ifdef DBCT_TEST_SPEED
	uint64_t	instr_count;
#endif	//DBCT_TEST_SPEED
//AJ2D--------------------------------------------------------------------------
};

#define ResetPin NresetSig
#define FIQPin NfiqSig
#define IRQPin NirqSig
#define AbortPin abortSig
#define TransPin NtransSig
#define BigEndPin bigendSig
#define Prog32Pin prog32Sig
#define Data32Pin data32Sig
#define LateAbortPin lateabtSig

/***************************************************************************\
*                        Types of ARM we know about                         *
\***************************************************************************/

/* The bitflags */
#define ARM_Fix26_Prop   0x01
#define ARM_Nexec_Prop   0x02
#define ARM_Debug_Prop   0x10
#define ARM_Isync_Prop   ARM_Debug_Prop
#define ARM_Lock_Prop    0x20
//chy 2003-08-11 
#define ARM_v4_Prop      0x40
#define ARM_v5_Prop      0x80
#define ARM_v5e_Prop     0x100
#define ARM_XScale_Prop  0x200
#define ARM_ep9312_Prop  0x400
#define ARM_iWMMXt_Prop  0x800
//chy 2005-09-19
#define ARM_PXA27X_Prop  0x1000

/* ARM2 family */
#define ARM2    (ARM_Fix26_Prop)
#define ARM2as  ARM2
#define ARM61   ARM2
#define ARM3    ARM2

#ifdef ARM60			/* previous definition in armopts.h */
#undef ARM60
#endif

/* ARM6 family */
#define ARM6    (ARM_Lock_Prop)
#define ARM60   ARM6
#define ARM600  ARM6
#define ARM610  ARM6
#define ARM620  ARM6


/***************************************************************************\
*                   Macros to extract instruction fields                    *
\***************************************************************************/

#define BIT(n) ( (ARMword)(instr>>(n))&1)	/* bit n of instruction */
#define BITS(m,n) ( (ARMword)(instr<<(31-(n))) >> ((31-(n))+(m)) )	/* bits m to n of instr */
#define TOPBITS(n) (instr >> (n))	/* bits 31 to n of instr */

/***************************************************************************\
*                      The hardware vector addresses                        *
\***************************************************************************/

#define ARMResetV 0L
#define ARMUndefinedInstrV 4L
#define ARMSWIV 8L
#define ARMPrefetchAbortV 12L
#define ARMDataAbortV 16L
#define ARMAddrExceptnV 20L
#define ARMIRQV 24L
#define ARMFIQV 28L
#define ARMErrorV 32L		/* This is an offset, not an address ! */

#define ARMul_ResetV ARMResetV
#define ARMul_UndefinedInstrV ARMUndefinedInstrV
#define ARMul_SWIV ARMSWIV
#define ARMul_PrefetchAbortV ARMPrefetchAbortV
#define ARMul_DataAbortV ARMDataAbortV
#define ARMul_AddrExceptnV ARMAddrExceptnV
#define ARMul_IRQV ARMIRQV
#define ARMul_FIQV ARMFIQV

/***************************************************************************\
*                          Mode and Bank Constants                          *
\***************************************************************************/

#define USER26MODE 0L
#define FIQ26MODE 1L
#define IRQ26MODE 2L
#define SVC26MODE 3L
#define USER32MODE 16L
#define FIQ32MODE 17L
#define IRQ32MODE 18L
#define SVC32MODE 19L
#define ABORT32MODE 23L
#define UNDEF32MODE 27L
//chy 2006-02-15 add system32 mode
#define SYSTEM32MODE 31L

#define ARM32BITMODE (state->Mode > 3)
#define ARM26BITMODE (state->Mode <= 3)
#define ARMMODE (state->Mode)
#define ARMul_MODEBITS 0x1fL
#define ARMul_MODE32BIT ARM32BITMODE
#define ARMul_MODE26BIT ARM26BITMODE

#define USERBANK 0
#define FIQBANK 1
#define IRQBANK 2
#define SVCBANK 3
#define ABORTBANK 4
#define UNDEFBANK 5
#define DUMMYBANK 6
#define SYSTEMBANK USERBANK
#define BANK_CAN_ACCESS_SPSR(bank)  \
  ((bank) != USERBANK && (bank) != SYSTEMBANK && (bank) != DUMMYBANK)


/***************************************************************************\
*                  Definitons of things in the emulator                     *
\***************************************************************************/

extern void ARMul_EmulateInit (void);
extern ARMul_State *ARMul_NewState (void);
extern void ARMul_Reset (ARMul_State * state);
extern ARMword ARMul_DoProg (ARMul_State * state);
extern ARMword ARMul_DoInstr (ARMul_State * state);

/***************************************************************************\
*                Definitons of things for event handling                    *
\***************************************************************************/

extern void ARMul_ScheduleEvent (ARMul_State * state, unsigned int delay,
				 unsigned (*func) (ARMul_State *));
extern void ARMul_EnvokeEvent (ARMul_State * state);
extern unsigned int ARMul_Time (ARMul_State * state);

/***************************************************************************\
*                          Useful support routines                          *
\***************************************************************************/

extern ARMword ARMul_GetReg (ARMul_State * state, unsigned mode,
			     unsigned reg);
extern void ARMul_SetReg (ARMul_State * state, unsigned mode, unsigned reg,
			  ARMword value);
extern ARMword ARMul_GetPC (ARMul_State * state);
extern ARMword ARMul_GetNextPC (ARMul_State * state);
extern void ARMul_SetPC (ARMul_State * state, ARMword value);
extern ARMword ARMul_GetR15 (ARMul_State * state);
extern void ARMul_SetR15 (ARMul_State * state, ARMword value);

extern ARMword ARMul_GetCPSR (ARMul_State * state);
extern void ARMul_SetCPSR (ARMul_State * state, ARMword value);
extern ARMword ARMul_GetSPSR (ARMul_State * state, ARMword mode);
extern void ARMul_SetSPSR (ARMul_State * state, ARMword mode, ARMword value);

/***************************************************************************\
*                  Definitons of things to handle aborts                    *
\***************************************************************************/

extern void ARMul_Abort (ARMul_State * state, ARMword address);
#define ARMul_ABORTWORD 0xefffffff	/* SWI -1 */
#define ARMul_PREFETCHABORT(address) if (state->AbortAddr == 1) \
                                        state->AbortAddr = (address & ~3L)
#define ARMul_DATAABORT(address) state->abortSig = HIGH ; \
                                 state->Aborted = ARMul_DataAbortV ;
#define ARMul_CLEARABORT state->abortSig = LOW

/***************************************************************************\
*              Definitons of things in the memory interface                 *
\***************************************************************************/

extern unsigned ARMul_MemoryInit (ARMul_State * state,
				  unsigned int initmemsize);
extern void ARMul_MemoryExit (ARMul_State * state);

extern ARMword ARMul_LoadInstrS (ARMul_State * state, ARMword address,
				 ARMword isize);
extern ARMword ARMul_LoadInstrN (ARMul_State * state, ARMword address,
				 ARMword isize);
extern ARMword ARMul_ReLoadInstr (ARMul_State * state, ARMword address,
				  ARMword isize);

extern ARMword ARMul_LoadWordS (ARMul_State * state, ARMword address);
extern ARMword ARMul_LoadWordN (ARMul_State * state, ARMword address);
extern ARMword ARMul_LoadHalfWord (ARMul_State * state, ARMword address);
extern ARMword ARMul_LoadByte (ARMul_State * state, ARMword address);

extern void ARMul_StoreWordS (ARMul_State * state, ARMword address,
			      ARMword data);
extern void ARMul_StoreWordN (ARMul_State * state, ARMword address,
			      ARMword data);
extern void ARMul_StoreHalfWord (ARMul_State * state, ARMword address,
				 ARMword data);
extern void ARMul_StoreByte (ARMul_State * state, ARMword address,
			     ARMword data);

extern ARMword ARMul_SwapWord (ARMul_State * state, ARMword address,
			       ARMword data);
extern ARMword ARMul_SwapByte (ARMul_State * state, ARMword address,
			       ARMword data);

extern void ARMul_Icycles (ARMul_State * state, unsigned number,
			   ARMword address);
extern void ARMul_Ccycles (ARMul_State * state, unsigned number,
			   ARMword address);

extern ARMword ARMul_ReadWord (ARMul_State * state, ARMword address);
extern ARMword ARMul_ReadByte (ARMul_State * state, ARMword address);
extern void ARMul_WriteWord (ARMul_State * state, ARMword address,
			     ARMword data);
extern void ARMul_WriteByte (ARMul_State * state, ARMword address,
			     ARMword data);

extern ARMword ARMul_MemAccess (ARMul_State * state, ARMword, ARMword,
				ARMword, ARMword, ARMword, ARMword, ARMword,
				ARMword, ARMword, ARMword);

/***************************************************************************\
*            Definitons of things in the co-processor interface             *
\***************************************************************************/

#define ARMul_FIRST 0
#define ARMul_TRANSFER 1
#define ARMul_BUSY 2
#define ARMul_DATA 3
#define ARMul_INTERRUPT 4
#define ARMul_DONE 0
#define ARMul_CANT 1
#define ARMul_INC 3

#define ARMul_CP13_R0_FIQ       0x1
#define ARMul_CP13_R0_IRQ       0x2
#define ARMul_CP13_R8_PMUS      0x1

#define ARMul_CP14_R0_ENABLE    0x0001
#define ARMul_CP14_R0_CLKRST    0x0004
#define ARMul_CP14_R0_CCD       0x0008
#define ARMul_CP14_R0_INTEN0    0x0010
#define ARMul_CP14_R0_INTEN1    0x0020
#define ARMul_CP14_R0_INTEN2    0x0040
#define ARMul_CP14_R0_FLAG0     0x0100
#define ARMul_CP14_R0_FLAG1     0x0200
#define ARMul_CP14_R0_FLAG2     0x0400
#define ARMul_CP14_R10_MOE_IB   0x0004
#define ARMul_CP14_R10_MOE_DB   0x0008
#define ARMul_CP14_R10_MOE_BT   0x000c
#define ARMul_CP15_R1_ENDIAN    0x0080
#define ARMul_CP15_R1_ALIGN     0x0002
#define ARMul_CP15_R5_X         0x0400
#define ARMul_CP15_R5_ST_ALIGN  0x0001
#define ARMul_CP15_R5_IMPRE     0x0406
#define ARMul_CP15_R5_MMU_EXCPT 0x0400
#define ARMul_CP15_DBCON_M      0x0100
#define ARMul_CP15_DBCON_E1     0x000c
#define ARMul_CP15_DBCON_E0     0x0003

extern unsigned ARMul_CoProInit (ARMul_State * state);
extern void ARMul_CoProExit (ARMul_State * state);
extern void ARMul_CoProAttach (ARMul_State * state, unsigned number,
			       ARMul_CPInits * init, ARMul_CPExits * exit,
			       ARMul_LDCs * ldc, ARMul_STCs * stc,
			       ARMul_MRCs * mrc, ARMul_MCRs * mcr,
			       ARMul_CDPs * cdp,
			       ARMul_CPReads * read, ARMul_CPWrites * write);
extern void ARMul_CoProDetach (ARMul_State * state, unsigned number);

/***************************************************************************\
*               Definitons of things in the host environment                *
\***************************************************************************/

extern unsigned ARMul_OSInit (ARMul_State * state);
extern void ARMul_OSExit (ARMul_State * state);
extern unsigned ARMul_OSHandleSWI (ARMul_State * state, ARMword number);
extern ARMword ARMul_OSLastErrorP (ARMul_State * state);

extern ARMword ARMul_Debug (ARMul_State * state, ARMword pc, ARMword instr);
extern unsigned ARMul_OSException (ARMul_State * state, ARMword vector,
				   ARMword pc);
extern int rdi_log;

/***************************************************************************\
*                            Host-dependent stuff                           *
\***************************************************************************/

#ifdef macintosh
pascal void SpinCursor (short increment);	/* copied from CursorCtl.h */
# define HOURGLASS           SpinCursor( 1 )
# define HOURGLASS_RATE      1023	/* 2^n - 1 */
#endif

//teawater add for arm2x86 2005.02.14-------------------------------------------
/*ywc 2005-03-31*/
/*
#include "arm2x86.h"
#include "arm2x86_dp.h"
#include "arm2x86_movl.h"
#include "arm2x86_psr.h"
#include "arm2x86_shift.h"
#include "arm2x86_mem.h"
#include "arm2x86_mul.h"
#include "arm2x86_test.h"
#include "arm2x86_other.h"
#include "list.h"
#include "tb.h"
*/
#define EQ 0
#define NE 1
#define CS 2
#define CC 3
#define MI 4
#define PL 5
#define VS 6
#define VC 7
#define HI 8
#define LS 9
#define GE 10
#define LT 11
#define GT 12
#define LE 13
#define AL 14
#define NV 15

#ifndef NFLAG
#define NFLAG	state->NFlag
#endif //NFLAG

#ifndef ZFLAG
#define ZFLAG	state->ZFlag
#endif //ZFLAG

#ifndef CFLAG
#define CFLAG	state->CFlag
#endif //CFLAG

#ifndef VFLAG
#define VFLAG	state->VFlag
#endif //VFLAG

#ifndef IFLAG
#define IFLAG	(state->IFFlags >> 1)
#endif //IFLAG

#ifndef FFLAG
#define FFLAG	(state->IFFlags & 1)
#endif //FFLAG

#ifndef IFFLAGS
#define IFFLAGS	state->IFFlags
#endif //VFLAG

#define FLAG_MASK	0xf0000000
#define NBIT_SHIFT	31
#define ZBIT_SHIFT	30
#define CBIT_SHIFT	29
#define VBIT_SHIFT	28

//teawater change for local tb branch directly jump 2005.10.18------------------
//AJ2D--------------------------------------------------------------------------
//AJ2D--------------------------------------------------------------------------

#endif /* _ARMDEFS_H_ */

extern void ARMul_UndefInstr (ARMul_State *, ARMword);
extern void ARMul_FixCPSR (ARMul_State *, ARMword, ARMword);
extern void ARMul_FixSPSR (ARMul_State *, ARMword, ARMword);
extern void ARMul_ConsolePrint (ARMul_State *, const char *, ...);
extern void ARMul_SelectProcessor (ARMul_State *, unsigned);

extern void skyeye_exit(int);

