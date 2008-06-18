//
// "$Id$"
//
// Fldtk_Editor implementation for the FLMM extension to FLTK.
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


#include "Fldtk_Editor.h"
#include "Fldtk_Document_Tabs.h"
#include "main.h"


/*---------------------------------------------------------------------------*/
Fldtk_Editor::Fldtk_Editor(const char *name)
:   Fl_Group(dtkDocumentTabs->x(), dtkDocumentTabs->y()+25, 
             dtkDocumentTabs->w(), dtkDocumentTabs->h()-25)
{
    if (name)
        copy_label(name);
	labelsize(12);
    end();
}


/*---------------------------------------------------------------------------*/
Fldtk_Editor::~Fldtk_Editor()
{
}


/*---------------------------------------------------------------------------*/
void Fldtk_Editor::updateName(const char *name) 
{
	copy_label(name);
}

//
// End of "$Id$".
//
