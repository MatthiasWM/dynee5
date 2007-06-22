//
// "$Id$"
//
// Dtk_Template implementation for the Dyne Toolkit.
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


#include "Dtk_Template_Proto.h"
#include "Dtk_Template.h"
#include "Dtk_Script_Slot.h"
#include "Dtk_Rect_Slot.h"

#include <FL/Fl_Menu_Item.H>


/*    
    {
        slotList_ = new Dtk_Slot_List(this);
        ntId_ = strdup(proto->name);
        viewBounds_ = new Dtk_Rect_Slot(slotList_, "viewBounds");
        slotList_->add(viewBounds_);
    }
*/

static void setupProtoTextButton(Dtk_Template *t)
{
    //ntId_ = strdup(proto->name);
    //slotList_ = new Dtk_Slot_List(this);

    Dtk_Script_Slot *buttonClickScript = new Dtk_Script_Slot(t->slotList(), "buttonClickScript");
    buttonClickScript->set("func()\nbegin\nend");
    t->add(buttonClickScript);

    Dtk_Script_Slot *text = new Dtk_Script_Slot(t->slotList(), "text");
    text->set("\"Button\"");
    t->add(text);

    Dtk_Rect_Slot *viewBounds = new Dtk_Rect_Slot(t->slotList(), "viewBounds");
    t->add(viewBounds); //t->viewBounds_ = viewBounds;
}

Dtk_Template_Proto proto[] = {
    { "protoApp", 157, setupProtoTextButton  },
    { "protoTextButton", 226, setupProtoTextButton },
};


#define DTK_T_MENU(i) \
    { proto[i].name, 0, 0L, proto+i, 0, FL_NORMAL_LABEL, 0, 11, 0}


static Fl_Menu_Item template_menu[] = {
    DTK_T_MENU(0),
    DTK_T_MENU(1)
};


Fl_Menu_Item *Dtk_Template_Proto::menu = template_menu;


//
// End of "$Id$".
//
