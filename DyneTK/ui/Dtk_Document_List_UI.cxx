//
// "$Id$"
//
// Dtk_Document_List_UI implementation for the Dyne Toolkit.
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


#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include "Dtk_Document_List_UI.h"
#include "fluid/main_ui.h"


/*---------------------------------------------------------------------------*/
Dtk_Document_List_UI::Dtk_Document_List_UI(Dtk_Document_List *list)
: list_(list),
  documentBrowser_(dtkDocumentBrowser),
  documentTabs_(dtkDocumentTabs)
{
  // FIXME add the browser callback here
  // FIXME add the tabs callback here
}


/*---------------------------------------------------------------------------*/
Dtk_Document_List_UI::~Dtk_Document_List_UI()
{
}

/*---------------------------------------------------------------------------*/
void Dtk_Document_List_UI::documentBbrowser_cb(Fldtk_Document_Browser *brsr, Dtk_Document_List_UI *This)
{
  
}

/*
void Dtk_Document_List::browser_cb(Fldtk_Document_Browser *w, Dtk_Document_List *d)
{
  int it = w->value();
  if (it==0)
    return;
  Dtk_Document *doc = (Dtk_Document*)w->data(it);
  if (doc)
    doc->edit();
  UpdateMainMenu();
}
*/

//
// End of "$Id$".
//
