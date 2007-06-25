//
// "$Id$"
//
// Fldtk_Value_Slot_Editor implementation for the FLMM extension to FLTK.
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

#include "Fldtk_Value_Slot_Editor.h"
#include "dtk/Dtk_Value_Slot.h"

#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Box.H>

#include <stdio.h>
#include <math.h>


/*---------------------------------------------------------------------------*/
Fldtk_Value_Slot_Editor::Fldtk_Value_Slot_Editor(Fl_Group *container, Dtk_Value_Slot *slot)
:   Fl_Group(container->x(), container->y(), container->w(), container->h()),
    slot_(slot)
{
    wValue_ = new Fl_Float_Input(x()+50, y()+25, 120, 20, "Value:");
    wValue_->textsize(12);
    wValue_->textfont(FL_COURIER);
    wValue_->labelsize(12);
    end();
    callback((Fl_Callback*)editor_cb, this);
}


/*---------------------------------------------------------------------------*/
Fldtk_Value_Slot_Editor::~Fldtk_Value_Slot_Editor()
{
}


/*---------------------------------------------------------------------------*/
void Fldtk_Value_Slot_Editor::value(double v)
{
    char buf[32];
    double i, f = fabs(modf(v, &i));
    if (f>0.0)
        sprintf(buf, "%.f%0f", i, f);
    else
        sprintf(buf, "%.f", i);
    wValue_->value(buf);
}


/*---------------------------------------------------------------------------*/
double Fldtk_Value_Slot_Editor::value()
{
    return atof(wValue_->value());
}


/*---------------------------------------------------------------------------*/
void Fldtk_Value_Slot_Editor::editor_cb(Fldtk_Value_Slot_Editor *w, unsigned int cmd)
{
    switch (cmd) {
    case 'aply': w->slot_->apply(); break;
    case 'rvrt': w->slot_->revert(); break;
    }
}

//
// End of "$Id$".
//
