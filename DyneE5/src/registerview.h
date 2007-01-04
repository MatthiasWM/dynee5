
#ifndef DN_REGISTER_VIEW_H
#define DN_REGISTER_VIEW_H

#include <FL/Fl_Box.H>


class Dn_CPU;


class Dn_Register_View : public Fl_Box {
  Dn_CPU *pCpu;
protected:
  void draw();
public:
  Dn_Register_View(int x, int y, int w, int h, const char *l=0);
  void cpu(Dn_CPU*);
};

#endif

