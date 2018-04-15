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
#include "dtk/Dtk_Layout.h"
#include "fltk/Fldtk_Layout_View.h"
#include "globals.h"

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <stdio.h>


Flnt_Widget *Flnt_Widget::rubberband_ = 0L;


/*---------------------------------------------------------------------------*/
Flnt_Widget::Flnt_Widget(Dtk_Template *tmpl, Dtk_Layout *layout)
:   Fl_Group(0, 0, 10, 10),
    top_(0), left_(0), bottom_(0), right_(0), justify_(0),
    template_(tmpl),
    layout_(layout)
{
    if (!layout_ && template_)
        layout_ = template_->layout();
    if (tmpl)
        copy_label(tmpl->widgetName());
    screenToNewt();
    init_sizes();
    align(FL_ALIGN_INSIDE|FL_ALIGN_WRAP|FL_ALIGN_CLIP|FL_ALIGN_TOP|FL_ALIGN_LEFT);
    //labelfont((Fl_Font)16);
    labelsize(10);
    box(FL_BORDER_BOX);
    resizable(0L);
}


/*---------------------------------------------------------------------------*/
Flnt_Widget::~Flnt_Widget()
{
}


/*---------------------------------------------------------------------------*/
void Flnt_Widget::newtToScreen()
{
    // FIXME support all justify flags
    Fl_Group *p = parent();
    int xx = left_, yy = top_, ww = right_-left_, hh = bottom_-top_;

    // horizontal justification to parent
	switch (justify_&48) {
	case 0: // align left
		ww = right_ - left_;
		xx = p->x() + left_;
		break;
	case 16: // center
		ww = right_-left_;
		xx = p->x() + (p->w()-ww)/2 + left_;
		break;
	case 32: // right
		xx = p->x() + p->w() + left_;
		ww = right_ - left_;
		break;
	case 48: // inset
		xx = p->x() + left_;
		ww = p->w() + right_ - left_;
		break;
	}

	switch (justify_&192) {
	case 0: // align top_
		yy = p->y() + top_;
		hh = bottom_ - top_;
		break;
	case 64: // center
		hh = bottom_-top_;
		yy = p->y() + (p->h()-hh)/2 + top_;
		break;
	case 128: // bottom_
		yy = p->y() + p->h() + top_;
		hh = bottom_ - top_;
		break;
	case 192: // inset
		yy = p->y() + top_;
		hh = p->h() + bottom_ - top_;
		break;
	}

    resize(xx, yy, ww, hh);
}


/*---------------------------------------------------------------------------*/
void Flnt_Widget::screenToNewt()
{
    // FIXME support all justify flags
    Fl_Group *p = parent();
    top_ = y(); left_ = x(); bottom_ = y()+h(); right_ = x()+w();
    int dd = 0;

    // horizontal justification to parent
	switch (justify_&48) {
	case 0: // align left
        left_ -= p->x();
        right_ -= p->x();
		break;
	case 16: // center
        dd = p->x() + (p->w()-right_+left_)/2;
        left_  = left_  - dd;
        right_ = right_ - dd;
		break;
	case 32: // right
        dd = p->x() + p->w();
        left_  = left_  - dd;
        right_ = right_ - dd;
		break;
	case 48: // inset
        left_ -= p->x();
        right_ = right_ - (p->x()+p->w());
		break;
	}

    // vertical justification to parent
	switch (justify_&192) {
	case 0: // align top
        top_    -= p->y();
        bottom_ -= p->y();
		break;
	case 64: // center
        dd = p->y() + (p->h()-bottom_+top_)/2;
        top_  = top_  - dd;
        bottom_ = bottom_ - dd;
		break;
	case 128: // bottom
        dd = p->y() + p->h();
        top_  = top_  - dd;
        bottom_ = bottom_ - dd;
		break;
	case 192: // inset
        top_ -= p->y();
        bottom_ = bottom_ - (p->y()+p->h());
		break;
	}

}


/*---------------------------------------------------------------------------*/
/*
void Flnt_Widget::newtResize()
{
    if (!template_)
        return;

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

	//printf("Layout: 0x%08x %9d, %4d %4d %4d %4d \"%s\"\n", justify, justify, left, top, right, bottom, label());
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
*/


