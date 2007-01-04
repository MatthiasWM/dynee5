/*
    mpdumper.cpp - main file
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <algorithm>
#include <vector>

#include "demangle.h"
#include "mpdumper.h"

using namespace std;


// ------------------------ Global data ------------------------ 

// endian type
static EndianType endian = ET_BIG;

// NOP operation encoded in big and little endians
static byte prog_exit_big_endian[] = {0xEF, 0x00, 0x00, 0x11};
static byte prog_exit_little_endian[] = {0x11, 0x00, 0x00, 0xEF};

// file data
static AIFHeader header;
static Symbol* symbols = 0;
static byte * string_table = 0;
static byte * debug_data = 0;

static int max_name_length = 0;

// for symbol ordering
bool do_sorting_by_name = false;
bool do_sorting_by_value = false;
bool do_alternative_print = false;
bool do_demangling = false;

vector<Symbol *> ordered_symbols;


// ------------------------ Functions ------------------------ 

// retrieves word32 from given address with given endianness
unsigned long get_word32 (byte* data) {
    unsigned long value = 0;
    
    if (endian == ET_BIG) {
        value = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
    }
    else {
        value = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];
    }
    
    return value;
}

// retrieves word16 from given address with given endianness
unsigned int get_word16 (byte* data) {
    unsigned int value = 0;
    
    if (endian == ET_BIG) {
        value = (data[0] << 8) | data[1];
    }
    else {
        value = (data[1] << 8) | data[0];
    }
    
    return value;
}

// recognising the endian
void recognise_endian () {
    if (memcmp (header.program_exit, 
                prog_exit_big_endian, 
                sizeof (header.entry_point)) == 0)
    {
        endian = ET_BIG;
    }
    else if (memcmp (header.program_exit, 
                     prog_exit_little_endian, 
                     sizeof (header.entry_point)) == 0) 
    {
        endian = ET_LITTLE;
    }
    else {
        printf ("WARNING: cant's deduce byte endian-ness - using big-endian by default!\n");
        endian = ET_BIG;
    }
}

// retrieves name by given offset in a string table
char* get_name(unsigned long offset) {
    StringTable * strings = (StringTable *) string_table;
    
    if (offset < get_word32(strings->length))
    {
        StringEntry * str = (StringEntry *) (string_table + offset);
        return str->str_data;
    }
    
    return "INVALID OFFSET!";
}

// introducing ordering on SymbolData domain
bool lessSymbol (Symbol* s1, Symbol* s2)
{
    if (do_sorting_by_name) {
        unsigned int flags1 = (unsigned int)(get_word32(s1->symbol) >> 24) & 0xF & ~ASD_ABSSYM;
        unsigned int flags2 = (unsigned int)(get_word32(s2->symbol) >> 24) & 0xF & ~ASD_ABSSYM;
        char * name1 = get_name (get_word32(s1->symbol) & 0xFFFFFF);
        char * name2 = get_name (get_word32(s2->symbol) & 0xFFFFFF);
        
        if (flags1 == flags2)
        {
            return strcmp (name1, name2) < 0 ;
        }
        else {
            return flags1 > flags2;     // to have all data at the beginning
        }
    }
    else if (do_sorting_by_value) {
        return get_word32(s1->value) < get_word32(s2->value);
    }
    
    return false;
}

// process symbol
void process_symbol (Symbol * symbol) {
    unsigned int flags = (unsigned int)(get_word32(symbol->symbol) >> 24) & 0xF;
    char * name = get_name (get_word32(symbol->symbol) & 0xFFFFFF);
    unsigned int value = get_word32(symbol->value);
    unsigned int is_global = flags & ASD_ABSSYM;
    char * demangled_name = name;
    bool do_free = false;
    
    // demangle name if needed
    if (do_demangling) { // FIXME: for testing only!
        demangled_name = cplus_demangle (name, DMGL_PARAMS|DMGL_ANSI|DMGL_TYPES);
        if (demangled_name) {
            do_free = true;
        }
        else {
            demangled_name = name;
        }
    }
    
    int len = strlen(demangled_name);
    if (len>max_name_length)
        max_name_length = len;
        
    // reset global from flags
    flags &= ~ASD_ABSSYM;
    
    if (do_alternative_print) {
        // alternative print - (value name)
        printf ("   %08X %s\n", value, demangled_name);
    }
    else {
        if (!is_global)
        {
            printf ("   Non global ");
        }
        else {
            printf ("   ");
        }
        
        // in the mean time we just print out symbol information
        if (flags == ASD_DATASYM || flags == ASD_ZINITSYM)
        {
            printf ("Data  symbol: %s = %08X\n", demangled_name, value);
        }
        else if (flags == ASD_TEXTSYM) {
            printf ("Code  symbol: %s = %08X\n", demangled_name, value);
        }
        else {
            printf ("Other symbol: %s = %08X\n", demangled_name, value);
        }
    }
    
    // free data
    if (do_free)
    {
        free (demangled_name);
    }
}

// process symbol table
void process_symbol_table (unsigned long nsyms) {
    printf ("Symbol table has: %08X (%u) entries\n", nsyms, nsyms);
    
    for (unsigned long i=0; i<nsyms; i++) {
        if (do_sorting_by_name || do_sorting_by_value) {
            ordered_symbols.push_back(&symbols[i]);
        }
        else {
            process_symbol (&symbols[i]);
        }
    }
    
    // order symbols if requested
    if (do_sorting_by_name || do_sorting_by_value) {
        sort (ordered_symbols.begin(), ordered_symbols.end(), lessSymbol);
        for (unsigned long i=0; i<ordered_symbols.size(); i++) {
            process_symbol (ordered_symbols[i]);
        }
    }
    
    printf ("Maximum symbol name length %d characters", max_name_length);
}

// prints program usage and exits
void usage (char * name) {
    printf ("Usage: %s [-ndxa] <image file name>\n", name);
    printf ("\tWhere options:\n"
            "\t\tn - sort symbols by category, name\n"
            "\t\tx - sort symbols by value\n"
            "\t\td - try to do symbol name de-mangling\n"
            "\t\ta - alternative printout format\n");
    
    exit(1);
}

//
// Main function:
//  opens the specified file, reads the header, sets the endianness, 
//  loads all the data structures (symbols and string table) and 
//  process all symbols
//
int main (int argc, char * argv[]) {
    FILE* fi = 0;
    char *filename = 0;
    
    if (argc < 2 || argc > 3) {
        usage (argv[0]);
    }
    
    // check for sorting option
    if (argv[1][0] == '-') 
    {
        if (argc < 3)
        {
            usage (argv[0]);
        }
        // loop through the arguments
        for (int i=1; i<strlen(argv[1]); i++) {
            switch (argv[1][i]) {
                case 'n':
                case 'N':
                    if (!do_sorting_by_value)
                        do_sorting_by_name = true;
                    else
                        usage (argv[0]);
                    break;
                    
                case 'x':
                case 'X':
                    if (!do_sorting_by_name)
                        do_sorting_by_value = true;
                    else
                        usage (argv[0]);
                    break;
                    
                case 'a':
                case 'A':
                    do_alternative_print = true;
                    break;

                case 'd':
                case 'D':
                    do_demangling = true;
                    break;

                default:
                    usage (argv[0]);
            }
        }

        filename = argv[2];
    }
    else if (argc > 2) {
        usage (argv[0]);
    }
    else {
        filename = argv[1];
    }
    
    // try to open file
    if (!(fi = fopen (filename, "rb"))) {
        printf ("Can't open file: %s\n", filename);
        usage (argv[0]);
    }
    
    // set demangling style
    cplus_demangle_set_style (arm_demangling);
    
    // read the header
    if (fread ((void*)&header, 1, sizeof(header), fi) == sizeof(header))
    {
        // recognise endianness
        recognise_endian ();
        
        unsigned long debug_offset = sizeof(header) + get_word32(header.ro_area_size) + get_word32(header.rw_area_size);
        unsigned long debug_size = get_word32(header.debug_area_size);
        
        // try to allocate enough memory for the debug info
        debug_data = new byte[debug_size];
        
        // attempt to read symbols
        if (!fseek (fi, debug_offset, SEEK_SET))
            if (fread ((void*)debug_data, 1, debug_size, fi) == debug_size)
            {
                // all data read successfully - set structures
                SectionItem * section = (SectionItem *) debug_data;
                int section_type = get_word32(section->length_type) & 0xFFFF;
                
                if (section_type == 1 && section->language == LANG_NONE)
                {
                    // set up global variables
                    symbols = (Symbol *) (debug_data + sizeof (SectionItem));
                    string_table = debug_data + sizeof (SectionItem) + get_word32(section->name_nsyms)*sizeof(Symbol);
                    
                    // process all symbols
                    process_symbol_table(get_word32(section->name_nsyms));
                }
                else {
                    printf ("Unsupported section type (%d) or language (%d) !", section_type, section->language);
                }
            }
            else {
                printf ("Invalid file size for: %s\n", argv[1]);
            }
        
    }
    else {
        printf ("Invalid file size for: %s\n", argv[1]);
    }
    
    // clean up
    delete[] debug_data;
    
    // close all
    fclose (fi);

    return 0;
}

