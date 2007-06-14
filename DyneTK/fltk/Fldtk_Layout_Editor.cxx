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
#include "dtk/Dtk_Layout_Document.h"

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Wizard.H>

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
Fl_Menu_Item methodsMenu[] = {
    { "afterScript" },
    { "beforeScript" },
    { "pickActionScript" },
    { 0 }
};

/*---------------------------------------------------------------------------*/
Fl_Menu_Item attributesMenu[] = {
    { "copyProtection" },
    { "declareSelf" },
    { "hideSound" },
    { 0 }
};

/*---------------------------------------------------------------------------*/
Fldtk_Layout_Editor::Fldtk_Layout_Editor(Dtk_Layout_Document *layout)
:   Fldtk_Editor(layout->name()),
    layout_(layout),
	tmplBrowser_(0L),
	slotBrowser_(0L)
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
                    Fl_Menu_Button *specific = new Fl_Menu_Button(X+10, Y+H/4+2+4, 100, 20, "Specific");
                    specific->labelsize(12);
                    specific->textsize(12);
                    specific->menu(specificMenu);
                    Fl_Menu_Button *methods = new Fl_Menu_Button(X+10+110, Y+H/4+2+4, 100, 20, "Methods");
                    methods->labelsize(12);
                    methods->textsize(12);
                    methods->menu(methodsMenu);
                    Fl_Menu_Button *attributes = new Fl_Menu_Button(X+10+220, Y+H/4+2+4, 100, 20, "Attributes");
                    attributes->labelsize(12);
                    attributes->textsize(12);
                    attributes->menu(attributesMenu);
                    Fl_Box *space = new Fl_Box(X+10+320, Y+H/4+2+4, (X+W-10-150)-(X+10+320), 20);
                    space->box(FL_NO_BOX);
                    slotMenus->resizable(space);
                    Fl_Button *apply = new Fl_Button(X+W-10-150, Y+H/4+2+4, 70, 20, "Apply");
                    apply->labelsize(12);
                    Fl_Button *revert = new Fl_Button(X+W-10-70, Y+H/4+2+4, 70, 20, "Revert");
                    revert->labelsize(12);
                }
                slotMenus->end();
                slotEditor_ = new Fl_Wizard(X, Y+H/4+2+30, W, H-H/4-2-30);
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


//
// End of "$Id$".
//
