


#include "AlMemoryMap.h"
#include "AlData.h"
#include "AlARMCode.h"
#include "AlCMethod.h"
#include "AlCPPMethod.h"


/**
 * This is the one global map of all things with a fixed place inmemory.
 */
AlMemoryMap gMemoryMap;


AlMemoryMap::AlMemoryMap()
{
}


AlMemoryMap::~AlMemoryMap()
{
}


bool AlMemoryMap::read(AlDatabase &db)
{
  const char *key = db.currentKey();
  const char *val = db.currentValue();
  if (strcmp(key, "begin")==0) {
    if (strcmp(val, "Data")==0) {
      AlData *data = new AlData();
      data->readBlock(db);
    } else if (strcmp(val, "ARMCode")==0) {
      AlData *data = new AlARMCode();
      data->readBlock(db);
    } else if (strcmp(val, "CMethod")==0) {
      AlData *data = new AlCMethod();
      data->readBlock(db);
    } else if (strcmp(val, "CPPMethod")==0) {
      AlData *data = new AlCPPMethod();
      data->readBlock(db);
    } else {
      db.printErrValue();
    }
  } else {
    return AlDatabaseObject::read(db);
  }
  return true;
}


void AlMemoryMap::write(AlDatabase &db, bool block)
{
  if (block) db.writeBlockBegin("MemoryMap");
  AlDatabaseObject::write(db, false);  
  for (AlMemoryMapIterator it(*this); !it.end(); it.incr()) {
    it.data()->write(db, true);
  }
  if (block) db.writeBlockEnd();
}


AlData *AlMemoryMap::find(unsigned int addr)
{
  AlSTLMemoryMap::iterator it = pMap.find(addr);
  if (it == pMap.end())
    return 0;
  else
    return it->second;
}


unsigned int AlMemoryMap::find(const char *name)
{
  AlSTLMemoryMap::iterator it;
  for (it = pMap.begin(); it != pMap.end(); ++it) {
    AlData *d = it->second;
    if (d && d->label()) {
      if (strcmp(d->label(), name)==0)
        return d->address();
    }
  }
  return 0xffffffff;
}


AlData *AlMemoryMap::at(unsigned int addr)
{
  AlSTLMemoryMap::iterator it = pMap.find(addr);
  if (it == pMap.end()) {
    AlData *data = new AlData();
    data->address(addr);
    pMap.insert(std::make_pair(addr, data));
    return data;
  } else {
    return it->second;
  }
}


void AlMemoryMap::set(unsigned int addr, AlData *data)
{
  data->address(addr);
  AlSTLMemoryMap::iterator it = pMap.find(addr);
  if (it != pMap.end()) {
    delete it->second;
    it->second = data;
  } else {
    pMap.insert(std::make_pair(addr, data));
  }
}




AlMemoryMapIterator::AlMemoryMapIterator(AlMemoryMap &map)
: pMap(map.pMap)
{
  pIterator = pMap.begin();
}


AlMemoryMapIterator::~AlMemoryMapIterator()
{
}


unsigned int AlMemoryMapIterator::address()
{
  return pIterator->first;
}


AlData *AlMemoryMapIterator::data()
{
  return pIterator->second;
}


bool AlMemoryMapIterator::end()
{
  return (pIterator==pMap.end());
}


bool AlMemoryMapIterator::incr()
{
  ++pIterator;
  return end();
}


