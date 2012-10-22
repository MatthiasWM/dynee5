//
//  decomp.h
//  Albert
//
//  Created by Matthias Melcher on 01.04.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef DECOMP_H
#define DECOMP_H

#include <stdio.h>


int decompile_function(FILE *f, unsigned int start, unsigned int end);
void analyse_class(const char *name);

#endif
