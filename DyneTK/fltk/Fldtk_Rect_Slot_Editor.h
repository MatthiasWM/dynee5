//
// "$Id$"
//
// Fldtk_Rect_Slot_Editor header file for the FLMM extension to FLTK.
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

#ifndef FLDTK_RECT_SLOT_EDITOR_H
#define FLDTK_RECT_SLOT_EDITOR_H


#include <FL/Fl_Group.H>


class Dtk_Rect_Slot;
class Fl_Input;
class Fl_Box;


/** GUI for editing script slots.
 */
class Fldtk_Rect_Slot_Editor : public Fl_Group
{
public:
	        Fldtk_Rect_Slot_Editor(Fl_Group *container, Dtk_Rect_Slot *slot);
	        ~Fldtk_Rect_Slot_Editor();
    void    rect(int t, int l, int b, int r);

protected:
    static void update_cb(Fl_Widget*, Fldtk_Rect_Slot_Editor*);

    Dtk_Rect_Slot * slot_;
    Fl_Input    *wTop_, *wLeft_, *wBottom_, *wRight_;
    Fl_Box      *wWidth_, *wHeight_;
};


#endif

//
// End of "$Id$".
//
