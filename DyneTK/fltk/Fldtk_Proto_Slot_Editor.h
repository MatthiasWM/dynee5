//
// "$Id$"
//
// Fldtk_Proto_Slot_Editor header file for the FLMM extension to FLTK.
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

#ifndef FLDTK_PROTO_SLOT_EDITOR_H
#define FLDTK_PROTO_SLOT_EDITOR_H


#include <FL/Fl_Group.H>


class Dtk_Proto_Slot;
class Fl_Choice;


/** GUI for editing prototype slots.
 *
 * This editor is not yet complete. For the time being, it allows
 * the user to choose the _proto magic pointer from a list of
 * prototypes found in the Platform files.
 *
 * \todo implement support for user protos
 * \todo implement support for viewClass derivation
 */
class Fldtk_Proto_Slot_Editor : public Fl_Group
{
public:

    /** Create a _proto slot editor.
     *
     * This editor contains a pulldown of all available protos.
     *
     * \param container This widget will hold the editor
     * \param slot      This is the slot that we can edit
     */
	                Fldtk_Proto_Slot_Editor(Fl_Group *container, Dtk_Proto_Slot *slot);

    /** Return all allocated resources to the system.
     */
    virtual         ~Fldtk_Proto_Slot_Editor();

    /** Set the proto using a cString ID.
     *
     * This function find the proto in the list of available protos 
     * and sets the pulldown menu item accordingly.
     *
     * \param id CString containing the name of the proto.
     *
     * \todo We have no good plan if the given proto doesn't exist.
     */
    virtual void    value(char *id);

    /** Return the proto as a cString.
     *
     * You can use Dtk_Platform::findProto to retrieve the 
     * magic pointer for this proto string.
     *
     * \return pointer to temporary string
     */
    virtual char    * value();

protected:

    /// back pointer to the slot
    Dtk_Proto_Slot  * slot_;

    /// GUI element to select a proto
    Fl_Choice       * wProto_;

    static void editor_cb(Fldtk_Proto_Slot_Editor *w, unsigned int cmd);
};


#endif

//
// End of "$Id$".
//
