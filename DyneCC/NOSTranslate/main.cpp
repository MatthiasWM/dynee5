//
//  main.cpp
//  NOSTranslate
//
//  Created by Matthias Melcher on 1/13/17.
//  Copyright © 2017 elektriktrick. All rights reserved.
//

/*
 
 This is the first half of a binary translator for the NewtonOS ROM that comes
 as part of the NewtonOS Driver Developer Kit.
 
 NOSTranslate converts the binary codes contained in the Apple MessagePad ROM
 into an intermediate code which can then be compiled back into a binary file
 using a modern C++ compiler.
 
 NOSTranslate generates a C-style intermediate code by using knowledge in the 
 DDK files and by static analysis. The intermediate code must be manually
 modified in critical sections that are not directly translateable.
 
 To protect the rights of the respective rights holders, no intermediate code
 shall be published or checked into git. Only manually created code shall be 
 checked in. Code derived from the ROM shall be created and merged with the
 manually created code by reading ROM/AIF/REX files provided by the user. If
 a NewtonOS binary is created, it shall verify the existence of a valid ROM
 that the end user must dowload from his personal MessagePad.

 Use: NOSTranslate --path ~/dev --aif ROM.aif --rex rex.bin ...
   --initialize: create everything from scratch; all developer code is lost
   --merge: analyse binaries and merge findings with developer code
   --clean: remove all non-developer code from the source files

 */

#include "main.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "NTMemory.hpp"


const char *gPath = 0;
const char *gAIFFilename = 0;
const char *gREXFilename = 0;
const char *gDestPathname = 0;

bool gAIFImageFound = false;
bool gREXImageFound = false;

// access to the NewtonOS memory image
NTMemory Mem;


char *concat(const char *a, const char *b)
{
    size_t n = strlen(a) + strlen(b);
    char *name = (char*)malloc(n+1);
    strcpy(name, a);
    strcat(name, b);
    return name;
}


size_t filesize(FILE *f)
{
    size_t curr = ftell(f);
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, curr, SEEK_SET);
    return size;
}


bool loadAIFImage(const char *filename)
{
    // verify that the given filename does point to the correct AIF file
    // tell the memory manager to load the binary data from file
    // SIG: E1A00000E1A00000EB00000C00000000, SIZE: 9,350,173 BYTES
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Can't open AIF file '%s': %s\n", filename, strerror(errno));
        return false;
    }
    if (filesize(f)!=9349820) {
        fprintf(stderr, "Can't open AIF file '%s': Unexpected file size (expected 9349820, found %ld)\n", filename, filesize(f));
        return false;
    }
    unsigned char sig[16] = { 0xE1, 0xA0, 0x00, 0x00, 0xE1, 0xA0, 0x00, 0x00, 0xEB, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00 };
    unsigned char buf[16];
    fread(buf, 16, 1, f);
    if (memcmp(sig, buf, 16)!=0) {
        fprintf(stderr, "Can't open AIF file '%s': Unexpected file signature\n", filename);
        return false;
    }
    return Mem.read(0, filename, 0x00000080, 0x0071FC4C);
}


bool loadREXImage(const char *filename)
{
    // verify that the given filename does point to the correct REX file
    // tell the memory manager to load the binary data from file
    // SIG: 524578426C6F636B000098E600000001, SIZE: 845,149 bytes
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Can't open REX file '%s': %s\n", filename, strerror(errno));
        return false;
    }
    if (filesize(f)!=844796) {
        fprintf(stderr, "Can't open REX file '%s': Unexpected file size (expected 844796, found %ld)\n", filename, filesize(f));
        return false;
    }
    unsigned char sig[16] = { 0x52, 0x45, 0x78, 0x42, 0x6C, 0x6F, 0x63, 0x6B, 0x00, 0x00, 0x98, 0xE6, 0x00, 0x00, 0x00, 0x01 };
    unsigned char buf[16];
    fread(buf, 16, 1, f);
    if (memcmp(sig, buf, 16)!=0) {
        fprintf(stderr, "Can't open REX file '%s': Unexpected file signature\n", filename);
        return false;
    }
    return Mem.read(0x0071FC4C, filename, 0x00000000, 0x000CE55D);
}


bool loadExternalResources()
{
    if (loadAIFImage(gAIFFilename)==false) return false;
    if (loadREXImage(gREXFilename)==false) return false;
    return true;
}


int main(int argc, const char * argv[])
{
    for (int i=1; i<argc; ++i) {
        const char *arg = argv[i];
        if (!arg) break;
        if (argc>i && argv[i+1] && strcmp(arg, "--path")==0) {
            gPath = strdup(argv[++i]);
        } else if (argc>i && argv[i+1] && strcmp(arg, "--aif")==0) {
            if (!gPath) {
                fprintf(stderr, "ERROR: must set --path before --aif\n");
                return 20;
            } else {
                gAIFFilename = concat(gPath, argv[++i]);
            }
        } else if (argc>i && argv[i+1] && strcmp(arg, "--rex")==0) {
            if (!gPath) {
                fprintf(stderr, "ERROR: must set --path before --rex\n");
                return 20;
            } else {
                gREXFilename = concat(gPath, argv[++i]);
            }
        } else if (argc>i && argv[i+1] && strcmp(arg, "--dest")==0) {
            if (!gPath) {
                fprintf(stderr, "ERROR: must set --path before --dest\n");
                return 20;
            } else {
                gDestPathname = concat(gPath, argv[++i]);
            }
        } else if (strcmp(arg, "--initialize")==0) {
            loadExternalResources();
            // analyse and create monolith
            // create directories, headers, and source files
        } else if (strcmp(arg, "--merge")==0) {
            loadExternalResources();
            // create monolith if not done yet
            // walk all files in the dest path and fill in the missing pieces from the monolith
        } else if (strcmp(arg, "--clean")==0) {
            // walk all files in the dest path and remove intermediate code
        }
    }
    return 0;
}
