/**
 * This file is obsolete with the introduction of NEWT/0
 */


#include "NtkObjectReader.h"
#include "NtkPipe.h"

#include <stdlib.h>
#include <string.h>


#define DEBUG


NtkObjectReader::NtkObjectReader(NtkPipe &pipe) 
: pipe_(pipe),
  indent_(0),
  nPrec_(0),
  NPrec_(0),
  prec_(0L)
{
  if (!pipe_.isOpen())
    throw("Can't open source");
}



NtkObjectReader::~NtkObjectReader()
{
  if (prec_)
    free(prec_);
}



NtkObject *NtkObjectReader::read()
{
  unsigned char c = pipe_.getc();
  if (c!=2) {
    throw("Unknown file format");
  }
  NtkObject *root = scan();
  /*
  int curr = ftell(file_);
  fseek(file_, 0, SEEK_END);
  int last = ftell(file_);
  fseek(file_, curr, SEEK_SET);
  printf("Read %d of %d bytes\n", curr, last);
  */
  return root;
}


NtkObject *NtkObjectReader::scan() 
{
  NtkObject *obj = 0L;
  unsigned char c = pipe_.getc();
  switch (c) {
  case 0:
    obj = readImmediate();
    break;
//case 1: character
//case 2: uniChar
  case 3:
    obj = readBinary();
    break;
  case 4:
    obj = readArray();
    break;
  case 5: 
	obj = plainArray();
	break;
  case 6:
    obj = readFrame();
    break;
  case 7:
    obj = readSymbol();
    break;
  case 8:
    obj = readString();
    break;
  case 9:
    obj = getPrecedent(readXLong());
# ifdef DEBUG
    indent(); printf("(using precedent) \""); obj->print(); printf("\"\n");
# endif
    break;
  case 10:
    obj = readNIL();
    break;
//case 11: smallRect
//case 12: largeBinary
  default:
    throw("unknown code in file");
    break;
  }
  return obj;
}


NtkObject *NtkObjectReader::readImmediate()
{
  NtkObject *o = 0L;
  int n = readXLong();
  if ((n&0x03)==0) {
    NtkObjectInt *ob = new NtkObjectInt();
    ob->set(n>>2);
    o = ob;
# ifdef DEBUG
    indent(); printf("Integer: %d\n", n>>2);
# endif
  } else if ((n&0x0f)==6) {
    printf("character\n");
  } else if (n==0x1a) {
    NtkObjectBool *ob = new NtkObjectBool();
    ob->set(1);
    o = ob;
# ifdef DEBUG
    indent(); printf("True\n");
# endif
  } else if (n==2) {
    NtkObjectBool *ob = new NtkObjectBool();
    ob->set(0);
    o = ob;
# ifdef DEBUG
    indent(); printf("False\n");
# endif
  } else if ((n&0x03)==3) {
    printf("magic\n");
  } else {
# ifdef DEBUG
    indent(); printf("Undefined Immediate: 0x%08x at 0x%08x\n", n, pipe_.getPos());
# endif
  }
  return o;
}


NtkObjectBinary *NtkObjectReader::readBinary()
{
  NtkObjectBinary *o = new NtkObjectBinary();
  addPrecedent(o);
  int n = readXLong();
  NtkObject *Class = scan();
  o->setClass(Class);
  if (n) {
    void *data = malloc(n);
    pipe_.read(data, n);
    o->keepData(data, n);
# ifdef DEBUG
    indent(); printf("Data: %d bytes\n", n);
    indent();
    int i, nn = n; if (nn>32) nn = 32;
    for (i=0; i<nn; i++)
      printf("%03o ", ((unsigned char*)data)[i]);
    for (i=0; i<nn; i++) {
      unsigned char c = ((unsigned char*)data)[i];
      printf("%c", (c>31&&c!=127)?c:'.');
    }
    if (n>nn) printf(" etc.");
    printf("\n");
# endif
  }
  return o;
}


NtkObjectArray *NtkObjectReader::readArray()
{
  NtkObjectArray *o = new NtkObjectArray();
  addPrecedent(o);
  int i, n = readXLong();
  o->setNumSlots(n);
# ifdef DEBUG
  indent(); printf("Array with %d slots\n", n); indent_++;
# endif
  NtkObject *Class = scan();
  o->setClass(Class);
  // read slots
  for (i=0; i<n; i++) {
#   ifdef DEBUG
    indent(); printf("-Slot %d\n", i); indent_++;
#   endif
    NtkObject *slot = scan();
    o->setSlot(i, slot);
# ifdef DEBUG
    indent_--;
# endif
  }
# ifdef DEBUG
  indent_--;
# endif
  return o;
}


