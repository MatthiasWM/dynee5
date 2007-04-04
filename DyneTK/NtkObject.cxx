/**
 * This file is obsolete with the introduction of NEWT/0
 */


#include "NtkObject.h"
#include "NtkPipe.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>



NtkObject::NtkObject()
{
}


NtkObject::~NtkObject() 
{
}


void NtkObject::print() 
{
  printf("<unprintable>");
}


void NtkObject::print(NtkFileWritePipe &pipe, int flags)
{
  if ((flags&PRINT_NO_INDENT)==0) pipe.printf("%s", pipe.indent());
  pipe.printf(";; unprintable object");
  if ((flags&PRINT_NO_EOL)==0) pipe.printf("\n");
}


NtkObjectBinary::NtkObjectBinary()
: nData_(0),
  data_(0L),
  Class_(0L)
{
}


NtkObjectBinary::~NtkObjectBinary()
{
  // FIXME
}


void NtkObjectBinary::keepData(void *data, int size)
{
  data_ = data;
  nData_ = size;
}


void NtkObjectBinary::setClass(NtkObject *obj)
{
  Class_ = obj;
}


void NtkObjectBinary::print(NtkFileWritePipe &pipe, int flags)
{
  if ((flags&PRINT_NO_INDENT)==0) pipe.printf("%s", pipe.indent());
  pipe.incrIndent();
  if (Class_) {
    pipe.printf("begin_code ");
    Class_->print(pipe, PRINT_NO_INDENT);
  } else
    pipe.printf("begin_code\n");
  unsigned char *src = (unsigned char*)data_;
  for (int i=0; i<nData_; i++) {
    unsigned char d = *src++, a = (d&0xf8)>>3, b = (d&0x07);
    switch (a) {
      case 000: 
        switch (b) {
          case 002: pipe.printf("%sret\n", pipe.indent()); break;
          default: pipe.printf("%sdb 0x%02x ;; %03o\n", pipe.indent(), d, d);
        }
        break;
      case 003: pipe.printf("%spush %d\n", pipe.indent(), b); break;
      case 005: pipe.printf("%scall %d\n", pipe.indent(), b); break;
      default: pipe.printf("%sdb 0x%02x ;; %03o\n", pipe.indent(), d, d);
    }
  }
  pipe.printf("%send_code", pipe.decrIndent());
  if ((flags&PRINT_NO_EOL)==0) pipe.printf("\n");
}

  
NtkObjectArray::NtkObjectArray() 
: nSlot_(0),
  NSlot_(0),
  slot_(0L)
{
}


NtkObjectArray::~NtkObjectArray()
{
  // FIXME: implement me
}


void NtkObjectArray::setNumSlots(int n)
{
  /* FIXME: if there are already slots allocated, be nice */
  if (NSlot_)
    throw("previously allocated slots not supported yet");
  nSlot_ = n;
  NSlot_ = n;
  slot_ = (NtkObject**)calloc(n, sizeof(NtkObject*));
}

void NtkObjectArray::setSlot(int n, NtkObject *obj)
{
  // FIXME: range check
  // FIXME: deallocate prevoius object
  slot_[n] = obj;
}

void NtkObjectArray::setClass(NtkObject *obj)
{
  Class_ = obj;
}


void NtkObjectArray::print(NtkFileWritePipe &pipe, int flags)
{
  if ((flags&PRINT_NO_INDENT)==0) pipe.printf("%s", pipe.indent());
  pipe.incrIndent();
  pipe.printf("begin_array\n");
  for (int i=0; i<nSlot_; i++) {
    if (slot_[i]) {
      slot_[i]->print(pipe, PRINT_NO_EOL);
      pipe.printf(" ;; %d\n", i);
    } else {
      pipe.printf("%s;; missing Slot in Array\n", pipe.indent());
    }
  }
  pipe.printf("%send_array", pipe.decrIndent());
  if ((flags&PRINT_NO_EOL)==0) pipe.printf("\n");
}


NtkObjectFrame::NtkObjectFrame() 
: nSlot_(0),
  NSlot_(0),
  slot_(0L),
  tag_(0L)
{
}


NtkObjectFrame::~NtkObjectFrame()
{
  // FIXME: implement me
}


void NtkObjectFrame::setNumSlots(int n)
{
  /* FIXME: if there are already slots allocated, be nice */
  if (NSlot_)
    throw("previously allocated slots not supported yet");
  nSlot_ = n;
  NSlot_ = n;
  slot_ = (NtkObject**)calloc(n, sizeof(NtkObject*));
  tag_  = (NtkObject**)calloc(n, sizeof(NtkObject*));
}

void NtkObjectFrame::setSlot(int n, NtkObject *obj)
{
  // FIXME: range check
  // FIXME: deallocate prevoius object
  slot_[n] = obj;
}


