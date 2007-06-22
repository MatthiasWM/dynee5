//
// "$Id$"
//
// Fldtk_Editor implementation for the FLMM extension to FLTK.
//
// Copyright 2002-2007 by Matthias Melcher.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "flmm@matthiasm.com".
//


#include "Flnt_Widget.h"
#include "dtk/Dtk_Template.h"
#include "fltk/Fldtk_Layout_View.h"
#include "globals.h"

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.h>
#include <stdio.h>


/*---------------------------------------------------------------------------*/
Flnt_Widget::Flnt_Widget(Dtk_Template *tmpl)
:   Fl_Group(0, 0, 10, 10),
    template_(tmpl)
{
    copy_label(tmpl->browserName());
    newtResize();
    init_sizes();
    align(FL_ALIGN_INSIDE|FL_ALIGN_WRAP|FL_ALIGN_CLIP|FL_ALIGN_TOP|FL_ALIGN_LEFT);
    labelfont((Fl_Font)16);
    labelsize(10);
    box(FL_BORDER_BOX);
    //align(FL_ALIGN_CLIP);
    resizable(0L);
}


/*---------------------------------------------------------------------------*/
Flnt_Widget::~Flnt_Widget()
{
}

/*---------------------------------------------------------------------------*/
void Flnt_Widget::newtResize()
{
    Fl_Group *par = parent();
    Fl_Widget *sib = 0L;
	if (sib==0L && par!=0L) {
		int ix = par->find(this);
		if (ix>0 && ix<par->children())
			sib = par->child(ix-1);
		else
			sib = par->child(par->children()-1);
	}

    int top, left, bottom, right;
    template_->getSize(top, left, bottom, right);
    unsigned int justify = template_->justify();
	int d, xx = left, yy = top, ww = right-left, hh = bottom-top;

	printf("Layout: 0x%08x %9d, %4d %4d %4d %4d \"%s\"\n", justify, justify, left, top, right, bottom, label());
	if (justify & 0x0f) {
		d = 3; // not yet supported?
	}

	if (!sib) sib = par;
	int sx = sib->x(), sy = sib->y(), sw = sib->w(), sh = sib->h();
	int px = par->x(), py = par->y(), pw = par->w(), ph = par->h();

	if (par) {
		if (!par) {
			d = 4; // error!
		}
		switch (justify&48) {
		case 0: // align left
			xx = px + left;
			ww = right - left;
			break;
		case 16: // center
			ww = right-left;
			xx = px + 0.5*(pw-ww) + left;
			break;
		case 32: // right
			xx = px + pw + left;
			ww = right - left;
			break;
		case 48: // inset
			xx = px + left;
			ww = pw + right - left;
			break;
		}
		switch (justify&192) {
		case 0: // align top
			yy = py + top;
			hh = bottom - top;
			break;
		case 64: // center
			hh = bottom-top;
			yy = py + 0.5*(ph-hh) + top;
			break;
		case 128: // bottom
			yy = py + ph + top;
			hh = bottom - top;
			break;
		case 192: // inset
			yy = py + top;
			hh = ph + bottom - top;
			break;
		}
	}
	if (justify & 1536) {
		switch (justify & 1536) {
		case 1536: // left rel to left, right rel to right
			xx = sx + left;
			ww = sx + sw + right - xx;
			break;
		}
	}
	if (justify & 12288) {
		switch (justify & 12288) {
		case 8192: // relative to siblings bottom
			yy = sy + sh + top;
			hh = bottom - top;
			break;
		case 12288: // top rel to top and btm rel to btm
			yy = sy + top;
			hh = sy + sh + bottom - yy;
			break;
		}
	}
	resize(xx, yy, ww, hh);
}

int Flnt_Widget::handle(int event)
{
    // mode 0=drag, 1=resize, 2=create
    static int mode, ox, oy, wx, wy, ww, wh;
    switch(event) {
        case FL_PUSH:
            mode = 0;
            if (!Fl_Group::handle(event)) {
                if (!template_->isSelected()) {
                    template_->selectedInView();
                    redraw();
                } 
                ox = Fl::event_x();
                oy = Fl::event_y();
                wx = x(); wy = y();
                ww = w(); wh = h();
                mode = 1;
                if (Fldtk_Layout_View::mode()==1)
                    mode = 3;
                else if (ox>=x()+w()-8 && oy>=y()+h()-8)
                    mode = 2;
            }
            return 1;
        case FL_DRAG:
            if (mode /*&& !Fl::event_is_click()*/) {
                if (mode==1) { // drag widget
                    position(wx-ox+Fl::event_x(), wy-oy+Fl::event_y());
                    window()->redraw();
                } else if (mode==2) { // resize widget
                    size(ww-ox+Fl::event_x(), wh-oy+Fl::event_y());
                    window()->redraw();
                } else if (mode==3) { // create a new widget
                    // FIXME show a rubberband rect
                }
                // FIXME tnewtReverseResize();
                // FIXME template_->setSize();
                return 1;
            }
            break;
        case FL_RELEASE:
            wx = Fl::event_x();
            wy = Fl::event_y();
            if (mode==3) {
                template_->add(ox, oy, wx-ox, wy-oy);
                SetModeEditTemplate();
            }
    }
    return Fl_Group::handle(event);
}

void Flnt_Widget::draw()
{
    fl_color(FL_BLACK);
    fl_rect(x(), y(), w(), h());
    fl_push_clip(x()+1, y()+1, w()-2, h()-2);
    Fl_Group::draw();
    if (template_->isSelected()) {
        fl_color(FL_BLACK);
        fl_rectf(x()+w()-7, y()+h()-7, 5, 5);
        fl_yxline(x()+2, y()+7, y()+2, x()+7);
        fl_yxline(x()+w()-3, y()+7, y()+2, x()+w()-8);
        fl_yxline(x()+2, y()+h()-8, y()+h()-3, x()+7);
    }
    fl_pop_clip();
}

//
// End of "$Id$".
//
