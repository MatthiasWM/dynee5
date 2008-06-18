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
#include "dtk/Dtk_Platform.h"
#include "main.h"

#include <FL/Fl_Choice.H>
#include <FL/Fl_Box.H>

#include <stdio.h>
#include <math.h>


Fldtk_Proto_Slot_Editor::Fldtk_Proto_Slot_Editor(Fl_Group *container, Dtk_Proto_Slot *slot)
:   Fl_Group(container->x(), container->y(), container->w(), container->h()),
    slot_(slot)
{
    wProto_ = new Fl_Choice(x()+80, y()+20, 180, 20, "Templates:");
    wProto_->labelsize(12);
    wProto_->textsize(12);
    wProto_->menu(dtkPlatform->templateChoiceMenu());
    //wProto_->callback((Fl_Callback*)specific_choice_cb, this);
  Fl_Box *var = new Fl_Box(x()+260, y()+40, w()-260, h()-40);
  resizable(var);
    callback((Fl_Callback*)editor_cb);
    end();
}


Fldtk_Proto_Slot_Editor::~Fldtk_Proto_Slot_Editor()
{
}


void Fldtk_Proto_Slot_Editor::value(char *id)
{
    const Fl_Menu_Item *mi = wProto_->menu();
    for (;;++mi) {
        const char *t = mi->label();
        if (!t) {
            // FIXME what is a good alternative?
            wProto_->value(wProto_->menu());
        }
        if (strcasecmp(t, id)==0) {
            wProto_->value(mi);
            return;
        }
    }
}

char *Fldtk_Proto_Slot_Editor::value()
{
    return (char*)wProto_->menu()[wProto_->value()].label();
}


void Fldtk_Proto_Slot_Editor::editor_cb(Fldtk_Proto_Slot_Editor *w, unsigned int cmd)
{
    switch (cmd) {
    case 'aply': w->slot_->apply(); break;
    case 'rvrt': w->slot_->revert(); break;
    }
}

//
// End of "$Id$".
//
