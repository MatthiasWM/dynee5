
#ifndef DN_HEX_VIEW_H
#define DN_HEX_VIEW_H

#include <FL/Fl_Box.H>


class Dn_Memory;


class Dn_Hex_View : public Fl_Box {
  Dn_Memory *pMem;
  unsigned int pAddr;
protected:
  void draw();
public:
  Dn_Hex_View(int x, int y, int w, int h, const char *l=0);
  void memory(Dn_Memory*);
  void set_base(unsigned int addr);
  void set_base(const char *addr);
};

#endif

