


#ifndef AL_CPP_METHOD_H
#define AL_CPP_METHOD_H

/**
 * \file
 */

#include "AlCMethod.h"


/**
 * This class describes a "C++" method entry.
 */
class AlCPPMethod : public AlCMethod
{
public:
  AlCPPMethod();
  ~AlCPPMethod();
  bool read(AlDatabase &db);
  void write(AlDatabase &db, bool block);
  const char *prototype();
  void decodeDecoratedLabel(const char *label);
  static bool isCPPLabel(const char *label);
  void klass(const char *);
  const char *klass();
  // exports
  void exportAsm(FILE *f);
  void exportCppCall(FILE *f);
  void exportCppCallEnd(FILE *f);
  const char* stringCppCall();
private:
  char *pKlass;
};


#endif
