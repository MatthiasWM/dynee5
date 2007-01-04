/*  arminit.c -- ARMulator initialization:  ARM6 Instruction Emulator.
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

//koodailar remove it for mingw 2005.12.18------------------------------------
#ifdef WIN32
#else
# include <unistd.h>
# include <termios.h>
#endif
//end ------------------------------------------------------------------------

#include "armdefs.h"
#include "armemu.h"

void skyeye_exit(int i) {
  exit(i);
}

/***************************************************************************\
*                 Definitions for the emulator architecture                 *
\***************************************************************************/

void ARMul_EmulateInit (void);
ARMul_State *ARMul_NewState (void);
void ARMul_Reset (ARMul_State * state);
ARMword ARMul_DoCycle (ARMul_State * state);
unsigned ARMul_DoCoPro (ARMul_State * state);
ARMword ARMul_DoProg (ARMul_State * state);
ARMword ARMul_DoInstr (ARMul_State * state);
void ARMul_Abort (ARMul_State * state, ARMword address);

unsigned ARMul_MultTable[32] =
	{ 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9,
	10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 16
};
ARMword ARMul_ImmedTable[4096];	/* immediate DP LHS values */
char ARMul_BitList[256];	/* number of bits in a byte table */

//chy 2006-02-22 add test debugmode
extern int debugmode;
//extern int remote_interrupt( void );


/***************************************************************************\
*         Call this routine once to set up the emulator's tables.           *
\***************************************************************************/

void
ARMul_EmulateInit (void)
{
	unsigned int i, j;

	for (i = 0; i < 4096; i++) {	/* the values of 12 bit dp rhs's */
		ARMul_ImmedTable[i] = ROTATER (i & 0xffL, (i >> 7L) & 0x1eL);
	}

	for (i = 0; i < 256; ARMul_BitList[i++] = 0);	/* how many bits in LSM */
	for (j = 1; j < 256; j <<= 1)
		for (i = 0; i < 256; i++)
			if ((i & j) > 0)
				ARMul_BitList[i]++;

	for (i = 0; i < 256; i++)
		ARMul_BitList[i] *= 4;	/* you always need 4 times these values */

}

/***************************************************************************\
*            Returns a new instantiation of the ARMulator's state           *
\***************************************************************************/

ARMul_State *
ARMul_NewState (void)
{
	ARMul_State *state;
	unsigned i, j;

	state = (ARMul_State *) malloc (sizeof (ARMul_State));
	if (state == NULL) {
		printf ("SKYEYE: ARMul_NewState malloc state error\n");
		skyeye_exit (-1);
	}
	memset (state, 0, sizeof (ARMul_State));

	state->Emulate = RUN;
	for (i = 0; i < 16; i++) {
		state->Reg[i] = 0;
		for (j = 0; j < 7; j++)
			state->RegBank[j][i] = 0;
	}
	for (i = 0; i < 7; i++)
		state->Spsr[i] = 0;
	state->Mode = 0;

	state->CallDebug = FALSE;
	state->Debug = FALSE;
	state->VectorCatch = 0;
	state->Aborted = FALSE;
	state->Reseted = FALSE;
	state->Inted = 3;
	state->LastInted = 3;

	state->MemInPtr = NULL;
	state->MemOutPtr = NULL;
	state->MemSparePtr = NULL;
	state->MemSize = 0;

	state->OSptr = NULL;
	state->CommandLine = NULL;

	state->EventSet = 0;
	state->Now = 0;
	state->EventPtr =
		(struct EventNode **) malloc ((unsigned) EVENTLISTSIZE *
					      sizeof (struct EventNode *));
	if (state->EventPtr == NULL) {
		printf ("SKYEYE: ARMul_NewState malloc state->EventPtr error\n");
		skyeye_exit (-1);
	}
	for (i = 0; i < EVENTLISTSIZE; i++)
		*(state->EventPtr + i) = NULL;

#ifdef ARM61
	state->prog32Sig = LOW;
	state->data32Sig = LOW;
#else
	state->prog32Sig = HIGH;
	state->data32Sig = HIGH;
#endif

	state->lateabtSig = LOW;
	state->bigendSig = HIGH;

	//chy:2003-08-19 
	state->LastTime = 0;
	state->CP14R0_CCD = -1;

	//ARMul_Reset (state);

	/*ywc 2005-03-31 */
	/*
	   if(!skyeye_config.no_dbct){
	   //teawater add for arm2x86 2005.02.14-------------------------------------------
	   state->tea_break_ok = 0;
	   state->tea_break_addr = 0;
	   state->tea_pc = 0;
	   if (arm2x86_init()) {
	   printf("SKYEYE: arm2x86_init error\n");
	   exit(-1);
	   }
	   //AJ2D--------------------------------------------------------------------------
	   }
	 */
	state->cpu = (cpu_config_t *) malloc (sizeof (cpu_config_t));
	state->mem_bank = (mem_config_t *) malloc (sizeof (mem_config_t));
	return (state);
}

