//
// "$Id$"
//
// Fldtk_Layout_View implementation for the FLMM extension to FLTK.
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

#include "Fldtk_Layout_View.h"
#include "flnt/Flnt_Widget.h"
#include "globals.h"
#include "fluid/main_ui.h"

#include <FL/Fl.H>

/*---------------------------------------------------------------------------*/


int Fldtk_Layout_View::mode_ = 0;


/*---------------------------------------------------------------------------*/
Fldtk_Layout_View::Fldtk_Layout_View(Dtk_Layout *layout, int width, int height)
:   Fl_Double_Window(width, height),
    layout_(layout)
{
    set_non_modal();
    callback(hide_cb, this);
    begin();
    Flnt_Widget *root = new Flnt_Widget(0L, layout);
    root->color(FL_LIGHT2);
    root->resize(0, 0, width, height);
}


/*---------------------------------------------------------------------------*/
Fldtk_Layout_View::~Fldtk_Layout_View()
{
    hide();
    UpdateMainMenu();
}


/*---------------------------------------------------------------------------*/
void Fldtk_Layout_View::hide_cb(Fl_Widget *w, void*)
{
    ((Fldtk_Layout_View*)w)->hide();
    UpdateMainMenu();
}

/*---------------------------------------------------------------------------*/
void Fldtk_Layout_View::mode(int m)
{
    switch (m) {
    case 0:
        dtkMain->tLayoutModeEdit->value(1);
        dtkMain->tLayoutModeAdd->value(0);
        break;
    case 1:
        dtkMain->tLayoutModeEdit->value(0);
        dtkMain->tLayoutModeAdd->value(1);
        break;
    }
    mode_ = m;
}


//
// End of "$Id$".
//
