//
// "$Id$"
//
// Fldtk_Proto_Slot_Editor implementation for the FLMM extension to FLTK.
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

#include "Fldtk_Proto_Slot_Editor.h"
#include "dtk/Dtk_Proto_Slot.h"

#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Box.H>

#include <stdio.h>
#include <math.h>


Fldtk_Proto_Slot_Editor::Fldtk_Proto_Slot_Editor(Fl_Group *container, Dtk_Proto_Slot *slot)
:   Fl_Group(container->x(), container->y(), container->w(), container->h()),
    slot_(slot)
{
    wValue_ = new Fl_Float_Input(x()+50, y()+25, 120, 20, "Value:");
    wValue_->textsize(12);
    wValue_->textfont(FL_COURIER);
    wValue_->labelsize(12);
    end();
}


Fldtk_Proto_Slot_Editor::~Fldtk_Proto_Slot_Editor()
{
}


void Fldtk_Proto_Slot_Editor::value(int v)
{
    char buf[32];
    sprintf(buf, "%d", v);
    wValue_->value(buf);
}

int Fldtk_Proto_Slot_Editor::value()
{
    return atoi(wValue_->value());
}


//
// End of "$Id$".
//
