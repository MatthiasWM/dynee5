

#include "hexview.h"
#include "memory.h"

#include <FL/fl_draw.H>
#include <FL/Fl.H>

#include <stdlib.h>
#include <stdio.h>


/** 
 * Create a hexadeciaml table view of a memory segment.
 *
 * This view has three columns: in the first column is the memory address
 * in hex, followed by several bytes of hex data, followed by the ASCII
 * text for the same values. To the right of this table may be a scroll
 * bar.
 */
Dn_Hex_View::Dn_Hex_View(int x, int y, int w, int h, const char *l) 
: Fl_Box(x, y, w, h, l),
  pMem(0L),
  pAddr(0)
{
}


/** 
 * Draw the current memory segment.
 */
void Dn_Hex_View::draw() {
  char buf[256];
  unsigned int addr = pAddr;
  int i, j;

  draw_box();
  draw_label();
  if (!pMem) 
    return;

  fl_font(FL_COURIER, 12);
  fl_color(FL_BLACK);
  int ch = fl_height();
  int ww = (w()-Fl::box_dw(box())-4)/fl_width('W');
  int hh = (h()-Fl::box_dh(box())-4)/ch;
  int xx = x()+Fl::box_dx(box())+2;
  int yy = y()+Fl::box_dy(box())+2 + ch - fl_descent();
  int nw = (ww-9)/4;
  int nh = hh;
  for (j=0; j<nh; j++) {
    char *d = buf;
    d += sprintf(d, "%08x ", addr);
    for (i=0; i<nw; i++) {
      uchar c = pMem->get8(addr+i, DN_MEM_PEEK|DN_MEM_VIRT);
      d += sprintf(d, "%02x ", c);
    }
    for (i=0; i<nw; i++) {
      uchar c = pMem->get8(addr+i, DN_MEM_PEEK|DN_MEM_VIRT);
      if (c<32||c>=127) c = '.';
      *d++ = c;
    }
    *d = 0;
    fl_draw(buf, xx, yy);
    yy += ch; 
    addr += nw;
  }
}

/**
 * Link this view to a memory chunk.
 */
void Dn_Hex_View::memory(Dn_Memory *m) {
  if (pMem!=m) {
    pMem = m;
    redraw();
  }
}

/**
 * Change the base address that is shown at the top of the view.
 */
void Dn_Hex_View::set_base(unsigned int a) {
  if (pAddr!=a) {
    pAddr = a;
    redraw();
  }
}

void Dn_Hex_View::set_base(const char *a) {
  unsigned int addr = 0;
  sscanf(a, "%x", &addr);
  set_base(addr);
}


