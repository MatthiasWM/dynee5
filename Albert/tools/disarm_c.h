/* Opcode table for the ARM.
 
 Copyright 1994, 1995, 1996, 1997 Free Software Foundation, Inc.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#ifndef DISARM_C_H
#define DISARM_C_H

struct arm_opcode_c {
  unsigned long value, mask;	/* recognise instruction if (op&mask)==value */
  const char *assembler;		/* how to disassemble this instruction */
};

/* format of the assembler string :
 
 %%			%
 %<bitfield>d		print the bitfield in decimal
 %<bitfield>x		print the bitfield in hex
 %<bitfield>r		print as an ARM register
 %<bitfield>f		print a floating point constant if >7 else a
 floating point register
 %c			print condition code (always bits 28-31)
 %P			print floating point precision in arithmetic insn
 %Q			print floating point precision in ldf/stf insn
 %R			print floating point rounding mode
 %<bitnum>$text$	print specified text iff bit is one
 %<bitnum>'c		print specified char iff bit is one
 %<bitnum>`c		print specified char iff bit is zero
 %<bitnum>?ab		print a if bit is one else print b
 %p			print 'p' iff bits 12-15 are 15
 %t			print 't' iff bit 21 set and bit 24 clear
 %h                   print 'h' iff bit 5 set, else print 'b'
 %o			print operand2 (immediate or register + shift)
 %a			print address for ldr/str instruction
 %s                   print address for ldr/str halfword/signextend instruction
 %b			print branch destination
 %B			print branch with link destination
 %A			print address for ldc/stc/ldf/stf instruction
 %m			print register mask for ldm/stm instruction
 %C			print the PSR sub type.
 %F			print the COUNT field of a LFM/SFM instruction.
 Thumb specific format options:
 %D                   print Thumb register (bits 0..2 as high number if bit 7 set)
 %S                   print Thumb register (bits 3..5 as high number if bit 6 set)
 %<bitfield>I         print bitfield as a signed decimal
 (top bit of range being the sign bit)
 %M                   print Thumb register mask
 %N                   print Thumb register mask (with LR)
 %O                   print Thumb register mask (with PC)
 %T                   print Thumb condition code (always bits 8-11)
 %<bitfield>B         print Thumb branch destination (signed displacement)
 %<bitfield>W         print (bitfield * 4) as a decimal
 %<bitfield>H         print (bitfield * 2) as a decimal
 %<bitfield>a         print (bitfield * 4) as a pc-rel offset + decoded symbol
 
 
 00      GetPortSWI
 01      PortSendSWI
 02      PortReceiveSWI
 03      EnterAtomicSWI
 04      ExitAtomicSWI
 05      GenericSWI
 06      GenerateMessageIRQ
 07      PurgeMMUTBLEntry
 08      FlushMMU
 09      FlushCache
 0a      GetCPUVersion
 0b      SemaphoreOpGlue
 0c      SetDomainRegister
 0d      SMemSetBufferSWI
 0e      SMemGetSizeSWI
 0f      SMemCopyToSharedSWI
 10      SMemCopyFromSharedSWI
 11      SMemMsgSetTimerParmsSWI
 12      SMemMsgSetMsgAvailPortSWI
 13      SMemMsgGetSenderTaskIdSWI
 14      SMemMsgSetUserRefConSWI
 15      SMemMsgGetUserRefConSWI
 16      SMemMsgCheckForDoneSWI
 17      SMemMsgMsgDoneSWI
 18      TurnOffCache
 19      TurnOnCache
 1b      MonitorDispatchSWI
 1c      MonitorExitSWI
 1d      MonitorThrowSWI
 1e      EnterFIQAtomicSWI
 1f      ExitFIQAtomicSWI
 20      MonitorFlushSWI
 21      PortResetFilterSWI
 22      DoSchedulerSWI
 
 */

/* Note: There is a partial ordering in this table - it must be searched from
 the top to obtain a correct match. */

