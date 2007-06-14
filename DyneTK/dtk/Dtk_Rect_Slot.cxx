//
// "$Id$"
//
// Dtk_Rect_Slot implementation for the Dyne Toolkit.
//
// Copyright 2007 by Matthias Melcher.
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


#include "Dtk_Rect_Slot.h"
#include "Dtk_Layout_Document.h"
#include "fltk/Fldtk_Rect_Slot_Editor.h"
#include "fltk/Fldtk_Slot_Editor_Group.h"

#include "allNewt.h"

#include <string.h>


/*---------------------------------------------------------------------------*/
Dtk_Rect_Slot::Dtk_Rect_Slot(Dtk_Slot_List *list, const char *theKey, newtRef slot)
:   Dtk_Slot(list, theKey, slot),
    editor_(0L),
    top_(0), left_(0), bottom_(0), right_(0)
{
/*
    newtRef value = NewtGetFrameSlot(slot, NewtFindSlotIndex(slot, NSSYM(value)));
    if (NewtRefIsString(value)) {
        script_ = strdup(NewtRefToString(value));
        char *s = script_;
        for (;;s++) {
            char c = *s;
            if (!c) break;
            if (c=='\r') *s = '\n';
        }
    }
*/
}


/*---------------------------------------------------------------------------*/
Dtk_Rect_Slot::~Dtk_Rect_Slot()
{
    if (editor_)
        editor_->parent()->remove(editor_);
}

/*---------------------------------------------------------------------------*/
void Dtk_Rect_Slot::edit()
{
    Fldtk_Slot_Editor_Group *container = layout()->slotEditor();
    if (!editor_) {
        container->begin();
        editor_ = new Fldtk_Rect_Slot_Editor(container, this);
        container->end();
        editor_->rect(top_, left_, bottom_, right_);
    }
    container->value(editor_);
}

//
// End of "$Id$".
//
