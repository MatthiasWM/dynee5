//
// "$Id$"
//
// Fldtk_Layout_Editor implementation for the FLMM extension to FLTK.
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

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include "Fldtk_Layout_Editor.h"
#include "Fldtk_Slot_Editor_Group.h"
#include "dtk/Dtk_Layout_Document.h"
#include "dtk/Dtk_Platform.h"
#include "main.h"
#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Menu_Button.H>

#include <stdio.h>
#include <stdlib.h>


/*---------------------------------------------------------------------------*/
Fl_Menu_Item specificMenu[] = {
    { "viewBounds" },
    { "viewFlags" },
    { "viewFormat" },
    { 0 }
};


/*---------------------------------------------------------------------------*/
Fldtk_Layout_Editor::Fldtk_Layout_Editor(Dtk_Layout_Document *layout)
:   Fldtk_Editor(layout->name()),
    layout_(layout),
	tmplBrowser_(0L),
	slotBrowser_(0L),
    slotEditor_(0L),
    specificChoice_(0L),
    methodsChoice_(0L),
    attributesChoice_(0L)
{
    int X = x()+3, Y = y()+3, W = w()-6, H = h()-6;
    begin();
    {
        Fl_Tile *tile = new Fl_Tile(X, Y, W, H);
        {
            Fl_Group *nw = new Fl_Group(X, Y, W/2, H/4);
            {
                tmplBrowser_ = new Fl_Hold_Browser(X, Y, W/2-2, H/4-2);
                tmplBrowser_->textsize(12);
                nw->resizable(tmplBrowser_);
            }
            nw->box(FL_FLAT_BOX);
            nw->end();
            Fl_Group *ne = new Fl_Group(X+W/2, Y, W-W/2, H/4);
            {
                slotBrowser_ = new Fl_Hold_Browser(X+W/2+2, Y, W-W/2-2, H/4-2);
                slotBrowser_->textsize(12);
                ne->resizable(slotBrowser_);
            }
            ne->box(FL_FLAT_BOX);
            ne->end();
            Fl_Group *s = new Fl_Group(X, Y+H/4, W, H-H/4);
            {
                Fl_Group *slotMenus = new Fl_Group(X, Y+H/4+2, W, 30);
                {
                    // pulldown menu to select specific methods of a templat
                    specificChoice_ = new Fl_Menu_Button(X+10, Y+H/4+2+4, 100, 20, "Specific");
                    specificChoice_->labelsize(12);
                    specificChoice_->textsize(12);
                    specificChoice_->menu(specificMenu);
                    specificChoice_->deactivate();
                    // pulldown menu for standard methods
                    methodsChoice_ = new Fl_Menu_Button(X+10+110, Y+H/4+2+4, 100, 20, "Methods");
                    methodsChoice_->labelsize(12);
                    methodsChoice_->textsize(12);
                    methodsChoice_->menu(dtkPlatform->methodsChoiceMenu());
                    methodsChoice_->deactivate();
                    // pulldown menu for standard attributes
                    attributesChoice_ = new Fl_Menu_Button(X+10+220, Y+H/4+2+4, 100, 20, "Attributes");
                    attributesChoice_->labelsize(12);
                    attributesChoice_->textsize(12);
                    attributesChoice_->menu(dtkPlatform->attributesChoiceMenu());
                    attributesChoice_->deactivate();
                    // box to manage nice resizing behavior
                    Fl_Box *space = new Fl_Box(X+10+320, Y+H/4+2+4, (X+W-10-150)-(X+10+320), 20);
                    space->box(FL_NO_BOX);
                    slotMenus->resizable(space);
                    // the "Apply" button copies the GUI settings into the template
                    // FIXME should initially be disabled until the slot editor changes
                    Fl_Button *apply = new Fl_Button(X+W-10-150, Y+H/4+2+4, 70, 20, "Apply");
                    apply->labelsize(12);
                    apply->callback((Fl_Callback*)apply_cb, this);
                    // the "Revert" button copies the template settings into the GUI
                    // FIXME should initially be disabled until the slot editor changes
                    Fl_Button *revert = new Fl_Button(X+W-10-70, Y+H/4+2+4, 70, 20, "Revert");
                    revert->labelsize(12);
                    revert->callback((Fl_Callback*)revert_cb, this);
                }
                slotMenus->end();
                slotEditor_ = new Fldtk_Slot_Editor_Group(X, Y+H/4+2+30, W, H-H/4-2-30);
                {
                    Fl_Box *box = new Fl_Box(X, Y+H/4+2+30, W, H-H/4-2-30);
                    box->box(FL_DOWN_BOX);
                }
                slotEditor_->box(FL_FLAT_BOX);
                slotEditor_->end();
                s->resizable(slotEditor_);
            }
            s->box(FL_FLAT_BOX);
            s->end();
        }
        tile->end();
        resizable(tile);
    }
    end();

    
    /*
	editor_->resize(x(), y()+h()/4, w(), h()-h()/4);
	editor_->box(FL_DOWN_BOX);
	tree_ = new Fl_Hold_Browser(x(), y(), w()/2, h()/4);
	member_ = new Fl_Hold_Browser(x()+w()/2, y(), w()-w()/2, h()/4);
	init_sizes();
	resizable(this);
*/
}


/*---------------------------------------------------------------------------*/
Fldtk_Layout_Editor::~Fldtk_Layout_Editor()
{
}


/*---------------------------------------------------------------------------*/
Dtk_Document *Fldtk_Layout_Editor::document() 
{
    return layout_; 
}


/*---------------------------------------------------------------------------*/
void Fldtk_Layout_Editor::apply_cb(Fl_Widget*, Fldtk_Layout_Editor *w)
{
    Fl_Widget *slot = w->slotEditor_->value();
    if (slot) slot->do_callback(slot, 'aply');
}


/*---------------------------------------------------------------------------*/
void Fldtk_Layout_Editor::revert_cb(Fl_Widget*, Fldtk_Layout_Editor *w)
{
    Fl_Widget *slot = w->slotEditor_->value();
    if (slot) slot->do_callback(slot, 'rvrt');
}


/*---------------------------------------------------------------------------*/
void Fldtk_Layout_Editor::userDeselectedTemplates()
{
    specificChoice_->deactivate();
    methodsChoice_->deactivate();
    attributesChoice_->deactivate();
}


/*---------------------------------------------------------------------------*/
void Fldtk_Layout_Editor::userSelectedTemplate(Dtk_Template *tmpl)
{
    specificChoice_->menu(dtkPlatform->specificChoiceMenu(tmpl));
    specificChoice_->activate();
    methodsChoice_->activate();
    attributesChoice_->activate();

    Dtk_Platform::updateActivation((Fl_Menu_Item*)specificChoice_->menu(), tmpl);
    Dtk_Platform::updateActivation((Fl_Menu_Item*)methodsChoice_->menu(), tmpl);
    Dtk_Platform::updateActivation((Fl_Menu_Item*)attributesChoice_->menu(), tmpl);
}

//
// End of "$Id$".
//
