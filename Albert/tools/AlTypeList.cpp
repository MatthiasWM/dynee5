



#include "AlTypeList.h"
#include "AlType.h"


/**
 * This is the one global list for all known types.
 */
AlTypeList gTypeList;


AlTypeList::AlTypeList()
{
}


AlTypeList::~AlTypeList()
{
}


bool AlTypeList::read(AlDatabase &db)
{
  const char *key = db.currentKey();
  const char *val = db.currentValue();
  if (strcmp(key, "begin")==0) {
    if (strcmp(val, "Type")==0) {
      AlType *type = new AlType();
      type->readBlock(db);
    } else {
      db.printErrValue();
    }
  } else {
    return AlDatabaseObject::read(db);
  }
  return true;
}


void AlTypeList::write(AlDatabase &db, bool block)
{
  if (block) db.writeBlockBegin("TypeList");
  AlDatabaseObject::write(db, false);  
  for (AlTypeListIterator it(*this); !it.end(); it.incr()) {
    it.type()->write(db, true);
  }
  if (block) db.writeBlockEnd();
}


AlType *AlTypeList::find(const char *key)
{
  AlSTLTypeList::iterator it = pMap.find(key);
  if (it == pMap.end())
    return 0;
  else
    return it->second;
}


AlType *AlTypeList::at(const char *key)
{
  AlSTLTypeList::iterator it = pMap.find(key);
  if (it == pMap.end()) {
    AlType *type = new AlType();
    type->key(key);
    pMap.insert(std::make_pair(strdup(key), type));
    return type;
  } else {
    return it->second;
  }
}


void AlTypeList::set(const char *key, AlType *type)
{
  type->key(key);
  AlSTLTypeList::iterator it = pMap.find(key);
  if (it != pMap.end()) {
    delete it->second;
    it->second = type;
  } else {
    pMap.insert(std::make_pair(strdup(key), type));
  }
}




AlTypeListIterator::AlTypeListIterator(AlTypeList &map)
: pMap(map.pMap)
{
  pIterator = pMap.begin();
}


AlTypeListIterator::~AlTypeListIterator()
{
}


const char *AlTypeListIterator::key()
{
  return pIterator->first;
}


AlType *AlTypeListIterator::type()
{
  return pIterator->second;
}


bool AlTypeListIterator::end()
{
  return (pIterator==pMap.end());
}


bool AlTypeListIterator::incr()
{
  ++pIterator;
  return end();
}


