

#ifndef AL_C_METHOD_H
#define AL_C_METHOD_H

/**
 * \file
 */

#include "AlARMCode.h"


/**
 * This class describes a "C" method entry.
 */
class AlCMethod : public AlARMCode
{
public:
  AlCMethod();
  ~AlCMethod();
  bool read(AlDatabase &db);
  void write(AlDatabase &db, bool block);
  void name(const char *n);
  const char *name();
  void addParameter(const char *p);
  bool isConst() { return pConst; }
  void setConst(bool b) { pConst = b; }
  bool isStatic() { return pStatic; }
  void setStatic(bool b) { pStatic = b; }
  // exports
  void exportAsm(FILE *f);
  void exportCppCall(FILE *f);
  void exportCppCallEnd(FILE *f);
  const char* stringCppCall();
protected:
  char *pName;
  bool pConst, pStatic;
};


#endif
