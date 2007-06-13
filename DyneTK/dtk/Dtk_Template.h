//
// "$Id$"
//
// Dtk_Template header file for the Dyne Toolkit.
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
// Please report all bugs and problems to "dtk@matthiasm.com".
//

#ifndef DTK_TEMPLATE_H
#define DTK_TEMPLATE_H


class Dtk_Layout_Document;
class Dtk_Template_List;

extern "C" {
#include "NewtType.h"
}


class Fl_Hold_Browser;
class Dtk_Slot_List;


/** Manage a template object in a layout.
 *
 * Templates are arranged as a tree inside a layout.
 * They contain a list of slots which make up the 
 * attributes and signal handling of a template.
 */
class Dtk_Template
{
public:

    /** Initialize a template.
     */
                    Dtk_Template(Dtk_Layout_Document *layout, Dtk_Template_List *list=0L);

    /** Remove a template and all its children.
     */
                    ~Dtk_Template();

    /** Load a template tree starting at the given newtRef
     */
    int             load(newtRef node);

    /** Recursively run through all templates in the tree.
     *
     * Upate all information needed to keep the browser information current.
     */
    void            updateBrowserLink(Fl_Hold_Browser *browser, int &indent, int &index, bool add=false);

    /** Return the name of the template as it will appear in the browser
     */
    const char      * browserName();

    /** Return the associated Layout
     */
    Dtk_Layout_Document * layout() { return layout_; }

    /** This template was selected by the user for editing.
     *
     * We need to update the slot browser and the slot editor.
     */
    void edit();

private:

    /// we must be part of a single layout
    Dtk_Layout_Document * layout_;

    /// every template except the root is a member of exactly one list
    Dtk_Template_List   * list_;

    /// a template can contain a list of templates to form a tree.
    Dtk_Template_List   * tmplList_;

    /// a template can have any number of slots
    Dtk_Slot_List       * slotList_;

    /// index in the browser widget
    int                 index_;

    /// depth within the tree
    int                 indent_;

    /// the browser that lists this template
    Fl_Hold_Browser     * browser_;

    /// name of the template as it appears in the browser view
    char                *browserName_;

    /// dtk name of template
    char                *ntName_;

    /// dtk id of template (compareable to C++ "class")
    char                *ntId_;
};


#endif

//
// End of "$Id$".
//
