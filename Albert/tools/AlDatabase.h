

#ifndef AL_DATABASE_H
#define AL_DATABASE_H

/**
 * \file AlDatabase.h
 */

#include <stdio.h>


/**
 * Manage a file on disk that can be loaded or saved.
 */
class AlDatabase 
{
public:
  AlDatabase(const char *filename, const char *mode);
  ~AlDatabase();
  
  void writeBlockBegin(const char *name);
  void writeBlockEnd();
  void writeKey(const char *key);
  void write(const char *text);
  void write(const char *key, const char *value);
  void write(const char *key, unsigned int value);
  void write(const char *key, int value);
  void write(const char *key, bool value);
  void writeHex(const char *key, unsigned int value);
  void writeNewLine();
  void writeIndent();
  
  void read();
  bool readNext();
  const char *currentKey() { return pCurrKey; }
  const char *currentValue() { return pCurrValue; }
  unsigned int currentValueAsUInt();
  int currentValueAsInt();
  bool currentValueAsBool();
  
  void printErrKey();
  void printErrValue();

private:
  char *pFilename;
  int pMode;
  FILE *pFile;
  int pLineNum;
  int pIndent;
  char pCurrKey[255];
  char pCurrValue[255];
};


#endif

