//
// "$Id$"
//
// Fldtk_Document_Tabs implementation for the FLMM extension to FLTK.
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

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include "Fldtk_Document_Tabs.h"
#include "Fldtk_Editor.h"

#include <FL/Fl.H>
#include <FL/fl_draw.h>

#include <stdio.h>
#include <stdlib.h>


Fldtk_Document_Tabs::Fldtk_Document_Tabs(int X, int Y, int W, int H, const char *L)
: Fl_Tabs(X, Y, W, H, L)
{
	labelsize(12);
	selection_color(FL_RED);
}


Fldtk_Document_Tabs::~Fldtk_Document_Tabs()
{
}


void Fldtk_Document_Tabs::draw() 
{
	if (children()) {
		Fl_Tabs::draw();
	} else {
		fl_color(color());
		fl_rect(x(), y(), w(), h());
	}
}


void Fldtk_Document_Tabs::add(Fldtk_Editor *doc)
{
	Fl_Tabs::add(doc);
	doc->resize(x(), y()+25, w(), h()-25);
	resizable(doc);
	redraw();
}


//
// End of "$Id$".
//
