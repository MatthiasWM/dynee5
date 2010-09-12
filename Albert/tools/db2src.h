/*
 *  db2src.h
 *  Albert
 *
 *  Created by Matthias Melcher on 08.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>


int disarm(char *dst, unsigned int addr, unsigned int cmd);
int disarm_c(char *dst, unsigned int addr, unsigned int cmd);

extern const char *db_path;
extern const char *src_path; 
extern const char *c_path; 
extern const char *cpp_path; 
extern const char *os_path; 

extern const unsigned int flags_type_mask;

extern const unsigned int flags_type_unknown;
extern const unsigned int flags_type_arm_code;
extern const unsigned int flags_type_arm_byte;
extern const unsigned int flags_type_arm_word;
extern const unsigned int flags_type_arm_text;
extern const unsigned int flags_type_patch_table;
extern const unsigned int flags_type_jump_table;
extern const unsigned int flags_type_unused;
extern const unsigned int flags_type_rex;
extern const unsigned int flags_type_ns;
extern const unsigned int flags_type_ns_obj;
extern const unsigned int flags_type_ns_ref;
extern const unsigned int flags_type_dict;
extern const unsigned int flags_type_classinfo;
extern const unsigned int flags_type_arm_wtext;
extern const unsigned int flags_type_data;

extern const unsigned int flags_is_function;
extern const unsigned int flags_is_target;

extern const char *type_lut[];

extern unsigned char ROM[0x00800000];
extern unsigned int ROM_flags[0x00200000];

void AsmPrintf(FILE *f, const char *pat, ...);

unsigned int rom_w(unsigned int addr);
double rom_real(unsigned int addr);
double rom_fixed(unsigned int addr);
unsigned int rom_flags(unsigned int addr);
void rom_flags_set(unsigned int addr, unsigned int f);
void rom_flags_clear(unsigned int addr, unsigned int f);
char rom_flags_is_set(unsigned int addr, unsigned int f);
void rom_flags_type(unsigned int addr, unsigned int t);
void rom_flags_type(unsigned int addr, unsigned int end, unsigned int t);
unsigned int rom_flags_type(unsigned int addr);
const char *get_symbol_at(unsigned int addr);
const char *get_plain_symbol_at(unsigned int addr);

const char *get_symbol_at(unsigned int addr);
const char *get_plain_symbol_at(unsigned int addr);

unsigned int decodeNSRef(FILE *newt, unsigned int i);
unsigned int decodeNSObj(FILE *newt, unsigned int i);
void extractStencils();
  
