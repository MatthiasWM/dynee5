//
// "$Id$"
//
// Fldtk_Script_Slot_Editor header file for the FLMM extension to FLTK.
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

#ifndef FLDTK_SCRIPT_SLOT_EDITOR_H
#define FLDTK_SCRIPT_SLOT_EDITOR_H


#include "fltk/Flmm_Newt_Script_Editor.h"


class Dtk_Script_Slot;


/** GUI for editing script slots.
 */
class Fldtk_Script_Slot_Editor : public Flmm_Newt_Script_Editor
{
public:
	        Fldtk_Script_Slot_Editor(Fl_Group *container, Dtk_Script_Slot *slot);
	        ~Fldtk_Script_Slot_Editor();
    void    text(char *script);
protected:
    Dtk_Script_Slot * slot_;
};


#endif

//
// End of "$Id$".
//