NtkObjectArray *NtkObjectReader::plainArray()
{
  NtkObjectArray *o = new NtkObjectArray();
  addPrecedent(o);
  int i, n = readXLong();
  o->setNumSlots(n);
# ifdef DEBUG
  indent(); printf("Plain Array with %d slots\n", n); indent_++;
# endif
  // read slots
  for (i=0; i<n; i++) {
#   ifdef DEBUG
    indent(); printf("-Slot %d\n", i); indent_++;
#   endif
    NtkObject *slot = scan();
    o->setSlot(i, slot);
# ifdef DEBUG
    indent_--;
# endif
  }
# ifdef DEBUG
  indent_--;
# endif
  return o;
}


NtkObjectFrame *NtkObjectReader::readFrame()
{
  NtkObjectFrame *o = new NtkObjectFrame();
  addPrecedent(o);
  int i, n = readXLong();
  o->setNumSlots(n);
# ifdef DEBUG
  indent(); printf("Frame with %d slots\n", n); indent_++;
# endif
  // read tags
  for (i=0; i<n; i++) {
    NtkObject *tag = scan();
    //if (!tag->isSymbol()) ...
    o->setTag(i, tag);
  }
  // read slots
  for (i=0; i<n; i++) {
#   ifdef DEBUG
    indent(); printf("-Slot %d \"", i); indent_++;
    o->getTag(i)->print();
    printf("\"\n");
#   endif
    NtkObject *slot = scan();
    o->setSlot(i, slot);
# ifdef DEBUG
    indent_--;
# endif
  }
# ifdef DEBUG
  indent_--;
# endif
  return o;
}


NtkObjectSymbol *NtkObjectReader::readSymbol()
{
  NtkObjectSymbol *o = new NtkObjectSymbol();
  addPrecedent(o);
  int n = readXLong();
  if (n) {
    char *buf = (char*)malloc(n+1);
    pipe_.read(buf, n);
    buf[n] = 0;
    o->setSymbol(buf, n);
# ifdef DEBUG
    indent(); printf("Symbol: '%s'\n", buf);
# endif
    free(buf);
  }
  return o;
}


NtkObjectString *NtkObjectReader::readString()
{
  NtkObjectString *o = new NtkObjectString();
  addPrecedent(o);
  int i, n = readXLong();
  if (n) {
    n = n/2; // FIXME!
    unsigned short *buf = (unsigned short*)malloc((n+1)*sizeof(unsigned short));
    for (i=0; i<n; i++) {
      short c = pipe_.getc()<<8;
      c |= pipe_.getc();
      buf[i] = c;
    }
    buf[n] = 0;
    o->setString(buf, n);
# ifdef DEBUG
    indent(); printf("String: '");
    for (i=0; i<n; i++) {
      short c = buf[i];
      if (buf[i]<32 || buf[i]>255) putchar('.'); else putchar(buf[i]);
    }
    printf("'\n", buf);
# endif
    free(buf);
  }
  return o;
}


NtkObjectBool *NtkObjectReader::readNIL()
{
  NtkObjectBool *o = new NtkObjectBool();
  o->set(0);
# ifdef DEBUG
  indent(); printf("False\n");
# endif
  return o;
}


int NtkObjectReader::readXLong()
{
  unsigned char c = pipe_.getc();
  if (c!=255) 
    return (int)c;
  int n = pipe_.getc()<<24;
  n |= pipe_.getc()<<16;
  n |= pipe_.getc()<<8;
  n |= pipe_.getc();
  return n;
}


void NtkObjectReader::indent()
{
  int i;
  for (i=0; i<indent_; i++) {
    printf("  ");
  }
}


NtkObject *NtkObjectReader::getPrecedent(int n) 
{
  if (n<0 || n>=nPrec_) {
    throw("Illegal precedent reference");
  }
  return prec_[n];
}


void NtkObjectReader::addPrecedent(NtkObject *obj) 
{
  if (NPrec_ == nPrec_) {
    NPrec_ += 50;
    prec_ = (NtkObject**)realloc(prec_, NPrec_*sizeof(NtkObject*));
  }
  prec_[nPrec_] = obj;
  nPrec_++;
}


