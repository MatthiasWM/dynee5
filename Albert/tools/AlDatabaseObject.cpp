


#include "AlDatabaseObject.h"

#include <string.h>


/**
 * Default constructor for theis class.
 */
AlDatabaseObject::AlDatabaseObject()
{
}


AlDatabaseObject::~AlDatabaseObject()
{
}


void AlDatabaseObject::readBlock(AlDatabase &db)
{
  db.readNext();
  for (;;) {
    const char *key = db.currentKey();
    if (strcmp(key, "end")==0) {
      return;
    } else if (read(db)) {
      db.readNext();
    } else {
      db.printErrKey();
      db.readNext();
    }
  }
}


bool AlDatabaseObject::read(AlDatabase &db)
{
  return false;
}


void AlDatabaseObject::write(AlDatabase &/*db*/, bool /*block*/)
{
}



