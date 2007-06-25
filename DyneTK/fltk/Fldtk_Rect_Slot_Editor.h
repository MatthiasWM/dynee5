//
// "$Id$"
//
// Fldtk_Rect_Slot_Editor header file for the FLMM extension to FLTK.
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
// Please report all bugs and problems to "flmm@matthiasm.com".
//

#ifndef FLDTK_RECT_SLOT_EDITOR_H
#define FLDTK_RECT_SLOT_EDITOR_H


#include <FL/Fl_Group.H>


class Dtk_Rect_Slot;
class Fl_Input;
class Fl_Box;


/** GUI for editing rectangle slots.
 *
 * A rectangle remembers values for the top left and the bottom right
 * coordinates. This editor does not do any additional processing.
 * The actual coordinates of a template depend on additional values
 * including 'viewJustify and possibly the position of the parent
 * and sibling.
 */
class Fldtk_Rect_Slot_Editor : public Fl_Group
{
public:

    /** Create an editor for rectangles.
     *
     * The rect editor has four integer input fields for the 
     * coordinates of two corners of the rectangle. It also 
     * calculates and show the height and width of the rectangle.
     *
     * \param container This widget will hold the editor
     * \param slot      This is the slot that we can edit
     */
	        Fldtk_Rect_Slot_Editor(Fl_Group *container, Dtk_Rect_Slot *slot);

    /** Release all alocated resources.
     */
	        ~Fldtk_Rect_Slot_Editor();

    /** Set new rectangle coordinates.
     *
     * \param top top edge of rectangle
     * \param left left edge
     * \param bottom bottom edge
     * \param right right edge
     */
    void    setRect(int top, int left, int bottom, int right);

    /** Get the current rectangle coordinates.
     *
     * \param top top edge of rectangle
     * \param left left edge
     * \param bottom bottom edge
     * \param right right edge
     */
    void    getRect(int &top, int &left, int &bottom, int &right);

protected:
    /// back pointer to slot
    Dtk_Rect_Slot * slot_;

    /// input fields for coordinates
    Fl_Input    *wTop_, *wLeft_, *wBottom_, *wRight_;

    /// output fields for calculated size
    Fl_Box      *wWidth_, *wHeight_;

    static void editor_cb(Fldtk_Rect_Slot_Editor *w, unsigned int cmd);

    static void update_cb(Fl_Widget*, Fldtk_Rect_Slot_Editor*);
};


#endif

//
// End of "$Id$".
//
