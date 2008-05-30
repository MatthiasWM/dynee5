//
// "$Id$"
//
// Fldtk_Tmpl_Browser header file for the FLMM extension to FLTK.
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

#ifndef FLDTK_TMPL_BROWSER_H
#define FLDTK_TMPL_BROWSER_H


#include <FL/Fl_Hold_Browser.H>


/**
 * A hold browser with the additional ability to remove Templates by
 * pressing "del".
 */
class Fldtk_Tmpl_Browser : public Fl_Hold_Browser
{
public:
				Fldtk_Tmpl_Browser(int X, int Y, int W, int H, const char *L=0L);
    int         handle(int event);
    void        deleteSelectedTemplate();
};


#endif

//
// End of "$Id$".
//
