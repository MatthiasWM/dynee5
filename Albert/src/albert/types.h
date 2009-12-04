/*
 *  types.h
 *  Albert
 *
 *  Created by Matthias Melcher on 03.12.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALBERT_TYPES_H
#define ALBERT_TYPES_H

typedef unsigned int t_unknown;
typedef unsigned int t_any;

extern t_any r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12;
extern t_any lr, sp, pc;

const unsigned short R0 = 0;
const unsigned short R1 = 1;
const unsigned short R2 = 2;
const unsigned short R3 = 4;
const unsigned short R4 = 8;
const unsigned short R5 = 16;
const unsigned short R6 = 32;
const unsigned short R7 = 64;
const unsigned short R8 = 128;
const unsigned short R9 = 256;
const unsigned short R10 = 512;
const unsigned short R11 = 1024;
const unsigned short R12 = 2048;
const unsigned short LR = 8096;
const unsigned short SP = 16384;
const unsigned short PC = 32768;

extern char arm_is_eq();
extern char arm_is_ne();
extern char arm_is_cs();
extern char arm_is_cc();
extern char arm_is_mi();
extern char arm_is_pl();
extern char arm_is_vs();
extern char arm_is_vc();
extern char arm_is_hi();
extern char arm_is_ls();
extern char arm_is_ge();
extern char arm_is_lt();
extern char arm_is_gt();
extern char arm_is_le();

extern void arm_mov(t_any dst, t_any src);
extern void arm_movs(t_any dst, t_any src);
extern void arm_mvn(t_any dst, t_any src);
extern void arm_neg(t_any dst, t_any src);
extern void arm_ldr(t_any dst, t_any src_addr);
extern void arm_ldrb(t_any dst, t_any src_addr);
extern void arm_str(t_any src, t_any dst_addr);
extern void arm_strb(t_any src, t_any dst_addr);
extern void arm_add(t_any dst, t_any a, t_any b);
extern void arm_and(t_any dst, t_any a, t_any b);
extern void arm_sub(t_any dst, t_any a, t_any b);
extern void arm_ldmdb(t_any src_addr, unsigned short reg_map);
extern void arm_stmdb(t_any dst_addr, unsigned short reg_map);
extern void arm_stmia(t_any dst_addr, unsigned short reg_map);
extern void arm_teq(t_any a, t_any b);
extern void arm_cmp(t_any a, t_any b);
extern void arm_cmn(t_any a, t_any b);

#endif

