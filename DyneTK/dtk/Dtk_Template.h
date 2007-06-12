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

private:

    /// we must be part of a single layout
    Dtk_Layout_Document * layout_;

    /// every template except the root is a member of exactly one list
    Dtk_Template_List   * list_;

    /// a template can contain a list of templates to form a tree.
    Dtk_Template_List   * tmplList_;

};


#endif

//
// End of "$Id$".
//
