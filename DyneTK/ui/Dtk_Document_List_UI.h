//
// "$Id$"
//
// Dtk_Document_List_UI header file for the Dyne Toolkit.
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

#ifndef DTK_DOCUMENT_LIST_UI_H
#define DTK_DOCUMENT_LIST_UI_H

class Dtk_Document_List;

class Fldtk_Document_Browser;
class Fldtk_Document_Tabs;


/**
 * Link the document list with the GUI.
 */
class Dtk_Document_List_UI
{
public:

  /** 
   * Link to the Doc List to UI.
   */
                Dtk_Document_List_UI(Dtk_Document_List*);

  /** 
   * Unlink Doc List from UI.
   */
                ~Dtk_Document_List_UI();

private:

  /// take resposibility for all user events form the document browser
  static void documentBbrowser_cb(Fldtk_Document_Browser*, Dtk_Document_List_UI*);
  
  /// back reference to document list
  Dtk_Document_List * list_;
  
  /// this browser lists all documents in the project
  Fldtk_Document_Browser * documentBrowser_;
  
  /// this Tab group contains all opened documents
  Fldtk_Document_Tabs * documentTabs_;
};


#endif

//
// End of "$Id$".
//
