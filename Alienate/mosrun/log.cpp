/*
 mosrun - the MacOS MPW runtime emulator
 Copyright (C) 2013  Matthias Melcher
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 The author can be contacted at mosrun AT matthiasm DOT com.
 The latest source code can be found at http://code.google.com/p/dynee5/
 */

#include "log.h"

#include <stdarg.h>


static FILE *gMosLogFile = 0L;


/**
 * Return the file struct to which we log our messages.
 */
FILE *mosLogFile()
{
  return gMosLogFile;
}


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