static struct arm_opcode_c arm_opcodes_c[] = {
  /* ARM instructions */
  {0x01a00000, 0x0fffffff, "a_nop();"},
  {0x01A0F00E, 0x0fffffff, "return r0;"},
  {0x06000010, 0x0fffffff, "a_throwSystemBoot();"},
  {0x06000110, 0x0fffffff, "a_throwExitToShell();"},
  {0x06000210, 0x0fffffff, "a_throwDebugger();"},
  {0x06000310, 0x0fffffff, "a_throwDebugStr();"},
  {0x06000410, 0x0fffffff, "a_throwPublicFiller();"},
  {0x06000510, 0x0fffffff, "a_throwSystemPanic();"},
  {0x06000710, 0x0fffffff, "a_throwSendTestResults();"},
  {0x06000810, 0x0fffffff, "a_throwTapFileCntl();"},
  {0x012FFF10, 0x0ffffff0, "a_bx(%0-3r);"}, // not used
  {0x00000090, 0x0fe000f0, "a_mul%20's(%16-19r, %0-3r, %8-11r);"},
  {0x00200090, 0x0fe000f0, "a_mla%20's(%16-19r, %0-3r, %8-11r, %12-15r);"},
  {0x01000090, 0x0fb00ff0, "a_swp%22'b(%12-15r, %0-3r, %16-19r);"},
  {0x00800090, 0x0fa000f0, "a_%22?sumull%20's(%12-15r, %16-19r, %0-3r, %8-11r);"},
  {0x00a00090, 0x0fa000f0, "a_%22?sumlal%20's(%12-15r, %16-19r, %0-3r, %8-11r);"},
  {0x00000090, 0x0e100090, "a_str%6's%h(%12-15r, %s);"},
  {0x00100090, 0x0e100090, "a_ldr%6's%h(%12-15r, %s);"},
  {0x00000000, 0x0de00000, "a_and%20's(%12-15r, %16-19r, %o);"},
  {0x00200000, 0x0de00000, "a_eor%20's(%12-15r, %16-19r, %o);"},
  {0x00400000, 0x0de00000, "a_sub%20's(%12-15r, %16-19r, %o);"},
  {0x00600000, 0x0de00000, "a_rsb%20's(%12-15r, %16-19r, %o);"},
  {0x00800000, 0x0de00000, "a_add%20's(%12-15r, %16-19r, %o);"},
  {0x00a00000, 0x0de00000, "a_adc%20's(%12-15r, %16-19r, %o);"},
  {0x00c00000, 0x0de00000, "a_sbc%20's(%12-15r, %16-19r, %o);"},
  {0x00e00000, 0x0de00000, "a_rsc%20's(%12-15r, %16-19r, %o);"},
  {0x0120f000, 0x0db6f000, "a_msr(%22?scpsr%C, %o);"},
  {0x010f0000, 0x0fbf0fff, "a_mrs(%12-15r, %22?scpsr);"},
  {0x01000000, 0x0de00000, "a_tst%p(%16-19r, %o);"},
  {0x01200000, 0x0de00000, "a_teq%p(%16-19r, %o);"},
  {0x01400000, 0x0de00000, "a_cmp%p(%16-19r, %o);"},
  {0x01600000, 0x0de00000, "a_cmn%p(%16-19r, %o);"},
  {0x01800000, 0x0de00000, "a_orr%20's(%12-15r, %16-19r, %o);"},
  {0x01a00000, 0x0de00000, "a_mov%20's(%12-15r, %o);"},
  {0x01c00000, 0x0de00000, "a_bic%20's(%12-15r, %16-19r, %o);"},
  {0x01e00000, 0x0de00000, "a_mvn%20's(%12-15r, %o);"},
  {0x04000000, 0x0e100000, "a_str%22'b%t(%12-15r, %a);"},
  {0x06000000, 0x0e100ff0, "a_str%22'b%t(%12-15r, %a);"},
  {0x04000000, 0x0c100010, "a_str%22'b%t(%12-15r, %a);"},
  {0x06000010, 0x0e000010, "a_undefined();"},
  {0x04100000, 0x0c100000, "a_ldr%22'b%t(%12-15r, %a);"},
  {0x08000000, 0x0e100000, "a_stm%23?id%24?ba%21$_wb$%22$_ub$(%16-19r, %m);"},
  {0x08100000, 0x0e100000, "a_ldm%23?id%24?ba%21$_wb$%22$_ub$(%16-19r, %m);"},
  {0x0a000000, 0x0f000000, "%b;"}, // branch
  {0x0b000000, 0x0f000000, "%B;"}, // branch with link
  {0x0f000000, 0x0f000000, "a_swi(%0-23x);"},
  
