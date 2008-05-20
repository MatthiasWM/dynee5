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
class Fl_Menu_Button;
class Dtk_Layout_Document;
class Dtk_Template;
class Fldtk_Slot_Editor_Group;
class Fldtk_Slot_Browser;
class Fldtk_Tmpl_Browser;


/** GUI element for editing layouts.
 */
class Fldtk_Layout_Editor : public Fldtk_Editor
{
public:
	                        Fldtk_Layout_Editor(Dtk_Layout_Document *layout);
	virtual                 ~Fldtk_Layout_Editor();
    virtual Dtk_Document    * document();
    Fldtk_Tmpl_Browser      * templateBrowser() { return tmplBrowser_; }
    Fldtk_Slot_Browser      * slotBrowser() { return slotBrowser_; }
    Fldtk_Slot_Editor_Group * slotEditor() { return slotEditor_; }
    void                    userChangedSlots();
    void                    userDeselectedTemplates();
    void                    userSelectedTemplate(Dtk_Template *tmpl);

protected:
    Dtk_Layout_Document     * layout_;

    /// The currently selected template
    Dtk_Template            * template_;
	Fldtk_Tmpl_Browser      * tmplBrowser_;
	Fldtk_Slot_Browser      * slotBrowser_;
    Fldtk_Slot_Editor_Group * slotEditor_;
    Fl_Menu_Button          * specificChoice_;
    Fl_Menu_Button          * methodsChoice_;
    Fl_Menu_Button          * attributesChoice_;

    // slot menus
    //  specific
    //  methods
    //  attributes
    // apply
    // revert
    // slot editor
    //  ... (many)

    static void apply_cb(Fl_Widget*, Fldtk_Layout_Editor*);
    static void revert_cb(Fl_Widget*, Fldtk_Layout_Editor*);
    static void specific_choice_cb(Fl_Menu_Button*, Fldtk_Layout_Editor*);
    static void methods_choice_cb(Fl_Menu_Button*, Fldtk_Layout_Editor*);
    static void attributes_choice_cb(Fl_Menu_Button*, Fldtk_Layout_Editor*);
};


#endif

//
// End of "$Id$".
//
