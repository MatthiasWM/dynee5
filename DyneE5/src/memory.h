

#ifndef DN_MEMORY_H
#define DN_MEMORY_H

#include "flash.h"

#ifdef WIN32
# pragma warning ( disable : 4786 )
#endif

#include <map>

// set this if you want to write into the ROM and not cause an exception
#define DN_MEM_PATCH 0x01
// set this if you want to read non-existing memory without causing an exception
#define DN_MEM_PEEK  0x02
// set this, if you want to read patched memory instead of original memory
#define DN_MEM_INSTR 0x04
// set this if the given address is a virtual address, which must still
// be converted to a physical address
#define DN_MEM_VIRT  0x08


extern class Dn_GPIO_Chunk *cGPIO;
extern class Dn_Flash_Chunk *cFlash;
extern class Dn_Memory_Chunk *cRAM;

const unsigned int GPIO_TMR  = 0x0f181800;
const unsigned int GPIO_TM0  = 0x0f182000;
const unsigned int GPIO_TM1  = 0x0f182400;
const unsigned int GPIO_TM2  = 0x0f182800;
const unsigned int GPIO_TM3  = 0x0f182c00;
const unsigned int GPIO_IR   = 0x0f183000;
const unsigned int GPIO_IACK = 0x0f183800;


const int delta_t = 1;  // ticks per command (assuming 200ns?)
const int delta_s = 5000000; // ticks per second (that would be 200e-9, 5 Mio.)


class Dn_Memory_Chunk {
protected:
  unsigned int pStart;
  unsigned int pSize;
  unsigned char *pData;
public:
  Dn_Memory_Chunk();
  Dn_Memory_Chunk(unsigned int start, unsigned int size);
  virtual ~Dn_Memory_Chunk();
  virtual char has(unsigned int addr);

  virtual unsigned char  get8 (unsigned int addr, char flags);
  virtual unsigned short get16(unsigned int addr, char flags);
  virtual unsigned int   get32(unsigned int addr, char flags);

  virtual void set8 (unsigned int addr, unsigned char v,  char flags);
  virtual void set16(unsigned int addr, unsigned short v, char flags);
  virtual void set32(unsigned int addr, unsigned int v,   char flags);

  virtual char load(const char *filename, unsigned int offset=0);
  virtual char save(const char *filename);
  virtual void read(FILE *f);
  virtual void write(FILE *f);
};


class Dn_ROM_Chunk : public Dn_Memory_Chunk {
  unsigned char *pPatchedData;
public:
  Dn_ROM_Chunk(unsigned int start, unsigned int size);
  ~Dn_ROM_Chunk();

  virtual void set8 (unsigned int addr, unsigned char v,  char flags);
  virtual void set16(unsigned int addr, unsigned short v, char flags);
  virtual void set32(unsigned int addr, unsigned int v,   char flags);

  virtual unsigned char  get8 (unsigned int addr, char flags);
  virtual unsigned short get16(unsigned int addr, char flags);
  virtual unsigned int   get32(unsigned int addr, char flags);

  virtual char load(const char *filename, unsigned int offset=0);
};


class Dn_GPIO_Chunk : public Dn_Memory_Chunk {
  typedef std::map < unsigned int, unsigned int > Dn_GPIO_Access;
  Dn_GPIO_Access access;
  void add_read(unsigned int a);
  void add_write(unsigned int a);
public:
  Dn_GPIO_Chunk(unsigned int start, unsigned int size);

  virtual unsigned char  get8 (unsigned int addr, char flags);
  virtual unsigned short get16(unsigned int addr, char flags);
  virtual unsigned int   get32(unsigned int addr, char flags);

  virtual void set8 (unsigned int addr, unsigned char v,  char flags);
  virtual void set16(unsigned int addr, unsigned short v, char flags);
  virtual void set32(unsigned int addr, unsigned int v,   char flags);

  virtual char save(const char *filename);

  void update_cycle();
  unsigned int get_tmr();
  unsigned int get_tm0();
  unsigned int get_tm1();
  unsigned int get_tm2();
  unsigned int get_tm3();
  unsigned int get_ir();
};


class Dn_Flash_Chunk : public Dn_Memory_Chunk {
  Dn_Flash *chip_lo;
  Dn_Flash *chip_hi;
public:
  Dn_Flash_Chunk(unsigned int start, unsigned int size);
  ~Dn_Flash_Chunk();

  virtual unsigned char  get8 (unsigned int addr, char flags);
  virtual unsigned short get16(unsigned int addr, char flags);
  virtual unsigned int   get32(unsigned int addr, char flags);

  virtual void set8 (unsigned int addr, unsigned char v,  char flags);
  virtual void set16(unsigned int addr, unsigned short v, char flags);
  virtual void set32(unsigned int addr, unsigned int v,   char flags);

  virtual char load(const char *filename, unsigned int offset=0);
  virtual char save(const char *filename);
  virtual void read(FILE *f);
  virtual void write(FILE *f);
};

class Dn_Screen_Chunk : public Dn_Memory_Chunk {
  class Fl_RGB_Image *pImage;
  unsigned char *pBitmap;
  unsigned char *base(unsigned int addr, unsigned int data);
  void redraw();
public:
  Dn_Screen_Chunk(unsigned int start, unsigned int size);

  virtual void set8 (unsigned int addr, unsigned char v,  char flags);
  virtual void set16(unsigned int addr, unsigned short v, char flags);
  virtual void set32(unsigned int addr, unsigned int v,   char flags);

  class Fl_RGB_Image *image() { return pImage; }
};


class Dn_Memory {
  Dn_Memory_Chunk *pChunk[10];
  int pNChunk;
  void add(Dn_Memory_Chunk*);
  Dn_Memory_Chunk *find(unsigned int addr);
  char pInstr;
  void fixup(unsigned int &addr, char &flags);
public:
  Dn_Memory();
  unsigned int v2p(unsigned int addr, char peek=0);

  unsigned char  get8 (unsigned int addr, char flags=0);
  unsigned short get16(unsigned int addr, char flags=0);
  unsigned int   get32(unsigned int addr, char flags=0);

  void set8 (unsigned int addr, unsigned char v,  char flags=0);
  void set16(unsigned int addr, unsigned short v, char flags=0);
  void set32(unsigned int addr, unsigned int v,   char flags=0);

  void read(FILE *f);
  void write(FILE *f);

  void begin_instr_fetch();
  void end_instr_fetch();

  Dn_Memory_Chunk *add_chunk(unsigned int start, unsigned int size);
  void add_chunk(Dn_Memory_Chunk *c);
};


#endif

