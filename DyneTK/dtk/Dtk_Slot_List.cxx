//
// "$Id$"
//
// Dtk_Slot_List implementation for the Dyne Toolkit.
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

// FIXME	Make sure that we can't add the same document twice. If we open the same
//			document, just bring it to the front.

// FIXME	For new documents, come up with a new name if the old one is taken.
//			Increment some number before the extension.



#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include "Dtk_Slot_List.h"
#include "Dtk_Slot.h"
#include "Dtk_Template.h"
#include "Dtk_Layout_Document.h"
#include "fltk/Fldtk_Slot_Editor_Group.h"

#include "allNewt.h"

#include <FL/Fl_Hold_Browser.H>



/*---------------------------------------------------------------------------*/
Dtk_Slot_List::Dtk_Slot_List(Dtk_Template *tmpl)
:   template_(tmpl),
    browser_(0L)
{
    browser_ = layout()->slotBrowser();
}


/*---------------------------------------------------------------------------*/
Dtk_Slot_List::~Dtk_Slot_List()
{
    int i, n = slotList_.size();
    for (i=n-1; i>=0; --i) {
        Dtk_Slot *slot = slotList_.at(i);
        delete slot;
    }
    browser_ = 0L;
    template_ = 0L;
}


/*---------------------------------------------------------------------------*/
Dtk_Layout_Document *Dtk_Slot_List::layout() 
{
    return template_->layout();
}


/*---------------------------------------------------------------------------*/
void Dtk_Slot_List::add(Dtk_Slot *slot) 
{
    slotList_.push_back(slot);
}


/*---------------------------------------------------------------------------*/
void Dtk_Slot_List::remove(Dtk_Slot *slot) 
{
    int i, n = slotList_.size();
    for (i=n-1; i>=0; --i) {
        if (slotList_.at(i)==slot) {
            slotList_.erase(slotList_.begin()+i);
            if (browser_) {
                browser_->remove(i+1);
                browser_->value(0);
            }
            if (layout()) {
                layout()->slotEditor()->blank();
                layout()->updateMenus();
            }
            delete slot;
            break;
        }
    }
}


/*---------------------------------------------------------------------------*/
int Dtk_Slot_List::size()
{
    return slotList_.size();
}


/*---------------------------------------------------------------------------*/
Dtk_Slot *Dtk_Slot_List::at(int index)
{
    return slotList_.at(index);
}


/*---------------------------------------------------------------------------*/
Dtk_Slot *Dtk_Slot_List::find(const char *key)
{
    int i, n = slotList_.size();
    for (i=n-1; i>=0; --i) {
        Dtk_Slot *slot = slotList_.at(i);
        if (strcasecmp(slot->key(), key)==0)
            return slot;
    }
    return 0L;
}


/*---------------------------------------------------------------------------*/
void Dtk_Slot_List::updateKey(Dtk_Slot *slot)
{
    if (!browser_)
        return;
    int i, n = size();
    for (i=0; i<n; ++i) {
        if (slotList_.at(i)==slot) {
            browser_->text(i+1, slot->key());
        }
    }
}


//
// End of "$Id$".
//
