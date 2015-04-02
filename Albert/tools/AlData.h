


#ifndef AL_DATA_H
#define AL_DATA_H

/**
 * \file AlData.h
 */

#include "AlDatabaseObject.h"


/**
 * This class hold information for any range of data in memory.
 */
class AlData : public AlDatabaseObject {
public:
  AlData();
  ~AlData();
  bool read(AlDatabase &db);
  void write(AlDatabase &db, bool block);
  unsigned int address();
  void address(unsigned int);
  unsigned int size();
  void size(unsigned int);
  const char *label();
  void label(const char *);
  // exports
  virtual void exportAsm(FILE *f);
  virtual void exportCppCall(FILE *f);
  virtual void exportCppCallEnd(FILE *f);
  virtual const char* stringCppCall();
protected:
  unsigned int pAddress;
  unsigned int pSize;
  char *pLabel;
};


#endif

