//
//  log.cpp
//  mosrun
//
//  Created by Matthias Melcher on 30.07.13.
//  Copyright (c) 2013 Matthias Melcher. All rights reserved.
//

#include "log.h"

#include <stdarg.h>


static FILE *gMosLogFile = 0L;


/**
 * Set the destination file for logging.
 * This call does not create or close any file handles.
 */
void mosLogTo(FILE *out)
{
  gMosLogFile = out;
}


/** 
 * Log text to a file using print() syntax.
 */
void mosLog(const char *format, ...)
{
  if (gMosLogFile) {
    va_list va;
    va_start(va, format);
    vfprintf(gMosLogFile, format, va);
    va_end(va);
  }
}


/**
 * Print text to stderr about issues with the emulation.
 */
void mosWarning(const char *format, ...)
{
  fprintf(stderr, "MOSRUN - WARNING!\n");
  va_list va;
  va_start(va, format);
  vfprintf(stderr, format, va);
  va_end(va);
}


/**
 * Print text to stderr about severe errors in the meulation.
 * After calling mosError, the application should sfely exit.
 */
void mosError(const char *format, ...)
{
  fprintf(stderr, "MOSRUN - ERROR!\n");
  va_list va;
  va_start(va, format);
  vfprintf(stderr, format, va);
  va_end(va);
}

