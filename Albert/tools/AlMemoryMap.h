


#ifndef AL_MEMORY_MAP_H
#define AL_MEMORY_MAP_H

/**
 * \file AlMemoryMap.h
 */

#include "AlDatabaseObject.h"

#include <map>

class AlData;
typedef std::map < unsigned int, AlData* > AlSTLMemoryMap;


/**
 * This class manages all known memory ranges.
 */
class AlMemoryMap : public AlDatabaseObject
{
  friend class AlMemoryMapIterator;
public:
  AlMemoryMap();
  ~AlMemoryMap();
  bool read(AlDatabase &db);
  void write(AlDatabase &db, bool block);
  AlData *find(unsigned int addr);
  unsigned int find(const char *name);
  AlData *at(unsigned int addr);
  void set(unsigned int addr, AlData *data);
private:
  AlSTLMemoryMap pMap;
};


/**
 * This class is used to iterate trough the memory map.
 */
class AlMemoryMapIterator
{
public:
  AlMemoryMapIterator(AlMemoryMap &map);
  ~AlMemoryMapIterator();
  unsigned int address();
  AlData *data();
  bool end();
  bool incr();
private:
  AlSTLMemoryMap::iterator pIterator;
  AlSTLMemoryMap &pMap;
};


extern AlMemoryMap gMemoryMap;


#endif
