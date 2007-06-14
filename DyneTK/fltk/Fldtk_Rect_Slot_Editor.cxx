//
// "$Id$"
//
// Fldtk_Rect_Slot_Editor implementation for the FLMM extension to FLTK.
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

#include "Fldtk_Rect_Slot_Editor.h"
#include "dtk/Dtk_Script_Slot.h"

#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Box.H>

#include <stdio.h>


Fldtk_Rect_Slot_Editor::Fldtk_Rect_Slot_Editor(Fl_Group *container, Dtk_Rect_Slot *slot)
:   Fl_Group(container->x(), container->y(), container->w(), container->h()),
    slot_(slot)
{
    wLeft_  = new Fl_Int_Input(x()+50, y()+25, 70, 20, "Left:");
    wLeft_->textsize(12);
    wLeft_->textfont(FL_COURIER);
    wLeft_->labelsize(12);
    wRight_  = new Fl_Int_Input(x()+180, y()+25, 70, 20, "Right:");
    wRight_->textsize(12);
    wRight_->textfont(FL_COURIER);
    wRight_->labelsize(12);
    wTop_   = new Fl_Int_Input(x()+50, y()+55, 70, 20, "Top:");
    wTop_->textsize(12);
    wTop_->textfont(FL_COURIER);
    wTop_->labelsize(12);
    wBottom_ = new Fl_Int_Input(x()+180, y()+55, 70, 20, "Bottom:");
    wBottom_->textsize(12);
    wBottom_->textfont(FL_COURIER);
    wBottom_->labelsize(12);
    // FIXME add a box that shows the calculated width and height
    end();
}


Fldtk_Rect_Slot_Editor::~Fldtk_Rect_Slot_Editor()
{
}


void Fldtk_Rect_Slot_Editor::rect(int t, int l, int b, int r)
{
    char buf[32];
    sprintf(buf, "%d", t); wTop_->value(buf);
    sprintf(buf, "%d", l); wLeft_->value(buf);
    sprintf(buf, "%d", b); wBottom_->value(buf);
    sprintf(buf, "%d", r); wRight_->value(buf);
}


//
// End of "$Id$".
//
