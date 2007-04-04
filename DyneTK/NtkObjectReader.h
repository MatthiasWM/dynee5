/**
 * This file is obsolete with the introduction of NEWT/0
 */

#ifndef NTK_OBJECT_READER
#define NTK_OBJECT_READER

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include "NtkObject.h"
#include <stdio.h>


class NtkPipe;


class NtkObjectReader 
{
  NtkPipe &pipe_;
  int indent_;
  int nPrec_;
  int NPrec_;
  NtkObject **prec_;

  void indent();
  void addPrecedent(NtkObject*);
  void resetPrecedents();

public:
  NtkObjectReader(NtkPipe &pipe);
  ~NtkObjectReader();
  NtkObject *read();
  NtkObject *scan();
  NtkObject *readImmediate();
  NtkObjectBinary *readBinary();
  NtkObjectArray *readArray();
  NtkObjectArray *plainArray();
  NtkObjectFrame *readFrame();
  NtkObjectSymbol *readSymbol();
  NtkObjectString *readString();
  NtkObjectBool *readNIL();
  NtkObject *getPrecedent(int n);
  int readXLong();
};


#endif

