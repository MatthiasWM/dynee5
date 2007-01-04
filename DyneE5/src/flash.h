
#ifndef DN_FLASH_H
#define DN_FLASH_H

#include <stdio.h>

class Dn_Flash {
public:
  Dn_Flash();
  virtual ~Dn_Flash();
  virtual void write(unsigned int addr, unsigned short v) = 0;
  virtual unsigned short read(unsigned int addr) = 0;
  virtual unsigned short peek(unsigned int addr) = 0;
  virtual void read(FILE *f) = 0;
  virtual void write(FILE *f) = 0;
};

const int flashSize = 1024*1024*2; // 1MBit*16 = 2MByte

class Dn_Flash_28F016 : public Dn_Flash {
  int cycle;
  unsigned char state;
  unsigned char CSR, GSR, BSR[32];
  unsigned char WCH, WCL, WCswap;
  unsigned short mem[flashSize];
  unsigned short page[2][256];
  char pName[32];
public:
  Dn_Flash_28F016(const char *name);
  ~Dn_Flash_28F016();
  virtual void write(unsigned int addr, unsigned short v);
  virtual unsigned short read(unsigned int addr);
  virtual unsigned short peek(unsigned int addr);
  virtual void read(FILE *f);
  virtual void write(FILE *f);
};

#endif
