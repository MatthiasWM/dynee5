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
    unsigned int value, mask;	/* recognise instruction if (op&mask)==value */
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
   %A			print address for ldc/stc/ldf/stf instruction
   %m			print register mask for ldm/stm instruction
   %C			print the PSR sub type.
   %F			print the COUNT field of a LFM/SFM instruction.
   %n                   print DyneE5 native instruction
   %w                   printf software interrupt name
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
*/

/* Note: There is a partial ordering in this table - it must be searched from
   the top to obtain a correct match. */

static struct arm_opcode_c arm_opcodes_c[] = {
    /* ARM instructions */
    {0xe1a00000, 0xffffffff, "%carm_nop();"},
    {0x012FFF10, 0x0ffffff0, "%carm_bx(%0-3r);"},
    {0x00000090, 0x0fe000f0, "%carm_mul%20's(%16-19r, %0-3r, %8-11r);"},
    {0x00200090, 0x0fe000f0, "%carm_mla%20's(%16-19r, %0-3r, %8-11r, %12-15r);"},
    {0x01000090, 0x0fb00ff0, "%carm_swp%22'b(%12-15r, %0-3r, [%16-19r]);"},
    {0x00800090, 0x0fa000f0, "%carm_%22?=sumull%20's(%12-15r, %16-19r, %0-3r, %8-11r);"},
    {0x00a00090, 0x0fa000f0, "%carm_%22?=sumlal%20's(%12-15r, %16-19r, %0-3r, %8-11r);"},
    {0x00000090, 0x0e100090, "%carm_str%6's%h(%12-15r, %s);"},
    {0x00100090, 0x0e100090, "%carm_ldr%6's%h(%12-15r, %s);"},
    {0x00000000, 0x0de00000, "%carm_and%20's(%12-15r, %16-19r, %o);"},
    {0x00200000, 0x0de00000, "%carm_eor%20's(%12-15r, %16-19r, %o);"},
    {0x00400000, 0x0de00000, "%carm_sub%20's(%12-15r, %16-19r, %o);"},
    {0x00600000, 0x0de00000, "%carm_rsb%20's(%12-15r, %16-19r, %o);"},
    {0x00800000, 0x0de00000, "%carm_add%20's(%12-15r, %16-19r, %o);"},
    {0x00a00000, 0x0de00000, "%carm_adc%20's(%12-15r, %16-19r, %o);"},
    {0x00c00000, 0x0de00000, "%carm_sbc%20's(%12-15r, %16-19r, %o);"},
    {0x00e00000, 0x0de00000, "%carm_rsc%20's(%12-15r, %16-19r, %o);"},
    {0x0120f000, 0x0db6f000, "%carm_msr(%22?scpsr%C, %o);"},
    {0x010f0000, 0x0fbf0fff, "%carm_mrs(%12-15r, %22?scpsr);"},
    {0x01000000, 0x0de00000, "%carm_tst%p(%16-19r, %o);"},
    {0x01200000, 0x0de00000, "%carm_teq%p(%16-19r, %o);"},
    {0x01400000, 0x0de00000, "%carm_cmp%p(%16-19r, %o);"},
    {0x01600000, 0x0de00000, "%carm_cmn%p(%16-19r, %o);"},
    {0x01800000, 0x0de00000, "%carm_orr%20's(%12-15r, %16-19r, %o);"},
    {0x01a00000, 0x0de00000, "%carm_mov%20's(%12-15r, %o);"},
    {0x01c00000, 0x0de00000, "%carm_bic%20's(%12-15r, %16-19r, %o);"},
    {0x01e00000, 0x0de00000, "%carm_mvn%20's(%12-15r, %o);"},
    {0x04000000, 0x0e100000, "%carm_str%22'b%t(%12-15r, %a);"},
    {0x06000000, 0x0e100ff0, "%carm_str%22'b%t(%12-15r, %a);"},
    {0x04000000, 0x0c100010, "%carm_str%22'b%t(%12-15r, %a);"},
    {0x06000010, 0x0e000010, "%carm_undefined());"},
    {0x04100000, 0x0c100000, "%carm_ldr%22'b%t(%12-15r, %a);"},
    {0x08000000, 0x0e100000, "%carm_stm%23?id%24?ba(%16-19r%21'!, %m%22'^);"},
    {0x08100000, 0x0e100000, "%carm_ldm%23?id%24?ba(%16-19r%21'!, %m%22'^);"},
    {0x0a000000, 0x0e000000, "%c%b"},
//  {0x0a000000, 0x0e000000, "%carm_b%24'l(%b);"},
    {0x0f000000, 0x0f000000, "%carm_swi(%w);"},

