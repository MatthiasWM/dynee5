//
// "$Id$"
//
// Fldtk_Layout_View header file for the FLMM extension to FLTK.
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

#ifndef FLDTK_LAYOUT_VIEW_H
#define FLDTK_LAYOUT_VIEW_H


#include <FL/Fl_Double_Window.H>


class Dtk_Layout_Document;


/** GUI element for visually editing layouts.
 */
class Fldtk_Layout_View : public Fl_Double_Window
{
public:
	                    Fldtk_Layout_View(Dtk_Layout_Document *layout, int width, int height);
	virtual             ~Fldtk_Layout_View();

protected:
    Dtk_Layout_Document * layout_;
};


#endif

//
// End of "$Id$".
//
