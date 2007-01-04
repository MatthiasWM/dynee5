

#include "screen.h"
#include "memory.h"
#include "main.h"

#include <FL/Fl_Image.H>
#include <FL/Fl.H>
#include <FL/fl_draw.H>

#include "cpu.h"


Dn_Screen::Dn_Screen(int x, int y, int w, int h, const char *l) 
: Fl_Box(x, y, w, h, l)
{
  pImage = 0L;
}

int Dn_Screen::handle(int e) {
  switch (e) {
    case FL_PUSH:
    case FL_DRAG:
    case FL_RELEASE:
      {
        int xp = Fl::event_x()-x()-6;
        int yp = Fl::event_y()-y()-24;
        //mem->set32(0xc104e9c+8, (xp<<23)|(yp<<9), 3);
        ARMul_WriteWord(cpu->state, 0xc104e9c+8, (xp<<21)|(yp<<7)|(0));
        if (e==FL_RELEASE) 
          ARMul_WriteByte(cpu->state, 0xc104e9c+12, 10);
        else 
          ARMul_WriteByte(cpu->state, 0xc104e9c+12, 0);
        ARMul_WriteByte(cpu->state, 0xc104e9c+4, 1);
      }
      return 1;
  }
  return Fl_Box::handle(e);
}

void Dn_Screen::draw() {
  draw_box();
  draw_label();
  if (pImage) {
    pImage->draw(x()+6, y()+24);
  } else {
    fl_color(FL_BLACK);
    fl_rect(x()+5, y()+25, 320, 480);
  }
}

/*
int Dn_Screen::handle(int ev) {
  return 0;
}
*/
void Dn_Screen::set_content(Fl_RGB_Image *img) {
  pImage = img;
  redraw();
}