    /* Floating point coprocessor instructions */
    {0x0e000100, 0x0ff08f10, "%carm_adf%P%R(%12-14f, %16-18f, %0-3f);"},
    {0x0e100100, 0x0ff08f10, "%carm_muf%P%R(%12-14f, %16-18f, %0-3f);"},
    {0x0e200100, 0x0ff08f10, "%carm_suf%P%R(%12-14f, %16-18f, %0-3f);"},
    {0x0e300100, 0x0ff08f10, "%carm_rsf%P%R(%12-14f, %16-18f, %0-3f);"},
    {0x0e400100, 0x0ff08f10, "%carm_dvf%P%R(%12-14f, %16-18f, %0-3f);"},
    {0x0e500100, 0x0ff08f10, "%carm_rdf%P%R(%12-14f, %16-18f, %0-3f);"},
    {0x0e600100, 0x0ff08f10, "%carm_pow%P%R(%12-14f, %16-18f, %0-3f);"},
    {0x0e700100, 0x0ff08f10, "%carm_rpw%P%R(%12-14f, %16-18f, %0-3f);"},
    {0x0e800100, 0x0ff08f10, "%carm_rmf%P%R(%12-14f, %16-18f, %0-3f);"},
    {0x0e900100, 0x0ff08f10, "%carm_fml%P%R(%12-14f, %16-18f, %0-3f);"},
    {0x0ea00100, 0x0ff08f10, "%carm_fdv%P%R(%12-14f, %16-18f, %0-3f);"},
    {0x0eb00100, 0x0ff08f10, "%carm_frd%P%R(%12-14f, %16-18f, %0-3f);"},
    {0x0ec00100, 0x0ff08f10, "%carm_pol%P%R(%12-14f, %16-18f, %0-3f);"},
    {0x0e008100, 0x0ff08f10, "%carm_mvf%P%R(%12-14f, %0-3f);"},
    {0x0e108100, 0x0ff08f10, "%carm_mnf%P%R(%12-14f, %0-3f);"},
    {0x0e208100, 0x0ff08f10, "%carm_abs%P%R(%12-14f, %0-3f);"},
    {0x0e308100, 0x0ff08f10, "%carm_rnd%P%R(%12-14f, %0-3f);"},
    {0x0e408100, 0x0ff08f10, "%carm_sqt%P%R(%12-14f, %0-3f);"},
    {0x0e508100, 0x0ff08f10, "%carm_log%P%R(%12-14f, %0-3f);"},
    {0x0e608100, 0x0ff08f10, "%carm_lgn%P%R(%12-14f, %0-3f);"},
    {0x0e708100, 0x0ff08f10, "%carm_exp%P%R(%12-14f, %0-3f);"},
    {0x0e808100, 0x0ff08f10, "%carm_sin%P%R(%12-14f, %0-3f);"},
    {0x0e908100, 0x0ff08f10, "%carm_cos%P%R(%12-14f, %0-3f);"},
    {0x0ea08100, 0x0ff08f10, "%carm_tan%P%R(%12-14f, %0-3f);"},
    {0x0eb08100, 0x0ff08f10, "%carm_asn%P%R(%12-14f, %0-3f);"},
    {0x0ec08100, 0x0ff08f10, "%carm_acs%P%R(%12-14f, %0-3f);"},
    {0x0ed08100, 0x0ff08f10, "%carm_atn%P%R(%12-14f, %0-3f);"},
    {0x0ee08100, 0x0ff08f10, "%carm_urd%P%R(%12-14f, %0-3f);"},
    {0x0ef08100, 0x0ff08f10, "%carm_nrm%P%R(%12-14f, %0-3f);"},
    {0x0e000110, 0x0ff00f1f, "%carm_flt%P%R(%16-18f, %12-15r);"},
    {0x0e100110, 0x0fff0f98, "%carm_fix%R(%12-15r, %0-2f);"},
    {0x0e200110, 0x0fff0fff, "%carm_wfs(%12-15r);"},
    {0x0e300110, 0x0fff0fff, "%carm_rfs(%12-15r);"},
    {0x0e400110, 0x0fff0fff, "%carm_wfc(%12-15r);"},
    {0x0e500110, 0x0fff0fff, "%carm_rfc(%12-15r);"},
    {0x0e90f110, 0x0ff8fff0, "%carm_cmf(%16-18f, %0-3f);"},
    {0x0eb0f110, 0x0ff8fff0, "%carm_cnf(%16-18f, %0-3f);"},
    {0x0ed0f110, 0x0ff8fff0, "%carm_cmfe(%16-18f, %0-3f);"},
    {0x0ef0f110, 0x0ff8fff0, "%carm_cnfe(%16-18f, %0-3f);"},
    {0x0c000100, 0x0e100f00, "%carm_stf%Q(%12-14f, %A);"},
    {0x0c100100, 0x0e100f00, "%carm_ldf%Q(%12-14f, %A);"},
    {0x0c000200, 0x0e100f00, "%carm_sfm(%12-14f, %F, %A);"},
    {0x0c100200, 0x0e100f00, "%carm_lfm(%12-14f, %F, %A);"},

