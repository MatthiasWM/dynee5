//
// "$Id$"
//
// Dtk_Rect_Slot header file for the Dyne Toolkit.
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

#ifndef DTK_RECT_SLOT_H
#define DTK_RECT_SLOT_H


#include "Dtk_Slot.h"


class Fldtk_Rect_Slot_Editor;
class Dtk_Script_Writer;


/** Manage a script slot inside a template.
 */
class Dtk_Rect_Slot : public Dtk_Slot
{
public:

    /** Initialize a slot
     */
                    Dtk_Rect_Slot(Dtk_Slot_List *list, const char *key, newtRef slot);

    /** Remove a slot and unlink it from the list.
     */
    virtual         ~Dtk_Rect_Slot();

    /** Pop up the script slot editor.
     */
    virtual void    edit();

    /** Return the coordintaes of the rectangle.
     */
    int     top() { return top_; }
    int     left() { return left_; }
    int     bottom() { return bottom_; }
    int     right() { return right_; }

    /** Write this slot as a Newt Script.
     *
     * \retval 0 if successful
     * \retval negative if an error occured
     */     
	virtual int		write(Dtk_Script_Writer &sw);

private:

    /// this is the editor that we are using
    Fldtk_Rect_Slot_Editor    * editor_;

    /// the rectangle iteslf
    int             top_, left_, bottom_, right_;
};


#endif

//
// End of "$Id$".
//
