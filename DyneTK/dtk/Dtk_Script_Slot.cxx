//
// "$Id$"
//
// Dtk_Script_Slot implementation for the Dyne Toolkit.
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


#include "Dtk_Script_Slot.h"
#include "Dtk_Layout_Document.h"
#include "Dtk_Script_Writer.h"
#include "fltk/Fldtk_Script_Slot_Editor.h"
#include "fltk/Fldtk_Slot_Editor_Group.h"

#include "allNewt.h"

#include <FL/Fl_Wizard.H>

#include <string.h>


/*---------------------------------------------------------------------------*/
Dtk_Script_Slot::Dtk_Script_Slot(Dtk_Slot_List *list, const char *theKey, newtRef slot)
:   Dtk_Slot(list, theKey, slot),
    editor_(0L),
    script_(0L)
{
    if (slot!=kNewtRefUnbind) {
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
	    newtRef dt = NewtGetFrameSlot(slot, NewtFindSlotIndex(slot, NSSYM(__ntDatatype)));
	    if (NewtRefIsString(dt)) {
            datatype_ = strdup(NewtRefToString(dt));
        } else {
            datatype_ = strdup("SCPT");
        }
    }
}


/*---------------------------------------------------------------------------*/
Dtk_Script_Slot::~Dtk_Script_Slot()
{
    if (editor_)
        editor_->parent()->remove(editor_);
    if (script_)
        free(script_);
}

/*---------------------------------------------------------------------------*/
void Dtk_Script_Slot::edit()
{
    Fldtk_Slot_Editor_Group *container = layout()->slotEditor();
    if (!editor_) {
        container->begin();
        editor_ = new Fldtk_Script_Slot_Editor(container, this);
        container->end();
        editor_->text(script_);
    }
    container->value(editor_);
}

/*---------------------------------------------------------------------------*/
int Dtk_Script_Slot::write(Dtk_Script_Writer &sw)
{
    char buf[1024];
    if (strchr(script_, '\n')) {
        sprintf(buf, "     %s:\n", key_);
        sw.put(buf);
        char *src = script_, *end = script_;
        for (;;) {
            if (*end==0) break;
            src = end;
            sw.put("        ");
            for (;;) {
                char c = *end;
                if (c==0 || c=='\n') break;
                end++;
            }
            sw.put(src, end-src);
            if (*end==0) 
                break;
            sw.put("\n");
            end++;
        }
        if (strstr(src, "//")) {
            sw.put("\n        ");
        }
    } else {
        sprintf(buf, "     %s: ", key_);
        sw.put(buf);
        sw.put(script_);
        // if the last buffer contained a double-slash comment, we must add a new line
        if (strstr(script_, "//"))
            sw.put("\n        ");
    }
    return 0;
}


/*---------------------------------------------------------------------------*/
void Dtk_Script_Slot::apply() 
{ 
    if (script_)
        free(script_);
    script_ = editor_->text();
}


/*---------------------------------------------------------------------------*/
void Dtk_Script_Slot::revert() 
{
    editor_->text(script_);
}


/*---------------------------------------------------------------------------*/
void Dtk_Script_Slot::set(const char *script) 
{
    if (script_)
        free(script_);
    script_ = strdup(script);
    if (editor_)
        revert();
}


/*---------------------------------------------------------------------------*/
newtRef	Dtk_Script_Slot::save()
{
    newtRefVar slotA[] = {
        NSSYM(value),           NewtMakeString(script_, false),
        NSSYM(__ntDataType),    NewtMakeString(datatype_, false) };
    newtRef slot = NewtMakeFrame2(2, slotA);

    return slot;
}


//
// End of "$Id$".
//
