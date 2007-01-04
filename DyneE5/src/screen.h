
#ifndef DN_SCREEN_H
#define DN_SCREEN_H

#include <FL/Fl_Box.H>

class Dn_Screen : public Fl_Box {
  class Fl_RGB_Image *pImage;
public:
  Dn_Screen(int x, int y, int w, int h, const char *l);
  void draw();
  int handle(int);
  void set_content(class Fl_RGB_Image*);
};

#endif

