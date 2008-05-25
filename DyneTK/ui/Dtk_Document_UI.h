//
// "$Id$"
//
// Dtk_Document_UI header file for the Dyne Toolkit.
//
// Copyright 2008 by Matthias Melcher.
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

#ifndef DTK_DOCUMENT_UI_H
#define DTK_DOCUMENT_UI_H


class Dtk_Document_UI;

class Dtk_Document;
class Fldtk_Document_Browser;
class Fldtk_Document_Tabs;


class Dtk_Document_UI
{
public:
  Dtk_Document_UI(Dtk_Document*);
  ~Dtk_Document_UI();
protected:
  
  /// The document that we link with the UI.
  Dtk_Document *document_;

  /// this browser lists all documents in the project
  Fldtk_Document_Browser * documentBrowser_;
  
  /// this Tab group contains all opened documents
  Fldtk_Document_Tabs * documentTabs_;
};


#endif

//
// End of "$Id$".
//
