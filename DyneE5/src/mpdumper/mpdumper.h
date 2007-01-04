/*
    mpdumper.h - definitions file
           Copyright (C) 2004
           Alexey Danilchenko

    This file is part of the mpdumper.
    Mpdumper is free software; you can redistribute it and/or
    modify it for whatever needs providing that you don't break
    any copyrights and laws.
    
    Mpdumper is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    
    Author does not hold the responsibility for any damages or 
    improper or illegal use of the mpdumper.
    
    This software uses parts of GNU libiberty library covered by 
    GPL version 2 license. 

*/

// type definition
typedef unsigned char byte;
typedef byte word32[4];
typedef byte word16[2];


// endianness enum
enum EndianType {
    ET_BIG = 0,
    ET_LITTLE = 1
};

// enum item types
enum ItemType {
    IT_SECTION = 1,
    IT_PROCEDURE,
    IT_END_PROC,
    IT_VARIABLE,
    IT_TYPE,
    IT_STRUCTURE,
    IT_ARRAY,
    IT_SUBRANGE,
    IT_SET,
    IT_FILEINFO,
    IT_CONTIGUOUS_ENUMERATION,
    IT_DISCONTIGUOUS_ENUMERATION,
    IT_PROCEDURE_DECL,
    IT_BEGIN_NAMING_SCOPE,
    IT_END_NAMING_SCOPE,
    IT_BITFIELD
};

// enum language types
enum SectionLangType {
    LANG_NONE = 0,      // Low-level debugging data only
    LANG_C,             // C source level debugging data
    LANG_PASCAL,        // Pascal source level debugging data
    LANG_FORTRAN,       // Fortran-77 source level debugging data
    LANG_ASM            // ARM Assembler line number data
};

// section flags
enum SectionFlags {
    SF_UNSPECIFIED = 0,         // debug information type is unspecified
    SF_LINENUM,                 // debugging data contains line-number information
    SF_VARS,                    // debugging data contains information about top-level variables
    SF_LINENUM_VARS             // SF_LINENUM and SF_VARS together
};

// enum symbols flags
enum SymbolFlag {
    ASD_GLOBSYM  = 0,           // if the symbol is absolute
    ASD_ABSSYM   = 0x01,        // if the symbol is global - this can be bitmasked with the others
    
    // others are mutially exclusive (not masks)
    ASD_TEXTSYM  = 0x02,        // if the symbol names code
    ASD_DATASYM  = 0x04,        // if the symbol names data
    ASD_ZINITSYM = 0x06         // if the symbol names 0-initialised data
};


// declaration of the AIF header
struct AIFHeader {
    word32 bl_decompress;           // should be NOP - we don't support compressed image
    word32 bl_self_reloc;           // should be NOP - we don't support relocatable image
    word32 bl_zero_init;            // usually BL 0x40 - EB 00 00 0C (b.e.) or 0C 00 00 EB (l.e.)
    word32 entry_point;             // should be offset only - we don't support executable image
    word32 program_exit;            // should be SWI 0x11
    word32 ro_area_size;            // read-only area size
    word32 rw_area_size;            // read-write area size
    word32 debug_area_size;         // debug area size
    word32 zero_init_area_size;     // zero-init data area size
    word32 debug_type;              // debug type (least significant byte)
    word32 image_base;              // image base address
    word32 min_workspace;           // min workspace size
    word32 addressing_type;         // 32/26 bit addressing + data segment flag
    word32 data_base;               // base for data segment (if linked separately as indicated by previous flag)
    word32 reserved1;               // should be 0 for non-extended image
    word32 reserved2;               // should be 0 for non-extended image
    word32 debug_init;              // debug init instruction (NOP)
    byte zero_init[60];             // zero init code
};

// Declaration of the debug sections table
struct SectionItem {
    word32 length_type;             // length of this structure (should be 0x24) - in most significant 16 bits, 
                                    // item type (should be 1) - in least significant 16 bits
    byte language;                  // language type from SectionLangType constants
    byte flags;                     // one of SectionFlag constants
    byte unused;                    // should be 0
    byte debug_version;             // supposed to be 0x02
    
    word32 codestart;               // address of first instruction in this section
    word32 datastart;               // address of start of static data for this section
    word32 codesize;                // byte size of executable code in this section
    word32 datasize;                // byte size of the static data in this section
    word32 fileinfo;                // offset in the debugging area of the fileinfo item for
                                    //    this section (0 if no fileinfo item present)
    word32 debugsize;               // total byte length of debug data for this section
    word32 name_nsyms;              // string for pascal, C or fortran and number of symbols for LANG_NONE
};

// Declaration of the Symbol 
struct Symbol {
    word32 symbol;                  // most significant byte - SymbolFlag, the rest - byte offset in string table
    word32 value;                   // value of the symbol
};

// Declaration of the String type 
struct StringEntry {
    byte length;            // string length
    char str_data[1];       // string data
};

// Declaration of the String table
struct StringTable {
    word32 length;          // length of the table including this word
    StringEntry str[1];     // beginning of the string table
};

