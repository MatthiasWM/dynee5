

// TODO: not yet used!
#if 0


#ifndef AL_SYMBOL_H
#define AL_SYMBOL_H

/**
 * \file AlSymbol.h
 */

#include "AlDatabaseObject.h"


/**
 * This class holds a symbol, its address, and information on the target type.
 */
class AlSymbol : public AlDatabaseObject {
public:
  AlSymbol();
  ~AlSymbol();
  void read(FILE *f);
  void write(FILE *f);
  unsigned int address();
  void address(unsigned int);
  const char *name();
  void name(const char *);
  const char *decodedName();
  void decodedName(const char *);
protected:
  unsigned int pAddress;
  char *pName;
  char *pDecodedName;
};


#endif

#endif
