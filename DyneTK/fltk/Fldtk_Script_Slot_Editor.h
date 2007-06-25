//
// "$Id$"
//
// Fldtk_Script_Slot_Editor header file for the FLMM extension to FLTK.
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

#ifndef FLDTK_SCRIPT_SLOT_EDITOR_H
#define FLDTK_SCRIPT_SLOT_EDITOR_H


#include "fltk/Flmm_Newt_Script_Editor.h"


class Dtk_Script_Slot;


/** GUI for editing script slots.
 *
 * Script slots add a simple script or a complex function to
 * a template. The script slot editor provides syntax highlighting
 * for the language specific keywords.
 *
 * \todo we could highlight predefined functions
 * \todo we should highlight symbols in ||'s
 * \todo editing functions from the main menu are not working yet
 */
class Fldtk_Script_Slot_Editor : public Flmm_Newt_Script_Editor
{
public:

    /** Create an editor fro script aslots.
     * 
     * This editor is linked to one slot.
     *
     * \param container This widget will hold the editor
     * \param slot      This is the slot that we can edit
     */
	        Fldtk_Script_Slot_Editor(Fl_Group *container, Dtk_Script_Slot *slot);

    /** Free all resources associated with this editor.
     */
	        ~Fldtk_Script_Slot_Editor();

    /** Replace the script in the editor.
     *
     * This creates its own buffer. \a script may be freed after
     * calling this function.
     *
     * \param text New script for this editor.
     */
    void    text(char *script);

    /** Return the script as ist is currently visible in the editor.
     *
     * This function returns a copy of the current script which must
     * be released by calling free().
     *
     * \returns copy of the current script text for this slot.
     */
    char    *text();

protected:

    /// back pointer to script slot
    Dtk_Script_Slot * slot_;

    static void editor_cb(Fldtk_Script_Slot_Editor *w, unsigned int cmd);
};


#endif

//
// End of "$Id$".
//
