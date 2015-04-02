

#ifndef AL_DATABASE_OBJECT_H
#define AL_DATABASE_OBJECT_H

/**
 * \file AlDatabaseObject.h
 */

#include "AlObject.h"
#include "AlDatabase.h"

#include <stdio.h>


/**
 * Base class for all objects that can go into a database.
 */
class AlDatabaseObject : public AlObject 
{
public:
  AlDatabaseObject();
  ~AlDatabaseObject();
  void readBlock(AlDatabase &db);
  virtual bool read(AlDatabase &db);
  virtual void write(AlDatabase &db, bool block);
};


#endif

