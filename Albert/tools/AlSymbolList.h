

// TODO: not yet used!
#if 0



#ifndef AL_SYMBOL_LIST_H
#define AL_SYMBOL_LIST_H

/**
 * \file AlSymbolList.h
 */

#include "AlSymbol.h"

#include <map>


typedef std::map < unsigned int, AlSymbol* > AlSymbolMap;


/**
 * This class manages all known symbols.
 */
class AlSymbolList 
{
public:
  AlSymbolList();
  ~AlSymbolList();
  AlSymbol *find(unsigned int addr);
  AlSymbol *at(unsigned int addr);
private:
  AlSymbolMap pMap;
};


/**
 * This class is used to iterate trough the symbol list.
 */
class AlSymbolListIterator
{
public:
  AlSymbolListIterator(AlSymbolList &list);
  ~AlSymbolListIterator();
  unsigned int address();
  AlSymbol *symbol();
  bool end();
  bool incr();
};


extern AlSymbolList gSymbolList;


#endif

#endif
