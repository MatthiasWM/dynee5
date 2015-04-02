


#include "AlARMCode.h"
#include "AlTypeRef.h"

#include <stdlib.h>
#include <memory.h>


AlARMCode::AlARMCode()
: nTypeRef(0),
  pTypeRef(0L)
{
}


AlARMCode::~AlARMCode()
{
  int i;
  if (pTypeRef) {
    for (i=0; i<nTypeRef; i++) {
      free(pTypeRef[i]);
    }
    free(pTypeRef);
  }
}


bool AlARMCode::read(AlDatabase &db)
{
  const char *key = db.currentKey();
  const char *val = db.currentValue();
  if (strcmp(key, "begin")==0) {
    if (strcmp(val, "TypeRef")==0) {
      AlTypeRef *tr = new AlTypeRef();
      tr->readBlock(db);
      add(tr);
    }
  } else {
    return AlData::read(db);
  }
  return true;
}


void AlARMCode::write(AlDatabase &db, bool block)
{
  int i;
  
  if (block) db.writeBlockBegin("ARMCode");
  AlData::write(db, false);
  for (i=0; i<nTypeRef; i++) {
    pTypeRef[i]->write(db, true);
  }
  if (block) db.writeBlockEnd();
}


void AlARMCode::add(AlTypeRef *tRef)
{
  pTypeRef = (AlTypeRef**)realloc(pTypeRef, (nTypeRef+1)*sizeof(AlTypeRef*));
  pTypeRef[nTypeRef] = tRef;
  nTypeRef++;
}


void AlARMCode::exportAsm(FILE *f)
{
  int i;
  AlData::exportAsm(f);
  for (i=0; i<nTypeRef; i++) {
    pTypeRef[i]->exportAsm(f);
  }
}


void AlARMCode::exportCppCall(FILE *f)
{
  fprintf(f, "EXPORT AlARMCode\n");
  int i;
  AlData::exportCppCall(f);
  for (i=0; i<nTypeRef; i++) {
    pTypeRef[i]->exportCppCall(f);
  }
}


void AlARMCode::exportCppCallEnd(FILE *f)
{
}


