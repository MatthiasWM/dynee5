

#include "registerview.h"
#include "cpu.h"
#include "memory.h"

#include <FL/fl_draw.H>
#include <FL/Fl.H>

#include <stdlib.h>
#include <stdio.h>


/** 
 * Create assembler view of a memory segment.
 */
Dn_Register_View::Dn_Register_View(int x, int y, int w, int h, const char *l) 
: Fl_Box(x, y, w, h, l),
  pCpu(0L)
{
}


/** 
 * Draw the current memory segment.
 *
 * Columns are: address opcode operands annotation
 */
void Dn_Register_View::draw() {
  char buf[256];
  int i;

  draw_box();
  draw_label();
  if (!pCpu) 
    return;

  fl_font(FL_COURIER, 12);
  fl_color(FL_BLACK);
  int ch = fl_height();
  int ww = (w()-Fl::box_dw(box())-4)/fl_width('W');
  int hh = (h()-Fl::box_dh(box())-4)/ch;
  int xx = x()+Fl::box_dx(box())+2;
  int yy = y()+Fl::box_dy(box())+2 + ch - fl_descent();

  int n = pCpu->get_num_registers();
  for (i=0; i<n; i++) {
    pCpu->print_register(buf, i);
    fl_draw(buf, xx, yy);
    yy += ch; 
  }
  pCpu->print_flags(buf); fl_draw(buf, xx, yy); yy += ch;
  yy += 4;
  sprintf(buf, "TMR=%08x", cGPIO->get_tmr()); fl_draw(buf, xx, yy); yy += ch;
  sprintf(buf, "TM0=%08x", cGPIO->get_tm0()); fl_draw(buf, xx, yy); yy += ch;
  sprintf(buf, "TM1=%08x", cGPIO->get_tm1()); fl_draw(buf, xx, yy); yy += ch;
  sprintf(buf, "TM2=%08x", cGPIO->get_tm2()); fl_draw(buf, xx, yy); yy += ch;
  sprintf(buf, "TM3=%08x", cGPIO->get_tm3()); fl_draw(buf, xx, yy); yy += ch;
  sprintf(buf, "RTC=%08x", cGPIO->get32(0x0f181000, DN_MEM_PEEK));  fl_draw(buf, xx, yy); yy += ch;
  sprintf(buf, "Alm=%08x", cGPIO->get32(0x0f181400, DN_MEM_PEEK));  fl_draw(buf, xx, yy); yy += ch;
  sprintf(buf, "IR =%08x", cGPIO->get32(0x0f183000, DN_MEM_PEEK));  fl_draw(buf, xx, yy); yy += ch;
  sprintf(buf, "ICR=%08x", cGPIO->get32(0X0f183400, DN_MEM_PEEK));  fl_draw(buf, xx, yy); yy += ch;
  sprintf(buf, "FM =%08x", cGPIO->get32(0x0f183c00, DN_MEM_PEEK));  fl_draw(buf, xx, yy); yy += ch;
  sprintf(buf, "IC1=%08x", cGPIO->get32(0x0f184000, DN_MEM_PEEK));  fl_draw(buf, xx, yy); yy += ch;
  sprintf(buf, "IC2=%08x", cGPIO->get32(0x0f184400, DN_MEM_PEEK));  fl_draw(buf, xx, yy); yy += ch;
  sprintf(buf, "IC3=%08x", cGPIO->get32(0x0f184800, DN_MEM_PEEK));  fl_draw(buf, xx, yy); yy += ch;
}

/**
 * Link this view to a cpu
 */
void Dn_Register_View::cpu(Dn_CPU *c) {
  if (pCpu!=c) {
    pCpu = c;
    redraw();
  }
}




