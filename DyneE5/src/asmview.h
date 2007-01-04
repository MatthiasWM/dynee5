
#ifndef DN_ASM_VIEW_H
#define DN_ASM_VIEW_H

#include <FL/Fl_Box.H>
#include <stdio.h>

class Dn_CPU;


class Dn_Asm_View : public Fl_Box {
  Dn_CPU *pCpu;
  unsigned int pAddr;
  unsigned int pOffset;
  unsigned int find_addr(int, int);
  unsigned int pMarkA, pMarkB;
  char pTraceback;
protected:
  void draw();
  int handle(int);
public:
  Dn_Asm_View(int x, int y, int w, int h, const char *l=0);
  void cpu(Dn_CPU*);
  void base(unsigned int addr);
  void save_marked(const char *filename);
  void save_marked_cc(const char *filename);
  void save_range(FILE *f, unsigned int start, unsigned int end);
  void save_range_cc(FILE *f, unsigned int start, unsigned int end);
  void set_traceback() { pTraceback = 1; pAddr = 0; pOffset = -15; redraw(); }
};

#endif

