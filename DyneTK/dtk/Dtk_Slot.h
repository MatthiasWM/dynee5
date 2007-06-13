//
// "$Id$"
//
// Dtk_Slot header file for the Dyne Toolkit.
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

#ifndef DTK_SLOT_H
#define DTK_SLOT_H


#include <vector>


class Dtk_Slot_List;


/** Manage a slot inside a template.
 */
class Dtk_Slot
{
public:

    /** Initialize a slot
     */
                    Dtk_Slot(Dtk_Slot_List *list);

    /** Remove a slot and unlink it from the list.
     */
                    ~Dtk_Slot();

private:

    /// back reference to the slot list
    Dtk_Slot_List     * list_;
};


#endif

//
// End of "$Id$".
//
