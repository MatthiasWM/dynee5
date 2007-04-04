/**
 * This file is obsolete with the introduction of NEWT/0
 */

#ifndef NTK_OBJECT
#define NTK_OBJECT

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include <stdio.h>


class NtkFileWritePipe;


static const int PRINT_NO_EOL = 1;
static const int PRINT_NO_INDENT = 2;

class NtkObject
{
public:
  NtkObject();
  virtual ~NtkObject();
  virtual void print();
  virtual void print(NtkFileWritePipe&, int flags=0);
};


class NtkObjectBinary : public NtkObject
{
  int nData_;
  void *data_;
  NtkObject *Class_;

public:
  NtkObjectBinary();
  virtual ~NtkObjectBinary();
  void keepData(void *data, int size);
  void setClass(NtkObject *obj);
  virtual void print(NtkFileWritePipe&, int flags=0);
};


class NtkObjectArray : public NtkObject
{
  int nSlot_;
  int NSlot_;
  NtkObject **slot_;
  NtkObject *Class_;

public:
  NtkObjectArray();
  virtual ~NtkObjectArray();
  void setNumSlots(int n);
  void setSlot(int n, NtkObject *obj);
  void setClass(NtkObject *obj);
  virtual void print(NtkFileWritePipe&, int flags=0);
};


class NtkObjectFrame : public NtkObject
{
  int nSlot_;
  int NSlot_;
  NtkObject **slot_;
  NtkObject **tag_;

public:
  NtkObjectFrame();
  virtual ~NtkObjectFrame();
  void setNumSlots(int n);
  void setSlot(int n, NtkObject *obj);
  void setTag(int n, NtkObject *obj);
  NtkObject *getTag(int n);
  virtual void print(NtkFileWritePipe&, int flags=0);
};


class NtkObjectSymbol : public NtkObject
{
  int nSym_;
  char *sym_;

public:
  NtkObjectSymbol();
  ~NtkObjectSymbol();
  void setSymbol(const char *sym, int len);
  void print();
  virtual void print(NtkFileWritePipe&, int flags=0);
};


class NtkObjectString : public NtkObject
{
  int nStr_;
  unsigned short *str_;

public:
  NtkObjectString();
  ~NtkObjectString();
  void setString(const unsigned short *sym, int len);
  virtual void print(NtkFileWritePipe&, int flags=0);
};


class NtkObjectBool : public NtkObject
{
  char val_;

public:
  NtkObjectBool();
  ~NtkObjectBool();
  void set(char val);
  virtual void print(NtkFileWritePipe&, int flags=0);
};


class NtkObjectInt : public NtkObject
{
  int val_;

public:
  NtkObjectInt();
  ~NtkObjectInt();
  void set(int val);
  virtual void print(NtkFileWritePipe&, int flags=0);
};


#endif

