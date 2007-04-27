

#include "asmview.h"
#include "cpu.h"
#include "symbols.h"
#include "main.h"

#include <FL/fl_draw.H>
#include <FL/Fl.H>
#include <FL/Enumerations.H>

#include <stdlib.h>
#include <stdio.h>


/** 
 * Create assembler view of a memory segment.
 */
Dn_Asm_View::Dn_Asm_View(int x, int y, int w, int h, const char *l) 
: Fl_Box(x, y, w, h, l),
  pCpu(0L),
  pAddr(0),
  pOffset(0),
  pMarkA(0), pMarkB(0),
  pTraceback(0)
{
}


/** 
 * Draw the current memory segment.
 *
 * Columns are: address opcode operands annotation
 */
void Dn_Asm_View::draw() {
  char buf[2048];
  unsigned int addr = pAddr + pOffset;
  if (addr>4) addr-=4; // show the previous two lines of code
  if (addr>4) addr-=4;
  int i, j;

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
  int nh = hh;
  int tba = pOffset+pAddr + NTraceback;
  for (j=0; j<nh; j++) {
    if (pTraceback) {
      addr = pCpu->traceback[tba%NTraceback];
    }
    const char *sym = symbols->get(addr);
    if (sym) {
      fl_color(FL_BLUE);
      fl_font(FL_HELVETICA_ITALIC, 11);
      fl_draw(sym, xx, yy);
      fl_font(FL_COURIER, 12);
      fl_color(FL_BLACK);
      yy += ch; j++;
      if (j>=nh) continue;
    }
    const char *cmt = symbols->get_comment(addr);
    if (cmt) {
      fl_color(0x3333ff00);
      fl_font(FL_HELVETICA, 11);
      fl_draw("//", xx+20, yy);
      fl_draw(cmt, xx+30, yy);
      fl_font(FL_COURIER, 12);
      fl_color(FL_BLACK);
      yy += ch; j++;
      if (j>=nh) continue;
    }
    char *d = buf;
    char bp = ':';
    if (symbols->is_breakpoint(addr)) {
      bp = '*';
    }
    if (symbols->is_tmp_breakpoint(addr)) {
      bp = (bp=='*') ? 'T' : 't';
    }
    d += sprintf(d, "%08x %c ", addr, bp);
    if (    (pTraceback && ((tba%NTraceback)==pCpu->iTraceback-1)) 
         || (!pTraceback && (addr==pAddr))) {
      fl_color(FL_LIGHT3);
      fl_rectf(xx, yy+fl_descent()-fl_height(), w()-Fl::box_dw(box())-4, fl_height());
      fl_color(FL_BLACK);
    }
    if (addr>=pMarkA && addr<=pMarkB) {
      fl_color(FL_YELLOW);
      fl_rectf(xx, yy+fl_descent()-fl_height(), 9, fl_height());
      fl_color(FL_BLACK);
    }
    addr += pCpu->dis(d, addr);
    fl_draw(buf, xx+10, yy);
    yy += ch; 
    tba++;
  }
}

/**
 * Link this view to a memory chunk.
 */
void Dn_Asm_View::cpu(Dn_CPU *c) {
  if (pCpu!=c) {
    pCpu = c;
    redraw();
  }
}

/**
 * Change the base address that is shown at the top of the view.
 */
void Dn_Asm_View::base(unsigned int a) {
  if (pAddr!=a) {
    pAddr = a;
    redraw();
  }
}

/**
 * Find the memory address displayed at a screen position
 */
unsigned int Dn_Asm_View::find_addr(int mx, int my) {
  unsigned int addr = pAddr + pOffset;
  if (addr>4) addr-=4; // show the previous two lines of code
  if (addr>4) addr-=4;
  fl_font(FL_COURIER, 12);
  fl_color(FL_BLACK);
  int ch = fl_height();
  int ww = (w()-Fl::box_dw(box())-4)/fl_width('W');
  int hh = (h()-Fl::box_dh(box())-4)/ch;
  int xx = x()+Fl::box_dx(box())+2;
  int yy = y()+Fl::box_dy(box())+2;
  int nh = hh;
  for (int j=0; j<nh; j++) {
    int ys = yy;
    const char *sym = symbols->get(addr);
    if (sym) {
      yy += ch; j++;
      if (j>=nh) continue;
    }
    yy += ch; 
    if (my>=ys && my<yy) return addr;
    addr += 4;
  }
  return -1;
}


/**
 * Handle keyboard shortcuts etc.
 */
int Dn_Asm_View::handle(int ev) {
  unsigned int addr;
  switch (ev) {
    case FL_FOCUS:
      return 1;
    case FL_UNFOCUS:
      return 1;
    case FL_PUSH:
      take_focus();
      addr = find_addr(Fl::event_x(), Fl::event_y());
      if (Fl::event_state(FL_SHIFT)) {
        pMarkB = addr;
      } else {
        pMarkA = addr;
      }
      redraw();
      break;
    case FL_MOUSEWHEEL:
      pOffset += 20*Fl::event_dy();
      redraw();
      return 1;
    case FL_KEYBOARD:
      switch (Fl::event_key()) {
        case FL_Page_Up: pOffset -= 20; redraw(); return 1;
        case FL_Page_Down: pOffset += 20; redraw(); return 1;
      }
      break;
  }
  return Fl_Box::handle(ev);
}


void Dn_Asm_View::save_range(FILE *f, unsigned int a, unsigned int b) {
  unsigned int addr = a, newAddr, v;
  for (;addr<b;) {
    char buf[2048];
    newAddr = addr + pCpu->dis(buf, addr);
    fprintf(f, "  // %08x %s", addr, buf);
    addr = newAddr;
  }
}

void Dn_Asm_View::save_range_cc(FILE *f, unsigned int a, unsigned int b) {
  unsigned int addr = a, newAddr, v;
  for (;addr<b;) {
    char buf[2048];
    newAddr = addr + pCpu->dis_cc(buf, addr);
    fprintf(f, "  %s", buf);
    addr = newAddr;
  }
}

void Dn_Asm_View::save_marked_cc(const char *filename) {
  FILE *f = fopen(filename, "wb");
  unsigned int addr = pMarkA, newAddr;
  for (;addr<=pMarkB;) {
    const char *sym = symbols->get(addr);
    if (sym) fprintf(f, "/* %s */\n", sym);
    char buf[2048];
    newAddr = addr + pCpu->dis_cc(buf, addr);
    fprintf(f, "  %s", buf);
    addr = newAddr;
  }
  fclose(f);
}


void Dn_Asm_View::save_marked(const char *filename) {
  FILE *f = fopen(filename, "wb");
  unsigned int addr = pMarkA, newAddr;
  for (;addr<=pMarkB;) {
    const char *sym = symbols->get(addr);
    if (sym) fprintf(f, "; %s\n", sym);
    char buf[2048];
    newAddr = addr + pCpu->dis(buf, addr);
    fprintf(f, "%08x         %s", addr, buf);
    addr = newAddr;
  }
  fclose(f);
}