/***************************************************************************\
*       Call this routine to set ARMulator to model a certain processor     *
\***************************************************************************/

void
ARMul_SelectProcessor (ARMul_State * state, unsigned properties)
{
	if (properties & ARM_Fix26_Prop) {
		state->prog32Sig = LOW;
		state->data32Sig = LOW;
	}
	else {
		state->prog32Sig = HIGH;
		state->data32Sig = HIGH;
	}
/* 2004-05-09 chy
below line sould be in skyeye_mach_XXX.c 's XXX_mach_init function
*/
	state->lateabtSig = LOW;


	state->is_v4 =
		(properties & (ARM_v4_Prop | ARM_v5_Prop)) ? HIGH : LOW;
	state->is_v5 = (properties & ARM_v5_Prop) ? HIGH : LOW;
	state->is_v5e = (properties & ARM_v5e_Prop) ? HIGH : LOW;
	state->is_XScale = (properties & ARM_XScale_Prop) ? HIGH : LOW;
	state->is_iWMMXt = (properties & ARM_iWMMXt_Prop) ? HIGH : LOW;
	state->is_ep9312 = (properties & ARM_ep9312_Prop) ? HIGH : LOW;
	//chy 2005-09-19
	state->is_pxa27x = (properties & ARM_PXA27X_Prop) ? HIGH : LOW;

	/* Only initialse the coprocessor support once we
	   know what kind of chip we are dealing with.  */
	ARMul_CoProInit (state);

}

/***************************************************************************\
* Call this routine to set up the initial machine state (or perform a RESET *
\***************************************************************************/

void
ARMul_Reset (ARMul_State * state)
{
	//fprintf(stderr,"armul_reset 0: state->  Cpsr 0x%x, Mode %d\n",state->Cpsr,state->Mode);  
	state->NextInstr = 0;
	if (state->prog32Sig) {
		state->Reg[15] = 0;
		state->Cpsr = INTBITS | SVC32MODE;
		state->Mode = SVC32MODE;
	}
	else {
		state->Reg[15] = R15INTBITS | SVC26MODE;
		state->Cpsr = INTBITS | SVC26MODE;
		state->Mode = SVC26MODE;
	}
	//fprintf(stderr,"armul_reset 1: state->  Cpsr 0x%x, Mode %d\n",state->Cpsr,state->Mode);  
	ARMul_CPSRAltered (state);
	state->Bank = SVCBANK;
	FLUSHPIPE;

	state->EndCondition = 0;
	state->ErrorCode = 0;

	//fprintf(stderr,"armul_reset 2: state->  Cpsr 0x%x, Mode %d\n",state->Cpsr,state->Mode);  
	state->NresetSig = HIGH;
	state->NfiqSig = HIGH;
	state->NirqSig = HIGH;
	state->NtransSig = (state->Mode & 3) ? HIGH : LOW;
	state->abortSig = LOW;
	state->AbortAddr = 1;

	state->NumInstrs = 0;
	state->NumNcycles = 0;
	state->NumScycles = 0;
	state->NumIcycles = 0;
	state->NumCcycles = 0;
	state->NumFcycles = 0;
#ifdef ASIM
	(void) ARMul_MemoryInit ();
	ARMul_OSInit (state);
#endif
	//fprintf(stderr,"armul_reset 3: state->  Cpsr 0x%x, Mode %d\n",state->Cpsr,state->Mode);  
	mmu_reset (state);
	//fprintf(stderr,"armul_reset 4: state->  Cpsr 0x%x, Mode %d\n",state->Cpsr,state->Mode);  
	mem_reset (state);
	//fprintf(stderr,"armul_reset 5: state->  Cpsr 0x%x, Mode %d\n",state->Cpsr,state->Mode);  
	/*remove later. walimis 03.7.17 */
	//io_reset(state);
	//lcd_disable(state);

	/*ywc 2005-04-07 move from ARMul_NewState , because skyeye_config.no_dbct will
	 *be configured in skyeye_option_init and it is called after ARMul_NewState*/
	state->tea_break_ok = 0;
	state->tea_break_addr = 0;
	state->tea_pc = 0;
#ifndef NO_DBCT
	if (!skyeye_config.no_dbct) {
		//teawater add for arm2x86 2005.02.14-------------------------------------------
		if (arm2x86_init (state)) {
			printf ("SKYEYE: arm2x86_init error\n");
			skyeye_exit (-1);
		}
		//AJ2D--------------------------------------------------------------------------
	}
#endif
}


