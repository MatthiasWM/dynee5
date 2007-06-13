//
// "$Id$"
//
// Fldtk_Layout_Editor header file for the FLMM extension to FLTK.
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

#ifndef FLDTK_LAYOUT_EDITOR_H
#define FLDTK_LAYOUT_EDITOR_H


#include "fltk/Fldtk_Editor.h"


class Fl_Hold_Browser;
class Fl_Wizard;
class Dtk_Layout_Document;


/** GUI element for editing layouts.
 */
class Fldtk_Layout_Editor : public Fldtk_Editor
{
public:
	                        Fldtk_Layout_Editor(Dtk_Layout_Document *layout);
	virtual                 ~Fldtk_Layout_Editor();
    virtual Dtk_Document    * document();
    Fl_Hold_Browser         * templateBrowser() { return tmplBrowser_; }
    Fl_Hold_Browser         * slotBrowser() { return slotBrowser_; }
    Fl_Wizard               * slotEditor() { return slotEditor_; }

protected:
    Dtk_Layout_Document     * layout_;
	Fl_Hold_Browser         * tmplBrowser_;
	Fl_Hold_Browser         * slotBrowser_;
    Fl_Wizard               * slotEditor_;
    // slot menus
    //  specific
    //  methods
    //  attributes
    // apply
    // revert
    // slot editor
    //  ... (many)
};


#endif

//
// End of "$Id$".
//
