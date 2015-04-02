


#ifndef AL_TYPE_REF
#define AL_TYPE_REF


#include "AlDatabaseObject.h"


/**
 * This class connects a register to a C/C++ type.
 */
class AlTypeRef : public AlDatabaseObject
{
public:
  AlTypeRef();
  AlTypeRef(const char *type, unsigned int indir, int store, const char *name);
  ~AlTypeRef();
  bool read(AlDatabase &db);
  void write(AlDatabase &db, bool block);
  const char *type() { return pType; }
  void type(const char *);
  int indirection() { return pIndirection; }
  void indirection(int i) { pIndirection = i; }
  int store() { return pStore; }
  void store(int s) { pStore = s; }
  // exports
  void exportAsm(FILE *f);
  void exportCppCall(FILE *f);
  void exportCppCallEnd(FILE *f);
  const char* stringCpp();
  const char* name() { return pName; }
//private:
  char *pType;
  int pIndirection;
  int pStore;
  char *pName;
};


#endif
