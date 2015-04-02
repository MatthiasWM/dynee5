



#ifndef AL_TYPE_H
#define AL_TYPE_H

/**
 * \file AlType.h
 */

#include "AlDatabaseObject.h"


/**
 * This class hold information for any C or C++ type.
 */
class AlType : public AlDatabaseObject {
public:
  AlType();
  ~AlType();
  bool read(AlDatabase &db);
  void write(AlDatabase &db, bool block);
  const char *key();
  void key(const char *);
  const char *name();
  void name(const char *);
protected:
  char *pKey;
};


#endif

