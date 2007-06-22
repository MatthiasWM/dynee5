//
// "$Id$"
//
// Dtk_Template_Proto header file for the Dyne Toolkit.
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

#ifndef DTK_TEMPLATE_PROTO_H
#define DTK_TEMPLATE_PROTO_H


struct Fl_Menu_Item;
class Dtk_Template;


typedef void (*FSetup)(Dtk_Template*);

/** A helper class that is used to create new templates.
 */
class Dtk_Template_Proto
{
public:
    static Fl_Menu_Item *menu;

    char    *name;
    int     id;
    FSetup  setup;
    // specific slots
    // methods
    // attributes
};


#endif

//
// End of "$Id$".
//
