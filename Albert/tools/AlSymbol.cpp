

// TODO: not yet used!
#if 0


#include "AlSymbol.h"

#include <stdlib.h>
#include <string.h>


AlSymbol::AlSymbol()
: pAddress(0xdeadbeef),
  pName(0L),
  pDecodedName(0L)
{
}


AlSymbol::~AlSymbol()
{
  if (pName) free(pName);
  if (pDecodedName) free(pDecodedName);
}


void AlSymbol::read(FILE *) 
{
  // TODO: implement me
}


void AlSymbol::write(FILE *f) 
{
// TODO: implement me
}


unsigned int AlSymbol::address()
{
  if (this)
    return pAddress;
  else
    return 0xdeadbeef;
}


void AlSymbol::address(unsigned int addr)
{
  if (this)
    pAddress = addr;
  else
    throw(0L);
}


const char *AlSymbol::name() 
{
  if (this) 
    return pName;
  else
    return 0L;
}


void AlSymbol::name(const char *n)
{
  if (this) {
    if (pName) free(pName);
    if (n)
      pName = strdup(n);
    else 
      pName = 0L;
  } else 
    throw(0L);
}


const char *AlSymbol::decodedName()
{
  if (this) 
    return pDecodedName;
  else
    return 0L;
}


void AlSymbol::decodedName(const char *n)
{
  if (this) {
    if (pDecodedName) free(pDecodedName);
    if (n)
      pDecodedName = strdup(n);
    else 
      pName = 0L;
  } else 
    throw(0L);
}


#endif
