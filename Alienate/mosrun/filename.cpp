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

/** \file filename.cpp
 This module converts filenames and their path from one OS format into another.
 
 We need to conver file separators and font encodings.
 
 MS-DOS support at this point is minimal and will likely fail to produce
 anything useful if a drive ID (e.g. "C:\") is involved.
 */

#include "filename.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/**
 Guess the type of a file name with path.
 
 There is no sure way to determine the format of a filepath. This function
 guesses the type by adding indicators. A file path separator can be one
 indicator, a correct utf-8 character can be another one.
 
 However, Unix does allow ':' in file names, which could be interpreted as a
 Mac file separator. MacOS however allows '/' in file names (and the default
 installation of tcl/tk on Mac actually uses it.
 
 A correct UTF-8 sequence can also be a legal two-character MacOS sequnce.
 
 If there are no indicators at all, we return MOS_TYPE_UNKNOWN, which also
 indicates, that probably no conversion is needed.
 
 FIXME: a single word could be a drive name in MacOS or a relative file Unix. Sigh.
*/
int mosFilenameGuessType(const char *filename)
{
  int unix = 0, dos = 0, macos = 0;
  if (!filename || !*filename)
    return MOS_TYPE_UNKNOWN;
  int i, n = strlen(filename)-1;
  for (i=0; i<n; i++) {
    char c = filename[i];
    if (c=='/') {
      unix++;  // Unix filename
    } else if (c==':') {
      if (i==2) dos++; else macos++;  // Mac filename
    } else if (c=='\\') {
      dos++; // DOS filename
    } else if ((c&0xE0)==0xC0 && (filename[i+1]&0xC0)==0x80) {
      unix++; // UTF-8
    } else if ((c&0xF0)==0xE0
               && (filename[i+1]&0xC0)==0x80
               && (filename[i+2]&0xC0)==0x80) {
      unix++; // UTF-8
    } else if ((c&0xF8)==0xF0
               && (filename[i+1]&0xC0)==0x80
               && (filename[i+2]&0xC0)==0x80
               && (filename[i+3]&0xC0)==0x80) {
      unix++; // UTF-8
    } else if (c&0x80) {
      macos++; // not UTF-8, so probably MacOS
    }
  }
  
  // is there a clear winner?
  if (unix>dos && unix>macos) return MOS_TYPE_UNIX;
  if (macos>unix && macos>dos) return MOS_TYPE_MAC;
  if (dos>unix && dos>macos) return MOS_TYPE_DOS;
  
  // if not, go by personal preference
  if (macos>unix) return MOS_TYPE_MAC;
  if (unix>macos) return MOS_TYPE_UNIX;
  if (macos>dos) return MOS_TYPE_MAC;
  if (unix>dos) return MOS_TYPE_UNIX;
  if (dos>unix) return MOS_TYPE_DOS;
  
  // ok, so it's undecided:
  return MOS_TYPE_UNKNOWN;
}


static void convertFromMac(const char *filename, char *buffer)
{
  const char *src = filename;
  char *dst = buffer;
  
  // is the filename relative or absolute?
  char isRelative = false;
  if (filename[0]==':') {
    // if the filename starts with a ':', it must be relative
    isRelative = true;
  } else if (strchr(filename, ':')) {
    // if the filename contains a ':', but does not start with one, it must be absolute
    isRelative = false;
  } else {
    // it's only a filename or a directory name. In either case, it's relative (an absolte directory name would have a trailing ':'
    isRelative = true;
  }
  
  // now begin the path
  if (isRelative) {
    src++;
    while (*src==':') { // multiple ':' at the start go up a directory
      *dst++ = '.'; *dst++ = '.'; *dst++ = '/';
      src++;
    }
  } else {
    *dst++ = '/';
  }
  
  // now copy the remaining path members over
  for (;;) {
    unsigned char c = (unsigned char)*src++;
    if (c==':') {
      *dst++ = '/';
      while (*src==':') { // multiple ':' go up a directory
        *dst++ = '.'; *dst++ = '.'; *dst++ = '/';
        src++;
      }
    } else if (c>=128) {
      *dst++ = c; // FIXME: convert Mac Roman to UTF-8
    } else {
      *dst++ = c;
    }
    if (c==0)
      break;
  }
//  fprintf(stderr, "FromMac: '%s' = '%s'\n", filename, buffer);
}


static void convertFromDos(const char *filename, char *buffer)
{
  strcpy(buffer, filename); // TODO: later
}


static void convertToMac(const char *filename, char *buffer)
{
  const char *src = filename;
  char *dst = buffer;
  
  // is the filename relative or absolute?
  char isRelative = false;
  if (filename[0]=='/') {
    // if the filename starts with a '/', it must be absolute
    isRelative = false;
  } else {
    // if no '/' at the start, it's relative
    isRelative = true;
  }

  // now begin the path
  if (isRelative) {
    if (strncmp(src, "../", 3)==0) { // "../" = parent directory
      *dst++ = ':';
      src+=2; // point to the trainling slash which will generate the second ':'
    } else if (strncmp(src, "./", 2)==0) { // "./" = current directory
      src++; // point to the trainling slash which will generate the ':'
    } else if (strcmp(src, "..")==0) { // filename ends with ".."
      *dst++ = ':';
      *dst++ = ':';
      src+=2;
    } else if (strcmp(src, ".")==0) { // filename ends with "."
      *dst++ = ':';
      src+=1;
    } else {
      *dst++ = ':';   // start with a ':'
    }
  } else {
    src++;          // skip the first '/'
  }
  
  // now copy the remaining path members over
  for (;;) {
    unsigned char c = (unsigned char)*src++;
    if (c=='/') {
      while (*src=='/') { // multiple '/' don't do anything
        src++;
      }
      if (strncmp(src, "../", 3)==0) { // "../" = parent directory
        *dst++ = ':';
        src+=2; // point to the trainling slash
      } else if (strncmp(src, "./", 2)==0) { // "./" = current directory
        src++; // point to the trainling slash
      } else if (strcmp(src, "..")==0) { // filename ends with ".."
        *dst++ = ':';
        *dst++ = ':';
        src+=2;
      } else if (strcmp(src, ".")==0) { // filename ends with "."
        *dst++ = ':';
        src+=1;
      } else {
        *dst++ = ':';
      }
    } else if (c>=128) {
      *dst++ = c; // FIXME: convert UTF-8 to Mac Roman
    } else {
      *dst++ = c;
    }
    if (c==0)
      break;
  }
  // FIXME: when do we ned a trailing ':'
//  fprintf(stderr, "ToMac: '%s' = '%s'\n", filename, buffer);
}


static void convertToDos(const char *filename, char *buffer)
{
  strcpy(buffer, filename); // TODO: later
}


/**
 User API to converting full pathnames between formats.
 
 We determine the current filename for mat by guessing. Then we convert the
 path to Unix, simply because the Unix format incorporates all other features.
 And then at long last, we convert into the final format.
 */
char *mosFilenameConvertTo(const char *filename, int dstType)
{
  static char buffer[2048];
  char *tmpname;
  int srcType = mosFilenameGuessType(filename);
  if (srcType==dstType || srcType==MOS_TYPE_UNKNOWN) {
    strcpy(buffer, filename);
    return buffer;
  }
  switch (srcType) {
    case MOS_TYPE_MAC: convertFromMac(filename, buffer); break;
    case MOS_TYPE_DOS: convertFromDos(filename, buffer); break;
    default: strcpy(buffer, filename); break;
  }
  switch (dstType) {
    case MOS_TYPE_MAC: tmpname = strdup(buffer); convertToMac(tmpname, buffer); free(tmpname); break;
    case MOS_TYPE_DOS: tmpname = strdup(buffer); convertToDos(tmpname, buffer); free(tmpname); break;
    default: break; // nothing to do, Unix type filename is in the buffer
    // TODO: actually, OS X expects another UTF-8 encoding than standard Unix, so, yes, there might be work to do
  }
  return buffer;
}


const char *mosFilenameName(const char *filename)
{
  if (!filename || !*filename)
    return filename;
  int n = strlen(filename)-1;
  while (n>-1) {
    char c = filename[n];
    if (c=='/') break;  // Unix filename
    if (c==':') break;  // Mac filename
    if (c=='\\') break; // DOS filename
    n--;
  }
  return filename + n + 1;
}

