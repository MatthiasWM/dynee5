


#include "AlData.h"

#include "AlMemoryMap.h"

#include <stdlib.h>
#include <string.h>


/**
 * Create a data object.
 */
AlData::AlData() 
: pAddress(0xdeadbeef),
  pSize(0),
  pLabel(0L)
{
}


/**
 * Destroy te data object.
 */
AlData::~AlData()
{
  if (pLabel) free(pLabel);
}


bool AlData::read(AlDatabase &db)
{
  const char *key = db.currentKey();
  if (strcmp(key, "addr")==0) {
    unsigned int addr = db.currentValueAsUInt();
    address(addr);
    gMemoryMap.set(addr, this);
  } else if (strcmp(key, "label")==0) {
    label(db.currentValue());
  } else {
    return AlDatabaseObject::read(db);
  }
  return true;
}


void AlData::write(AlDatabase &db, bool block)
{
  if (block) db.writeBlockBegin("Data");
  AlDatabaseObject::write(db, false);
  db.writeHex("addr", address());
  if (label()) db.write("label", label());
  if (block) db.writeBlockEnd();
}


unsigned int AlData::address()
{
  return pAddress;
}


void AlData::address(unsigned int addr)
{
  pAddress = addr;
}


unsigned int AlData::size()
{
  return pSize;
}


void AlData::size(unsigned int sz)
{
  pSize = sz;
}


const char *AlData::label() 
{
  return pLabel;
}


void AlData::label(const char *l)
{
  if (pLabel)
    free(pLabel);
  if (l)
    pLabel = strdup(l);
  else
    pLabel = 0L;
}


void AlData::exportAsm(FILE *f)
{
  if (pLabel) fprintf(f, "\t@ label = '%s'\n", pLabel);
  if (pSize) fprintf(f, "\t@ size = %d bytes\n", pSize);
}


void AlData::exportCppCall(FILE *f)
{
  fprintf(f, "EXPORT AlData\n");
  if (pLabel) fprintf(f, "\t@ label = '%s'\n", pLabel);
  if (pSize) fprintf(f, "\t@ size = %d bytes\n", pSize);
}


void AlData::exportCppCallEnd(FILE *f)
{
}


const char* AlData::stringCppCall()
{
  static char buf[1024];
  sprintf(buf, "%s(...)", pLabel);
  return buf;
}
