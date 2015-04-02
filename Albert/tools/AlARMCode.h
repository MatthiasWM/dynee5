

#ifndef AL_ARM_CODE_H
#define AL_ARM_CODE_H

/**
 * \file
 */

#include "AlData.h"


class AlTypeRef;


/**
 * This type marks memory that is executed by the ARM CPU somehow.
 */
class AlARMCode : public AlData {
public:
  AlARMCode();
  ~AlARMCode();
  bool read(AlDatabase &db);
  void write(AlDatabase &db, bool block);
  void add(AlTypeRef *);
  // exports
  void exportAsm(FILE *f);
  void exportCppCall(FILE *f);
  void exportCppCallEnd(FILE *f);
//protected:
  unsigned int nTypeRef;
  AlTypeRef **pTypeRef;
};


#endif


