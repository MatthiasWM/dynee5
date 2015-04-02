

#include "AlDatabase.h"

#include "AlMemoryMap.h"

#include <stdio.h>
#include <string.h>
#include <memory.h>

/**
 * Create a database reader or writer.
 */
AlDatabase::AlDatabase(const char *filename, const char *mode) 
: pFilename(0L),
  pMode(0),
  pFile(0L),
  pLineNum(1),
  pIndent(0)
{
  if (!filename)
    throw("No Filename!");
  pFilename = strdup(filename);
  if (!mode) 
    throw("No mode!");
  if (*mode=='r') {
    pMode = 0;
  } else if (*mode=='w') {
    pMode = 1;
  } else {
    throw("Invalid mode, must be \"r\" or \"w\"!");
  }
  pFile = fopen(filename, mode);
  if (!pFile) 
    throw("Can't open file in desired mode!");
  if (pMode==0) {
    readNext();
  }
}


AlDatabase::~AlDatabase()
{
  if (pFile)
    fclose(pFile);
}


void AlDatabase::writeBlockBegin(const char *name)
{
  writeIndent();
  writeKey("begin");
  write(name);
  pIndent++;
  writeNewLine();
}


void AlDatabase::writeBlockEnd()
{
  pIndent--;
  writeIndent();
  write("end");
  writeNewLine();
}


void AlDatabase::write(const char *text)
{
  fputs(text, pFile);
}


void AlDatabase::writeNewLine()
{
  fputc('\n', pFile);
  pLineNum++;
}


void AlDatabase::write(const char *key, const char *value)
{
  writeIndent();
  writeKey(key);
  write(value);
  writeNewLine();
}


void AlDatabase::writeHex(const char *key, unsigned int value)
{
  writeIndent();
  writeKey(key);
  fprintf(pFile, "0x%08X", value);
  writeNewLine();
}


void AlDatabase::write(const char *key, int value)
{
  writeIndent();
  writeKey(key);
  fprintf(pFile, "%d", value);
  writeNewLine();
}


void AlDatabase::write(const char *key, bool value)
{
  writeIndent();
  writeKey(key);
  if (value) fputc('1', pFile); else fputc('0', pFile);
  writeNewLine();
}


void AlDatabase::writeKey(const char *key)
{
  write(key);
  fputc(' ', pFile);
}


void AlDatabase::writeIndent()
{
  int i;
  for (i=0; i<pIndent; i++) {
    fputc(' ', pFile);
    fputc(' ', pFile);
  }
}


void AlDatabase::read()
{
  for (;;) {
    const char *key = currentKey();
    if (strcmp(key, "begin")==0) {
      const char *value = currentValue();
      if (strcmp(value, "MemoryMap")==0) {
        gMemoryMap.readBlock(*this);
      } else {
        printErrValue();
      }
    } else {
      printErrKey();
    }
    if (!readNext())
      return;
  }
}


bool AlDatabase::readNext()
{
  char buf[255];
  if (fgets(buf, 254, pFile)) {
    pLineNum++;
    // extract the key:
    char *s = buf, *d = pCurrKey, *e;
    // remove all whitespace
    while (*s) {
      char c = *s;
      if (c>' ') break;
      ++s;
    }
    // copy all printables
    while (*s) {
      char c = *s;
      if (c<=' ') break;
      *d++ = c;
      ++s;
    }
    *d = 0;
    d = e = pCurrValue;
    // skip more whitespace
    while (*s) {
      char c = *s;
      if (c>' ') break;
      ++s;
    }
    // copy all printables to the 'value' field
    while (*s) {
      char c = *s;
      if (c<' ') break;
      *d++ = c;
      if (c>' ') e = d; // cut off trailing spaces
      ++s;
    }
    *e = 0;
    return true;
  } else {
    *pCurrKey = 0;
    *pCurrValue = 0;
    return false;
  }
}


unsigned int AlDatabase::currentValueAsUInt()
{
  unsigned int ret = 0;
  if (pCurrValue[0]=='0' && pCurrValue[1]=='x') {
    sscanf(pCurrValue+2, "%ux", &ret);
  } else {
    sscanf(pCurrValue, "%ud", &ret);
  }
  return ret;
}


int AlDatabase::currentValueAsInt()
{
  int ret = 0;
  sscanf(pCurrValue, "%d", &ret);
  return ret;
}


bool AlDatabase::currentValueAsBool()
{
  return (*pCurrValue != '0');
}


void AlDatabase::printErrKey()
{
  fprintf(stderr, "Unknown key '%s' in '%s:%d'\n", pCurrKey, pFilename, pLineNum);
}


void AlDatabase::printErrValue()
{
  fprintf(stderr, "Unknown value '%s' for key '%s' in '%s:%d'\n", pCurrValue, pCurrKey, pFilename, pLineNum);
}


