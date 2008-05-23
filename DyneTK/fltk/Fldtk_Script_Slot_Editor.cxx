//
// "$Id$"
//
// Fldtk_Script_Slot_Editor implementation for the FLMM extension to FLTK.
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

#include "Fldtk_Script_Slot_Editor.h"
#include "dtk/Dtk_Script_Slot.h"

#include <FL/Fl_Text_Buffer.H>


Fldtk_Script_Slot_Editor::Fldtk_Script_Slot_Editor(Fl_Group *container, Dtk_Script_Slot *slot)
:   Flmm_Newt_Script_Editor(container->x(), container->y(), container->w(), container->h()),
    slot_(slot)
{
    //buffer(new Fl_Text_Buffer());
	buffer()->tab_distance(4);
	textfont(FL_COURIER);
	textsize(12);
    callback((Fl_Callback*)editor_cb);
}


Fldtk_Script_Slot_Editor::~Fldtk_Script_Slot_Editor()
{
}


void Fldtk_Script_Slot_Editor::text(char *script) 
{
  if (script)
    buffer()->text(script);
  else
    buffer()->text("");
}

char *Fldtk_Script_Slot_Editor::text() 
{
    return buffer()->text();
}

void Fldtk_Script_Slot_Editor::editor_cb(Fldtk_Script_Slot_Editor *w, unsigned int cmd)
{
    switch (cmd) {
    case 'aply': w->slot_->apply(); break;
    case 'rvrt': w->slot_->revert(); break;
    }
}


//
// End of "$Id$".
//