    /* Generic coprocessor instructions */
    {0x0e000000, 0x0f000010, "%carm_cdp(%8-11d, %20-23d, cr%12-15d, cr%16-19d, cr%0-3d, {%5-7d});"},
    {0x0e100010, 0x0f100010, "%carm_mrc(%8-11d, %21-23d, %12-15r, cr%16-19d, cr%0-3d, {%5-7d});"},
    {0x0e000210, 0x0f100f10, "%carm_DyneNativeCall %n);"},
    {0x0e000010, 0x0f100010, "%carm_mcr(%8-11d, %21-23d, %12-15r, cr%16-19d, cr%0-3d, {%5-7d});"},
    {0x0c000000, 0x0e100000, "%carm_stc%22'l(%8-11d, cr%12-15d, %A);"},
    {0x0c100000, 0x0e100000, "%carm_ldc%22'l(%8-11d, cr%12-15d, %A);"},
    /* the rest */
    {0x0fffffff, 0x06000010, "%creturn SystemBoot();"},
    {0x0fffffff, 0x06000110, "%creturn ExitToShell();"},
    {0x0fffffff, 0x06000210, "%creturn Debugger();"},
    {0x0fffffff, 0x06000310, "%creturn DebugStr();"},
    {0x0fffffff, 0x06000410, "%creturn PublicFiller();"},
    {0x0fffffff, 0x06000510, "%cSystemPanic(text);"},
    {0x0fffffff, 0x06000710, "%creturn SendTestResults();"},
    {0x0fffffff, 0x06000810, "%creturn TapFileCntl();"},
    {0x00000000, 0x00000000, "%carm_undefined_instruction %0-31x);"},
    {0x00000000, 0x00000000, 0}
};

#define BDISP(x) ((((x) & 0xffffff) ^ 0x800000) - 0x800000) /* 26 bit */

#endif