/***************************************************************************\
* Emulate the execution of an entire program.  Start the correct emulator   *
* (Emulate26 for a 26 bit ARM and Emulate32 for a 32 bit ARM), return the   *
* address of the last instruction that is executed.                         *
\***************************************************************************/

//teawater add DBCT_TEST_SPEED 2005.10.04---------------------------------------
#ifdef DBCT_TEST_SPEED
static ARMul_State	*dbct_test_speed_state = NULL;
static void
dbct_test_speed_sig(int signo)
{
	printf("\n0x%llx %llu\n", dbct_test_speed_state->instr_count, dbct_test_speed_state->instr_count);
	skyeye_exit(0);
}
#endif	//DBCT_TEST_SPEED
//AJ2D--------------------------------------------------------------------------

ARMword
ARMul_DoProg (ARMul_State * state)
{
	ARMword pc = 0;
//end---------------------------------------------------------------------------------

//teawater add DBCT_TEST_SPEED 2005.10.04---------------------------------------
#ifdef DBCT_TEST_SPEED
	{
		if (!dbct_test_speed_state) {
			//init timer
			struct itimerval	value;
			struct sigaction	act;

			dbct_test_speed_state = state;
			state->instr_count = 0;
			act.sa_handler = dbct_test_speed_sig;
			act.sa_flags = SA_RESTART;
			//cygwin don't support ITIMER_VIRTUAL or ITIMER_PROF
#ifndef __CYGWIN__
			if (sigaction(SIGVTALRM, &act, NULL) == -1) {
#else
			if (sigaction(SIGALRM, &act, NULL) == -1) {
#endif	//__CYGWIN__
				fprintf(stderr, "init timer error.\n");
				skyeye_exit(-1);
			}
			if (skyeye_config.dbct_test_speed_sec) {
				value.it_value.tv_sec = skyeye_config.dbct_test_speed_sec;
			}
			else {
				value.it_value.tv_sec = DBCT_TEST_SPEED_SEC;
			}
			printf("dbct_test_speed_sec = %ld\n", value.it_value.tv_sec);
			value.it_value.tv_usec = 0;
			value.it_interval.tv_sec = 0; 
			value.it_interval.tv_usec = 0;
#ifndef __CYGWIN__
			if (setitimer(ITIMER_VIRTUAL, &value, NULL) == -1) {
#else
			if (setitimer(ITIMER_REAL, &value, NULL) == -1) {
#endif	//__CYGWIN__
				fprintf(stderr, "init timer error.\n");
				skyeye_exit(-1);
			}
		}
	}
#endif	//DBCT_TEST_SPEED
//AJ2D--------------------------------------------------------------------------
	state->Emulate = RUN;
	while (state->Emulate != STOP) {
		state->Emulate = RUN;

		/*ywc 2005-03-31 */
		if (state->prog32Sig && ARMul_MODE32BIT) {
			pc = ARMul_Emulate32 (state);
		}

	//	else
	//		pc = ARMul_Emulate26 (state);
	}
	/* Restore the original terminal settings */
	return (pc);
}

/***************************************************************************\
* Emulate the execution of one instruction.  Start the correct emulator     *
* (Emulate26 for a 26 bit ARM and Emulate32 for a 32 bit ARM), return the   *
* address of the instruction that is executed.                              *
\***************************************************************************/

ARMword
ARMul_DoInstr (ARMul_State * state)
{
	ARMword pc = 0;

	state->Emulate = ONCE;

	/*ywc 2005-03-31 */
	if (state->prog32Sig && ARMul_MODE32BIT) {
#ifndef NO_DBCT
		if (skyeye_config.no_dbct) {
			pc = ARMul_Emulate32 (state);
		}
		else {
//teawater add compile switch for DBCT GDB RSP function 2005.10.21--------------
#ifndef DBCT_GDBRSP
			printf("DBCT GDBRSP function switch is off.\n");
			printf("To use this function, open \"#define DBCT_GDBRSP\" in arch/arm/common/armdefs.h & recompile skyeye.\n");
			skyeye_exit(-1);
#endif	//DBCT_GDBRSP
//AJ2D--------------------------------------------------------------------------
			pc = ARMul_Emulate32_dbct (state);
		}
#else
		pc = ARMul_Emulate32 (state);
#endif
	}

	//else
	//	pc = ARMul_Emulate26 (state);

	return (pc);
}

/***************************************************************************\
* This routine causes an Abort to occur, including selecting the correct    *
* mode, register bank, and the saving of registers.  Call with the          *
* appropriate vector's memory address (0,4,8 ....)                          *
\***************************************************************************/

#if 0
void
ARMul_Abort (ARMul_State * state, ARMword vector)
{
	ARMword temp;

	state->Aborted = FALSE;

	if (ARMul_OSException (state, vector, ARMul_GetPC (state)))
		return;

	if (state->prog32Sig)
		if (ARMul_MODE26BIT)
			temp = R15PC;
		else
			temp = state->Reg[15];
	else
		temp = R15PC | ECC | ER15INT | EMODE;

	switch (vector) {
	case ARMul_ResetV:	/* RESET */
		state->Spsr[SVCBANK] = CPSR;
		SETABORT (INTBITS, state->prog32Sig ? SVC32MODE : SVC26MODE);
		ARMul_CPSRAltered (state);
		state->Reg[14] = temp;
		break;
	case ARMul_UndefinedInstrV:	/* Undefined Instruction */
		state->Spsr[state->prog32Sig ? UNDEFBANK : SVCBANK] = CPSR;
		SETABORT (IBIT, state->prog32Sig ? UNDEF32MODE : SVC26MODE);
		ARMul_CPSRAltered (state);
		state->Reg[14] = temp - 4;
		break;
	case ARMul_SWIV:	/* Software Interrupt */
		state->Spsr[SVCBANK] = CPSR;
		SETABORT (IBIT, state->prog32Sig ? SVC32MODE : SVC26MODE);
		ARMul_CPSRAltered (state);
		state->Reg[14] = temp - 4;
		break;
	case ARMul_PrefetchAbortV:	/* Prefetch Abort */
		state->AbortAddr = 1;
		state->Spsr[state->prog32Sig ? ABORTBANK : SVCBANK] = CPSR;
		SETABORT (IBIT, state->prog32Sig ? ABORT32MODE : SVC26MODE);
		ARMul_CPSRAltered (state);
		state->Reg[14] = temp - 4;
		break;
	case ARMul_DataAbortV:	/* Data Abort */
		state->Spsr[state->prog32Sig ? ABORTBANK : SVCBANK] = CPSR;
		SETABORT (IBIT, state->prog32Sig ? ABORT32MODE : SVC26MODE);
		ARMul_CPSRAltered (state);
		state->Reg[14] = temp - 4;	/* the PC must have been incremented */
		break;
	case ARMul_AddrExceptnV:	/* Address Exception */
		state->Spsr[SVCBANK] = CPSR;
		SETABORT (IBIT, SVC26MODE);
		ARMul_CPSRAltered (state);
		state->Reg[14] = temp - 4;
		break;
	case ARMul_IRQV:	/* IRQ */
		state->Spsr[IRQBANK] = CPSR;
		SETABORT (IBIT, state->prog32Sig ? IRQ32MODE : IRQ26MODE);
		ARMul_CPSRAltered (state);
		state->Reg[14] = temp - 4;
		break;
	case ARMul_FIQV:	/* FIQ */
		state->Spsr[FIQBANK] = CPSR;
		SETABORT (INTBITS, state->prog32Sig ? FIQ32MODE : FIQ26MODE);
		ARMul_CPSRAltered (state);
		state->Reg[14] = temp - 4;
		break;
	}
	if (ARMul_MODE32BIT) {
		if (state->mmu.control & CONTROL_VECTOR)
			vector += 0xffff0000;	//for v4 high exception  address
		ARMul_SetR15 (state, vector);
	}
	else
		ARMul_SetR15 (state, R15CCINTMODE | vector);
}
#endif

void
ARMul_Abort (ARMul_State * state, ARMword vector)
{
	ARMword temp;
	int isize = INSN_SIZE;
	int esize = (TFLAG ? 0 : 4);
	int e2size = (TFLAG ? -4 : 0);

	state->Aborted = FALSE;

	if (ARMul_OSException (state, vector, ARMul_GetPC (state)))
		return;

	if (state->prog32Sig)
		if (ARMul_MODE26BIT)
			temp = R15PC;
		else
			temp = state->Reg[15];
	else
		temp = R15PC | ECC | ER15INT | EMODE;

	switch (vector) {
	case ARMul_ResetV:	/* RESET */
		SETABORT (INTBITS, state->prog32Sig ? SVC32MODE : SVC26MODE,
			  0);
		break;
	case ARMul_UndefinedInstrV:	/* Undefined Instruction */
		SETABORT (IBIT, state->prog32Sig ? UNDEF32MODE : SVC26MODE,
			  isize);
		break;
	case ARMul_SWIV:	/* Software Interrupt */
		SETABORT (IBIT, state->prog32Sig ? SVC32MODE : SVC26MODE,
			  isize);
		break;
	case ARMul_PrefetchAbortV:	/* Prefetch Abort */
		state->AbortAddr = 1;
		SETABORT (IBIT, state->prog32Sig ? ABORT32MODE : SVC26MODE,
			  esize);
		break;
	case ARMul_DataAbortV:	/* Data Abort */
		SETABORT (IBIT, state->prog32Sig ? ABORT32MODE : SVC26MODE,
			  e2size);
		break;
	case ARMul_AddrExceptnV:	/* Address Exception */
		SETABORT (IBIT, SVC26MODE, isize);
		break;
	case ARMul_IRQV:	/* IRQ */
		//chy 2003-09-02 the if sentence seems no use
#if 0
		if (!state->is_XScale || !state->CPRead[13] (state, 0, &temp)
		    || (temp & ARMul_CP13_R0_IRQ))
#endif
			SETABORT (IBIT,
				  state->prog32Sig ? IRQ32MODE : IRQ26MODE,
				  esize);
		break;
	case ARMul_FIQV:	/* FIQ */
		//chy 2003-09-02 the if sentence seems no use
#if 0
		if (!state->is_XScale || !state->CPRead[13] (state, 0, &temp)
		    || (temp & ARMul_CP13_R0_FIQ))
#endif
			SETABORT (INTBITS,
				  state->prog32Sig ? FIQ32MODE : FIQ26MODE,
				  esize);
		break;
	}
	if (ARMul_MODE32BIT) {
		if (state->mmu.control & CONTROL_VECTOR)
			vector += 0xffff0000;	//for v4 high exception  address
		ARMul_SetR15 (state, vector);
	}
	else
		ARMul_SetR15 (state, R15CCINTMODE | vector);
#if 0
	if (ARMul_ReadWord (state, ARMul_GetPC (state)) == 0) {
		/* No vector has been installed.  Rather than simulating whatever
		   random bits might happen to be at address 0x20 onwards we elect
		   to stop.  */
		switch (vector) {
		case ARMul_ResetV:
			state->EndCondition = RDIError_Reset;
			break;
		case ARMul_UndefinedInstrV:
			state->EndCondition = RDIError_UndefinedInstruction;
			break;
		case ARMul_SWIV:
			state->EndCondition = RDIError_SoftwareInterrupt;
			break;
		case ARMul_PrefetchAbortV:
			state->EndCondition = RDIError_PrefetchAbort;
			break;
		case ARMul_DataAbortV:
			state->EndCondition = RDIError_DataAbort;
			break;
		case ARMul_AddrExceptnV:
			state->EndCondition = RDIError_AddressException;
			break;
		case ARMul_IRQV:
			state->EndCondition = RDIError_IRQ;
			break;
		case ARMul_FIQV:
			state->EndCondition = RDIError_FIQ;
			break;
		default:
			break;
		}
		state->Emulate = FALSE;
	}
#endif
}
