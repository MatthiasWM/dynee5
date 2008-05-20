//
// "$Id$"
//
// Fldtk_Tmpl_Browser implementation for the FLMM extension to FLTK.
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


#include "Fldtk_Tmpl_Browser.h"
#include "dtk/Dtk_Template.h"
#include "dtk/Dtk_Layout_Document.h"
#include <FL/Fl.H>


#include <stdlib.h>
#include <string.h>


Fldtk_Tmpl_Browser::Fldtk_Tmpl_Browser(int X, int Y, int W, int H, const char *L)
:	Fl_Hold_Browser(X, Y, W, H, L)
{
}


int Fldtk_Tmpl_Browser::handle(int event) 
{
    switch (event) {
        case FL_SHORTCUT:
            switch (Fl::event_key()) {
                case FL_Delete:
                case FL_BackSpace:
                    deleteSelectedTemplate();
                    return 1;
            }
            break;
    }
    return Fl_Hold_Browser::handle(event);
}


void Fldtk_Tmpl_Browser::deleteSelectedTemplate()
{
    int ix = value();
    if (!ix) 
        return;
    Dtk_Template *tmpl = (Dtk_Template*)data(ix);
    if (!tmpl) 
        return;
    Dtk_Layout_Document *lyt = tmpl->layout();
    lyt->removeTemplate(tmpl);
}


//
// End of "$Id$".
//