/*---------------------------------------------------------------------------*/
void Flnt_Widget::newtSetJustify(unsigned int justify)
{
    justify_ = justify;
    screenToNewt();
    parent()->redraw();
}


/*---------------------------------------------------------------------------*/
void Flnt_Widget::newtSetRect(int top, int left, int bottom, int right)
{
    top_ = top;
    left_ = left;
    bottom_ = bottom;
    right_ = right;
    newtToScreen();
    parent()->redraw();
}


/*---------------------------------------------------------------------------*/
void Flnt_Widget::newtGetRect(int &top, int &left, int &bottom, int &right)
{
    screenToNewt();
    top = top_;
    left = left_;
    bottom = bottom_;
    right = right_;
}


/*---------------------------------------------------------------------------*/
void Flnt_Widget::layoutChildren()
{
    screenToNewt();
    int i, n = children();
    for (i=0; i<n; i++) {
        Flnt_Widget *ci = (Flnt_Widget*)child(i);
        ci->newtToScreen();
        ci->layoutChildren();
    }
}


// mode 0=drag, 1=resize, 2=create
static int mode, ox, oy, wx, wy, ww, wh;

int Flnt_Widget::handle(int event)
{
    switch(event) {
        case FL_PUSH:
            mode = 0;
            if (!Fl_Group::handle(event)) {
                if (template_ && !template_->isSelected()) {
                    template_->selectedInView();
                    redraw();
                } 
                ox = Fl::event_x();
                oy = Fl::event_y();
                wx = x(); wy = y();
                ww = w(); wh = h();
                if (template_)
                    mode = 1;
                if (Fldtk_Layout_View::mode()==1) {
                    mode = 3;
                    rubberband_ = this;
                    redraw();
                } else if (template_ && ox>=x()+w()-8 && oy>=y()+h()-8) {
                    mode = 2;
                }
            }
            return 1;
        case FL_DRAG:
            if (mode /*&& !Fl::event_is_click()*/) {
                if (mode==1) { // drag widget
                    position(wx-ox+Fl::event_x(), wy-oy+Fl::event_y());
                    layoutChildren();
                    window()->redraw();
                } else if (mode==2) { // resize widget
                    size(ww-ox+Fl::event_x(), wh-oy+Fl::event_y());
                    layoutChildren();
                    window()->redraw();
                } else if (mode==3) { // create a new widget
                    wx = Fl::event_x();
                    wy = Fl::event_y();
                    redraw();
                }
                // FIXME tNewtReverseResize();
                // FIXME template_->setSize();
                return 1;
            }
            break;
        case FL_RELEASE:
            wx = Fl::event_x();
            wy = Fl::event_y();
            if (mode==1 || mode==2) {
                layoutChildren();
                screenToNewt();
                signalBoundsChanged(this);
            } else if (mode==3) {
                if (template_) {
                    template_->addTemplate(ox, oy, wx-ox, wy-oy);
                } else {
                    layout_->addTemplate(ox, oy, wx-ox, wy-oy);
                }
                SetModeEditTemplate();
                mode = 0;
                rubberband_ = 0L;
                redraw();
                return 1;
            }
            break;

    }
    return Fl_Group::handle(event);
}

void Flnt_Widget::draw()
{
    fl_color(FL_BLACK);
    if (template_)
        fl_rect(x(), y(), w(), h());
    fl_push_clip(x()+1, y()+1, w()-2, h()-2);
    Fl_Group::draw();
    if (template_ && template_->isSelected()) {
        fl_color(FL_BLACK);
        fl_rectf(x()+w()-7, y()+h()-7, 5, 5);
        fl_yxline(x()+2, y()+7, y()+2, x()+7);
        fl_yxline(x()+w()-3, y()+7, y()+2, x()+w()-8);
        fl_yxline(x()+2, y()+h()-8, y()+h()-3, x()+7);
    }
    if (rubberband_==this) {
        fl_color(FL_RED);
        fl_rect(ox, oy, wx-ox, wy-oy);
    }
    fl_pop_clip();
}

//
// End of "$Id$".
//
