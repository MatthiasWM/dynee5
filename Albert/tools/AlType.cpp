



#include "AlType.h"

#include "AlTypeList.h"

#include <stdlib.h>
#include <string.h>


/**
 * Create a data object.
 */
AlType::AlType() 
: pKey(0L)
{
}


/**
 * Destroy te data object.
 */
AlType::~AlType()
{
  if (pKey) free(pKey);
}


bool AlType::read(AlDatabase &db)
{
  const char *keywd = db.currentKey();
  if (strcmp(keywd, "key")==0) {
    const char *val = db.currentValue();
    key(val);
    gTypeList.set(val, this);
  } else {
    return AlDatabaseObject::read(db);
  }
  return true;
}


void AlType::write(AlDatabase &db, bool block)
{
  if (block) db.writeBlockBegin("Type");
  AlDatabaseObject::write(db, false);
  if (key()) db.write("key", key());
  if (block) db.writeBlockEnd();
}


const char *AlType::key()
{
  return pKey;
}


void AlType::key(const char *k)
{
  if (pKey)
    free(pKey);
  if (k)
    pKey = strdup(k);
  else
    pKey = 0L;
}


