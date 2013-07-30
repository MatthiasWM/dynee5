//
//  log.h
//  mosrun
//
//  Created by Matthias Melcher on 30.07.13.
//  Copyright (c) 2013 Matthias Melcher. All rights reserved.
//

#ifndef __mosrun__log__
#define __mosrun__log__


#include <stdio.h>


void mosLogTo(FILE *out);
void mosLog(const char *format, ...);
void mosWarning(const char *format, ...);
void mosError(const char *format, ...);


#endif /* defined(__mosrun__log__) */
