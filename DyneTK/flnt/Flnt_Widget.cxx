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

#include <FL/Fl_Group.H>


/*---------------------------------------------------------------------------*/
Flnt_Widget::Flnt_Widget(Dtk_Template *tmpl)
:   Fl_Group(0, 0, 10, 10),
    template_(tmpl)
{
    newtResize();
    init_sizes();
    copy_label(tmpl->browserName());
    align(FL_ALIGN_INSIDE|FL_ALIGN_WRAP|FL_ALIGN_CLIP|FL_ALIGN_TOP|FL_ALIGN_LEFT);
    labelfont((Fl_Font)16);
    labelsize(10);
    box(FL_BORDER_BOX);
}


/*---------------------------------------------------------------------------*/
Flnt_Widget::~Flnt_Widget()
{
}

/*---------------------------------------------------------------------------*/
void Flnt_Widget::newtResize()
{
    int t, l, b, r;
    int X, Y, W, H;
    unsigned int jst;

    template_->getSize(t, l, b, r);
    jst = template_->justify();

    // FIXME these following calculations depend heavily on the justification flags
    X = l;
    Y = t;
    W = r - l;
    H = b - t;

    resize(X, Y, W, H);
}


//
// End of "$Id$".
//
