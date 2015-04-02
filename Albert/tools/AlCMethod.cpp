

#include "AlCMethod.h"

#include "AlTypeList.h"
#include "AlTypeRef.h"

#include <string.h>
#include <stdlib.h>


AlCMethod::AlCMethod()
: pName(0L),
  pConst(false),
  pStatic(false)
{
}


AlCMethod::~AlCMethod()
{
  if (pName) free(pName);
}


bool AlCMethod::read(AlDatabase &db)
{
  const char *key = db.currentKey();
  if (strcmp(key, "name")==0) {
    name(db.currentValue());
  } else if (strcmp(key, "static")==0) {
    setStatic(db.currentValueAsBool());
  } else if (strcmp(key, "const")==0) {
    setConst(db.currentValueAsBool());
  } else {
    return AlARMCode::read(db);
  }
  return true;
}


void AlCMethod::write(AlDatabase &db, bool block)
{
  if (block) db.writeBlockBegin("CMethod");
  AlARMCode::write(db, false);
  if (name()) db.write("name", name());
  if (isStatic()) db.write("static", isStatic());
  if (isConst()) db.write("const", isConst());
  if (block) db.writeBlockEnd();
}


const char *AlCMethod::name() 
{
  if (pName) 
    return pName;
  else
    return label();
}


void AlCMethod::name(const char *l)
{
  if (pName)
    free(pName);
  if (l)
    pName = strdup(l);
  else
    pName = 0L;
}


void AlCMethod::addParameter(const char *p)
{
  int indirection = -1;
  // remove leading space
  for (;;p++) {
    if (*p==0 || *p>' ') break;
  }
  // "void" without an indirection is no type
  if (strcmp(p, "void")==0)
    return;
  // remove a preceding "const" keyword
  if (strncmp(p, "const ", 6)==0) {
    p+=6;
    // TODO: we must add that modifier to the indirection flags later
  }
  // make a copy that we can modify
  char buf[256];
  strcpy(buf, p);
  // find qualifiers
  char *s = buf;
  if (strncmp(s, "signed ", 7)==0) {
    s+=7;
  } else if (strncmp(s, "unsigned ", 9)==0) {
    s+=9;
  } // we may have a "const" here as well
  // extract the remainder of the type
  if (isalpha(*s)||*s=='_') {
    s++;
    for (;;) {
      if (isalnum(*s)||*s=='_') {
        s++;        
      } else {
        break;
      }
    }
  }
  if (*s==' ') {
    // indirection and or name follows
    if (strcmp(s, " *")==0 || strcmp(s, " const *")==0) {
      // TODO: extremely simplified test to find sinple indirection
      indirection = 1; // simple indirection
    }
  } else {
    indirection = 0; // direct type
  }
  *s = 0;
  //strcat(s, " (");
  //strcat(s, p);
  //strcat(s, ")");
  //printf("  Parameter: '%s'\n", buf);
  
  // create that type if it isn't there yet
  /*AlType *t =*/ gTypeList.at(buf);
  
  // function call parameters:
  // r0, r1, r2, r3, sp-4, sp-8, ...
  int store = nTypeRef; // TODO: this is *wrong*! nTypeRef is not the same as the parameter count (unless we add <unknown> as a possible parameter)!
  if (store>3) store = -4*(store-3);
  AlTypeRef *tr = new AlTypeRef(buf, indirection, store, p);
  add(tr);
  
  // TODO: implement me!
  // CBuffer
  // CBuffer const
  // CBuffer *
  // CBuffer *&
  // CBuffer (*)[13]
  // CBuffer const *
  // CBuffer const *const
  // CBuffer *const
  // CBuffer **
  // CBuffer (**)[17]
  // CBuffer ***
  // CBuffer &
  // CBuffer (&)[8]
  // CBuffer const &
  // CBuffer::Other
  // function pointers (*)(...)
}


void AlCMethod::exportAsm(FILE *f)
{
  AlARMCode::exportAsm(f);
  if (pName) fprintf(f, "\t@ name = '%s'\n", pName);
  if (pStatic) fprintf(f, "\t@ static\n");
  if (pConst) fprintf(f, "\t@ const\n");
}


void AlCMethod::exportCppCall(FILE *f)
{
  fprintf(f, "EXPORT AlCMethod\n");
  AlARMCode::exportAsm(f);
  if (pStatic) fprintf(f, "static ");
  if (pConst) fprintf(f, "const ");
  if (pName) fprintf(f, "%s\n", pName);
}


void AlCMethod::exportCppCallEnd(FILE *f)
{
}


const char* AlCMethod::stringCppCall()
{
  static char buf[1024];
  sprintf(buf, "%s(...)", pName);
  return buf;
}


