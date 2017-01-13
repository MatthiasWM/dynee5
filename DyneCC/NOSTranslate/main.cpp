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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


const char *gPath = 0;
const char *gAifFilename = 0;
const char *gRexFilename = 0;
const char *gDestPathname = 0;


char *concat(const char *a, const char *b)
{
    size_t n = strlen(a) + strlen(b);
    char *name = (char*)malloc(n+1);
    strcpy(name, a);
    strcat(name, b);
    return name;
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
                gAifFilename = concat(gPath, argv[++i]);
            }
        } else if (argc>i && argv[i+1] && strcmp(arg, "--rex")==0) {
            if (!gPath) {
                fprintf(stderr, "ERROR: must set --path before --rex\n");
                return 20;
            } else {
                gRexFilename = concat(gPath, argv[++i]);
            }
        } else if (argc>i && argv[i+1] && strcmp(arg, "--dest")==0) {
            if (!gPath) {
                fprintf(stderr, "ERROR: must set --path before --dest\n");
                return 20;
            } else {
                gDestPathname = concat(gPath, argv[++i]);
            }
        } else if (strcmp(arg, "--initialize")==0) {
            // load AIF
            // load REX
            // analyse and create monolith
            // create directories, headers, and source files
        } else if (strcmp(arg, "--merge")==0) {
            // create monolith if not done yet
            // walk all files in the dest path and fill in the missing pieces from the monolith
        } else if (strcmp(arg, "--clean")==0) {
            // walk all files in the dest path and remove intermediate code
        }
    }
    return 0;
}