void NtkObjectFrame::setTag(int n, NtkObject *obj)
{
  // FIXME: range check
  // FIXME: deallocate prevoius object
  tag_[n] = obj;
}


NtkObject *NtkObjectFrame::getTag(int n)
{
  return tag_[n];
}


void NtkObjectFrame::print(NtkFileWritePipe &pipe, int flags)
{
  if ((flags&PRINT_NO_INDENT)==0) pipe.printf("%s", pipe.indent());
  pipe.incrIndent();
  pipe.printf("begin_frame\n");
  for (int i=0; i<nSlot_; i++) {
    pipe.printf("$");
    if (tag_[i]) 
      tag_[i]->print(pipe, PRINT_NO_INDENT);
    else
      pipe.printf(";; missing Tag in Frame\n");
    if (slot_[i]) 
      slot_[i]->print(pipe);
    else
      pipe.printf("%s;; missing Slot in Frame\n", pipe.indent());
  }
  pipe.printf("%send_frame", pipe.decrIndent());
  if ((flags&PRINT_NO_EOL)==0) pipe.printf("\n");
}


NtkObjectSymbol::NtkObjectSymbol() 
: nSym_(0),
  sym_(0L)
{
}


NtkObjectSymbol::~NtkObjectSymbol()
{
  if (sym_) 
    free(sym_);
}


void NtkObjectSymbol::setSymbol(const char *sym, int len) 
{
  if (sym_) {
    free(sym_);
    sym_ = 0L;
  }
  nSym_ = 0;
  if (sym && len) {
    nSym_ = len;
    sym_ = (char*)malloc(len+1);
    memcpy(sym_, sym, len);
    sym_[len] = 0;
  }
}


void NtkObjectSymbol::print() 
{
  if (sym_)
    printf("%s", sym_);
  else 
    printf("<null>");
}


void NtkObjectSymbol::print(NtkFileWritePipe &pipe, int flags)
{
  if ((flags&PRINT_NO_INDENT)==0) pipe.printf("%s", pipe.indent());
  if (sym_)
    pipe.printf("'%s", sym_);
  else
    pipe.printf("0");
  if ((flags&PRINT_NO_EOL)==0) pipe.printf("\n");
}


NtkObjectString::NtkObjectString() 
: nStr_(0),
  str_(0L)
{
}


NtkObjectString::~NtkObjectString()
{
  if (str_) 
    free(str_);
}


void NtkObjectString::setString(const unsigned short *str, int len) 
{
  if (str_) {
    free(str_);
    str_ = 0L;
  }
  nStr_ = 0;
  if (str && len) {
    nStr_ = len;
    str_ = (unsigned short*)malloc((len+1)*sizeof(short));
    memcpy(str_, str, len*sizeof(short));
    str_[len] = 0;
  }
}

void NtkObjectString::print(NtkFileWritePipe &pipe, int flags)
{
  if ((flags&PRINT_NO_INDENT)==0) pipe.printf("%s", pipe.indent());
  pipe.printf("\"");
  for (int i=0; i<nStr_; i++) {
    unsigned short c = str_[i];
    if (c>=32 && c<=255 && c!=127) {
      pipe.printf("%c", c);
    } else if (c=='\n') {
      pipe.printf("\\n");
    } else if (c=='\r') {
      pipe.printf("\\r");
    } else if (c=='\t') {
      pipe.printf("\\t");
    } else if (c<=255) {
      if (c==0 && i==nStr_-1) continue;
      pipe.printf("\\%03o", c);
    } else {
      pipe.printf("\\u%04x", c);
    }
  }
  pipe.printf("\"");
  if ((flags&PRINT_NO_EOL)==0) pipe.printf("\n");
}



NtkObjectBool::NtkObjectBool() 
: val_(0)
{
}


NtkObjectBool::~NtkObjectBool()
{
}


void NtkObjectBool::set(char val) 
{
  val_ = val;
}


void NtkObjectBool::print(NtkFileWritePipe &pipe, int flags)
{
  if ((flags&PRINT_NO_INDENT)==0) pipe.printf("%s", pipe.indent());
  pipe.printf("%s", val_ ? "true" : "false");
  if ((flags&PRINT_NO_EOL)==0) pipe.printf("\n");
}


NtkObjectInt::NtkObjectInt() 
: val_(0)
{
}


NtkObjectInt::~NtkObjectInt()
{
}


void NtkObjectInt::set(int val) 
{
  val_ = val;
}


void NtkObjectInt::print(NtkFileWritePipe &pipe, int flags)
{
  if ((flags&PRINT_NO_INDENT)==0) pipe.printf("%s", pipe.indent());
  pipe.printf("%d", val_);
  if ((flags&PRINT_NO_EOL)==0) pipe.printf("\n");
}


