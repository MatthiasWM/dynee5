//
// "$Id$"
//
// DyneTK header file
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
// Please report all bugs and problems to "dyne@matthiasm.com".
//


#ifndef _MAIN_AS_H_
#define _MAIN_AS_H_

#include "globals.h"

// FIXME: move these into the correct files
extern void send_test(int);
extern void preferences_cb(class Fl_Menu_*, void*);
extern void quit_cb(class Fl_Menu_*, void*);

// global references to user interface elements
// FIXME: sort this out and keep only the minimum
extern class Fl_Button          * wInspectorConnect; 
extern class Fldtk_Prefs        * dtkPrefs;
extern class Fldtk_Proj_Settings * dtkProjSettings;
extern class Fldtk_Main_Window  * dtkMain;
extern class Flio_Inspector     * wInspectorSerial; 
extern class Flmm_Console       * wConsole; 

extern class Fldtk_Document_Browser * dtkDocumentBrowser;
extern class Fldtk_Document_Tabs    * dtkDocumentTabs;
extern class Fl_Tabs                * dtkBrowserTabs;

// global dtk project and document roots
extern class Dtk_Document_List  * dtkGlobalDocuments;
extern class Dtk_Project        * dtkProject;
extern class Dtk_Platform       * dtkPlatform;

extern char *dtkReleaseNotes;

#endif

//
// End of "$Id$".
//
