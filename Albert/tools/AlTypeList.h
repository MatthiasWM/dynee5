



#ifndef AL_TYPE_LIST_H
#define AL_TYPE_LIST_H

/**
 * \file AlTypeList.h
 */

#include "AlDatabaseObject.h"

#include <map>

class AlType;
typedef struct 
{
  bool operator()(char const *a, char const *b) const
  {
    return ::strcmp(a, b) < 0;
  }
} AlTypeListLess;
typedef std::map < const char*, AlType*, AlTypeListLess > AlSTLTypeList;


/**
 * This class manages all known types.
 */
class AlTypeList : public AlDatabaseObject
{
  friend class AlTypeListIterator;
public:
  AlTypeList();
  ~AlTypeList();
  bool read(AlDatabase &db);
  void write(AlDatabase &db, bool block);
  AlType *find(const char *key);
  AlType *at(const char *key);
  void set(const char *key, AlType *type);
  static bool less(const char *, const char *);
private:
  AlSTLTypeList pMap;
};


/**
 * This class is used to iterate trough the type list.
 */
class AlTypeListIterator
{
public:
  AlTypeListIterator(AlTypeList &map);
  ~AlTypeListIterator();
  const char *key();
  AlType *type();
  bool end();
  bool incr();
private:
  AlSTLTypeList::iterator pIterator;
  AlSTLTypeList &pMap;
};


extern AlTypeList gTypeList;


#endif