  /* Floating point coprocessor instructions */
  {0x0e000100, 0x0ff08f10, "a_adf%P%R(%12-14f, %16-18f, %0-3f);"},
  {0x0e100100, 0x0ff08f10, "a_muf%P%R(%12-14f, %16-18f, %0-3f);"},
  {0x0e200100, 0x0ff08f10, "a_suf%P%R(%12-14f, %16-18f, %0-3f);"},
  {0x0e300100, 0x0ff08f10, "a_rsf%P%R(%12-14f, %16-18f, %0-3f);"},
  {0x0e400100, 0x0ff08f10, "a_dvf%P%R(%12-14f, %16-18f, %0-3f);"},
  {0x0e500100, 0x0ff08f10, "a_rdf%P%R(%12-14f, %16-18f, %0-3f);"},
  {0x0e600100, 0x0ff08f10, "a_pow%P%R(%12-14f, %16-18f, %0-3f);"},
  {0x0e700100, 0x0ff08f10, "a_rpw%P%R(%12-14f, %16-18f, %0-3f);"},
  {0x0e800100, 0x0ff08f10, "a_rmf%P%R(%12-14f, %16-18f, %0-3f);"},
  {0x0e900100, 0x0ff08f10, "a_fml%P%R(%12-14f, %16-18f, %0-3f);"},
  {0x0ea00100, 0x0ff08f10, "a_fdv%P%R(%12-14f, %16-18f, %0-3f);"},
  {0x0eb00100, 0x0ff08f10, "a_frd%P%R(%12-14f, %16-18f, %0-3f);"},
  {0x0ec00100, 0x0ff08f10, "a_pol%P%R(%12-14f, %16-18f, %0-3f);"},
  {0x0e008100, 0x0ff08f10, "a_mvf%P%R(%12-14f, %0-3f);"},
  {0x0e108100, 0x0ff08f10, "a_mnf%P%R(%12-14f, %0-3f);"},
  {0x0e208100, 0x0ff08f10, "a_abs%P%R(%12-14f, %0-3f);"},
  {0x0e308100, 0x0ff08f10, "a_rnd%P%R(%12-14f, %0-3f);"},
  {0x0e408100, 0x0ff08f10, "a_sqt%P%R(%12-14f, %0-3f);"},
  {0x0e508100, 0x0ff08f10, "a_log%P%R(%12-14f, %0-3f);"},
  {0x0e608100, 0x0ff08f10, "a_lgn%P%R(%12-14f, %0-3f);"},
  {0x0e708100, 0x0ff08f10, "a_exp%P%R(%12-14f, %0-3f);"},
  {0x0e808100, 0x0ff08f10, "a_sin%P%R(%12-14f, %0-3f);"},
  {0x0e908100, 0x0ff08f10, "a_cos%P%R(%12-14f, %0-3f);"},
  {0x0ea08100, 0x0ff08f10, "a_tan%P%R(%12-14f, %0-3f);"},
  {0x0eb08100, 0x0ff08f10, "a_asn%P%R(%12-14f, %0-3f);"},
  {0x0ec08100, 0x0ff08f10, "a_acs%P%R(%12-14f, %0-3f);"},
  {0x0ed08100, 0x0ff08f10, "a_atn%P%R(%12-14f, %0-3f);"},
  {0x0ee08100, 0x0ff08f10, "a_urd%P%R(%12-14f, %0-3f);"},
  {0x0ef08100, 0x0ff08f10, "a_nrm%P%R(%12-14f, %0-3f);"},
  {0x0e000110, 0x0ff00f1f, "a_flt%P%R(%16-18f, %12-15r);"},
  {0x0e100110, 0x0fff0f98, "a_fix%R(%12-15r, %0-2f);"},
  {0x0e200110, 0x0fff0fff, "a_wfs(%12-15r);"},
  {0x0e300110, 0x0fff0fff, "a_rfs(%12-15r);"},
  {0x0e400110, 0x0fff0fff, "a_wfc(%12-15r);"},
  {0x0e500110, 0x0fff0fff, "a_rfc(%12-15r);"},
  {0x0e90f110, 0x0ff8fff0, "a_cmf(%16-18f, %0-3f);"},
  {0x0eb0f110, 0x0ff8fff0, "a_cnf(%16-18f, %0-3f);"},
  {0x0ed0f110, 0x0ff8fff0, "a_cmfe(%16-18f, %0-3f);"},
  {0x0ef0f110, 0x0ff8fff0, "a_cnfe(%16-18f, %0-3f);"},
  {0x0c000100, 0x0e100f00, "a_stf%Q(%12-14f, %A);"},
  {0x0c100100, 0x0e100f00, "a_ldf%Q(%12-14f, %A);"},
  {0x0c000200, 0x0e100f00, "a_sfm(%12-14f, %F, %A);"},
  {0x0c100200, 0x0e100f00, "a_lfm(%12-14f, %F, %A);"},
  
  /* Generic coprocessor instructions */
  {0x0e000000, 0x0f000010, "a_cdp(%8-11d, %20-23d, %12-15d, %16-19d, %0-3d, {%5-7d});"},
  {0x0e100010, 0x0f100010, "a_mrc(%8-11d, %21-23d, %12-15r, %16-19d, %0-3d, %5-7d);"},
  {0x0e000010, 0x0f100010, "a_mcr(%8-11d, %21-23d, %12-15r, %16-19d, %0-3d, %5-7d);"},
  {0x0c000000, 0x0e100000, "a_stc%22'l(%8-11d, %12-15d, %A);"},
  {0x0c100000, 0x0e100000, "a_ldc%22'l(%8-11d, %12-15d, %A);"},
  
  /* the rest */
  {0x00000000, 0x00000000, "undefined instruction %0-31x"},
  {0x00000000, 0x00000000, 0}
};

#define BDISP(x) ((((x) & 0xffffff) ^ 0x800000) - 0x800000) /* 26 bit */

#endif
