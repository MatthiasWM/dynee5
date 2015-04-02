

#include "AlTypeRef.h"

#include <stdlib.h>
#include <string.h>


AlTypeRef::AlTypeRef()
: pType(0L),
  pIndirection(-1),
  pStore(0)
{
}


AlTypeRef::AlTypeRef(const char *type, unsigned int indir, int store, const char *name)
: pType(strdup(type)),
  pIndirection(indir),
  pStore(store),
  pName(strdup(name))
{
}


AlTypeRef::~AlTypeRef()
{
  if (pType) free(pType);
}


void AlTypeRef::type(const char *t)
{
  if (pType)
    free(pType);
  if (t)
    pType = strdup(t);
  else
    pType = 0L;
}


bool AlTypeRef::read(AlDatabase &db)
{
  const char *key = db.currentKey();
  if (strcmp(key, "type")==0) {
    type(db.currentValue());
  } else if (strcmp(key, "indir")==0) {
    indirection(db.currentValueAsInt());
  } else if (strcmp(key, "store")==0) {
    store(db.currentValueAsInt());
  } else {
    return AlDatabaseObject::read(db);
  }
  return true;
}


void AlTypeRef::write(AlDatabase &db, bool block)
{
  if (block) db.writeBlockBegin("TypeRef");
  AlDatabaseObject::write(db, false);
  if (type()) db.write("type", type());
  if (indirection()!=-1) db.write("indir", indirection());
  if (store()!=-1) db.write("store", store());
  if (block) db.writeBlockEnd();
}


void AlTypeRef::exportAsm(FILE *f)
{
  // print store
  if (pStore>=0)
    fprintf(f, "\t@ ARM R%d = ", pStore);
  else if (pStore<-1) 
    fprintf(f, "\t@ frame[%d] = ", pStore);
  else
    fprintf(f, "\t@ Unknown location %d = ", pStore);
  // print data type
  if (pType) {
    fprintf(f, "type: '%s'", pType);
  } else {
    fprintf(f, "unknown type");
  }
  // indirection
  if (pIndirection==0)
    ; //fprintf(f, "");
  else if (pIndirection==1)
    fprintf(f, "*");
  else 
    fprintf(f, " (unknown indirection)");
  // end of export
  fprintf(f, "\n");
}


void AlTypeRef::exportCppCall(FILE *f)
{
  fputs(AlTypeRef::stringCpp(), f);
}


void AlTypeRef::exportCppCallEnd(FILE *f)
{
}


const char* AlTypeRef::stringCpp()
{
  static char buf[1024];
  char *dst = buf;
  
  if (pType) {
    dst += sprintf(dst, "%s", pType);
  } else {
    dst += sprintf(dst, "<unknown type>");
  }
  if (pIndirection==0)
    ;
  else if (pIndirection==1)
    dst += sprintf(dst, "*");
  else
    dst += sprintf(dst, "<unknown indirection>");
  
  return buf;
}


