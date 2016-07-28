/*
 *  db2src.h
 *  Albert
 *
 *  Created by Matthias Melcher on 08.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef DB2SRC_H
#define DB2SRC_H

#include <stdio.h>

#include "Albert.h"

#include "CallGraph.h"


typedef uint32_t PAddr;
typedef uint32_t VAddr;
typedef uint32_t Instr;

const uint32_t kInvalidAddress = 0xFFFFFFFF;
const uint32_t kUnknownAddress = 0xFFFFFFFE;


extern bool gCodeCoverageChecked;

int disarm(char *dst, unsigned int addr, unsigned int cmd);
int disarm_c(char *dst, unsigned int addr, unsigned int cmd);

extern const char *db_path;
extern const char *src_path; 
extern const char *c_path; 
extern const char *cpp_path; 
extern const char *os_path;

const unsigned int flags_type_mask          = 0x000000ff;

const unsigned int flags_type_unknown       = 0x00000000;
const unsigned int flags_type_arm_code      = 0x00000001;
const unsigned int flags_type_arm_byte      = 0x00000002;
const unsigned int flags_type_arm_word      = 0x00000003;
const unsigned int flags_type_arm_text      = 0x00000004;
const unsigned int flags_type_patch_table   = 0x00000005;
const unsigned int flags_type_jump_table    = 0x00000006;
const unsigned int flags_type_unused        = 0x00000007;
const unsigned int flags_type_rex           = 0x00000008;
const unsigned int flags_type_ns            = 0x00000009;
const unsigned int flags_type_ns_obj        = 0x0000000a;
const unsigned int flags_type_ns_ref        = 0x0000000b;
const unsigned int flags_type_dict          = 0x0000000c;
const unsigned int flags_type_classinfo     = 0x0000000d;
const unsigned int flags_type_arm_wtext     = 0x0000000e;
const unsigned int flags_type_data          = 0x0000000f;

const unsigned int flags_is_function        = 0x30000000;
const unsigned int flags_is_target          = 0x10000000;
const unsigned int flags_walked             = 0x40000000;
const unsigned int flags_include            = 0x80000000;

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

inline bool isFunctionStart(PAddr addr) {
    return (rom_flags_is_set(addr, flags_is_function));
}

const char *get_symbol_at(unsigned int addr);
const char *get_plain_symbol_at(unsigned int addr);

unsigned int decodeNSRef(FILE *newt, unsigned int i);
unsigned int decodeNSObj(FILE *newt, unsigned int i);
void extractStencils();

extern unsigned int branch_address_in_ROM(unsigned int addr, unsigned int cmd=0xffffffff);
extern unsigned int branch_address(unsigned int addr, unsigned int cmd=0xffffffff);

extern void checkCodeCoverage();

// For ROM Explorer:

extern void readSymbols(const char *cpp_filename, const char *plain_filename);
extern void zeroFlags();
extern void load_db(char const *path, char const *filename);
extern void preset_rom_use();
extern void checkScriptCoverage();
extern void writeLabel(FILE *newt, unsigned int i);
extern void writeComments(FILE* newt, unsigned int i);
extern void writeLabelIfNone(FILE *newt, unsigned int i);
extern const char *p_ascii(unsigned char c);
extern char hasLabel(unsigned int i);
extern unsigned char printable(unsigned char c);


#endif
