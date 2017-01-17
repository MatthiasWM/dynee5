//
//  main.hpp
//  NOSTranslate
//
//  Created by Matthias Melcher on 1/13/17.
//  Copyright © 2017 elektriktrick. All rights reserved.
//

#ifndef main_h
#define main_h

#include <stdio.h>
#include <stdlib.h>


typedef unsigned int uint;
typedef unsigned char ubyte;


extern class NTMemory Mem;
extern class NTSymbolList SymbolList;


extern uint readWord(FILE *f);


#endif /* main_h */
