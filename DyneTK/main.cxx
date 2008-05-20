//
// "$Id$"
//
// DyneTK implementation file
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

#include "main.h"

#include <FL/Fl_Window.H>
#include <FL/Fl_Tooltip.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_XPM_Image.H>

#include "fltk/Flio_Serial_Port.h"

#include "fluid/Fldtk_Prefs.h"
#include "fluid/Fldtk_Proj_Settings.h"
#include "fluid/main_ui.h"

#include "images/toolbox_open.xpm"
#include "images/toolbox_closed.xpm"

#include "dtk/Dtk_Document_List.h"
#include "dtk/Dtk_Platform.h"

#include "allNewt.h"

#include <stdio.h>


/*---------------------------------------------------------------------------*/
/**
 * Global variables that must be exterminated or at least renamed.
 */
Fldtk_Prefs *dtkPrefs;
Fldtk_Proj_Settings *dtkProjSettings;
Fldtk_Main_Window *dtkMain;

Flio_Inspector *wInspectorSerial; 
Flmm_Console *wConsole; 
Fl_Button *wInspectorConnect; 

Fl_Window *wConnect;
Fl_Image *toolbox_open_pixmap;
Fl_Image *toolbox_closed_pixmap;

Fldtk_Document_Browser * dtkDocumentBrowser;
Fldtk_Document_Tabs    * dtkDocumentTabs;

// global dtk project and document roots
Dtk_Project         * dtkProject;
Dtk_Document_List   * dtkGlobalDocuments;
Dtk_Platform        * dtkPlatform;


/*---------------------------------------------------------------------------*/
/**
 * Output message from the compiler.
 * \todo	Move this where it belongs and be a bit more verbose.
 */
extern "C" void yyerror(char * s)
{
	wConsole->insert(" // Error: ");
	wConsole->insert(s);
	wConsole->insert("\n");
}


static void update_menus_cb(Fl_Widget*, void*)
{
    UpdateMainMenu();
}


/*---------------------------------------------------------------------------*/
/**
 * Main entry function.
 * 
 * \param	[in]	number of arguments when launched
 * \param	[inout]	launch arguments
 * \retval	0
 */
int main(int argc, char **argv) {

	// initialize our GUI library
	Fl::lock();
	Fl::scheme("GTK+");
    Fl::get_system_colors();
	Fl_Tooltip::size(11);
    // Set font 16/17 to Epsy Sans (Bold)
    Fl::set_font((Fl_Font)16, (Fl_Font)1);
    Fl::set_font((Fl_Font)17, (Fl_Font)2);
    Fl::set_font((Fl_Font)16, " Nu Sans");
    Fl::set_font((Fl_Font)17, "BNu Sans");
    // Set font 20/21 to Casual (Bold)
    Fl::set_font((Fl_Font)20, (Fl_Font)14);
    Fl::set_font((Fl_Font)21, (Fl_Font)15);
    Fl::set_font((Fl_Font)20, " Nu Casual");
    Fl::set_font((Fl_Font)21, "BNu Casual");
    // font for all messages
	fl_message_font(FL_HELVETICA, 12);

	toolbox_open_pixmap = new Fl_Pixmap(toolbox_open_xpm);
	toolbox_closed_pixmap = new Fl_Pixmap(toolbox_closed_xpm);

	// initialize the local interpreter and compiler
	NewtInit(argc, (const char**)argv, 0);
	NcSetGlobalVar(NSSYM(printLength), NSINT(9999));
	NcSetGlobalVar(NSSYM(printDepth), NSINT(30));
	NEWT_INDENT = 1;
	NEWT_DUMPBC = 1;
    NEWT_MODE_NOS2 = true;

	// create some global classes 
	dtkGlobalDocuments = new Dtk_Document_List();

	// create various dialog panels
	dtkPrefs = new Fldtk_Prefs();
	dtkProjSettings = new Fldtk_Proj_Settings();

    // load the standard platform file.
    dtkPlatform = new Dtk_Platform("NEWTON21.PTF");

	// create the main window
	dtkMain = new Fldtk_Main_Window(785, 595);
	int w = dtkPrefs->win_w, h = dtkPrefs->win_h;
	int x = dtkPrefs->win_x, y = dtkPrefs->win_y;
	if (x!=0xDECAFF && y!=0xDECAFF)
		dtkMain->position(x, y);
	if (w!=0xDECAFF && h!=0xDECAFF)
		dtkMain->size(w, h);

	// link the functional elements to the visual elements
	dtkDocumentBrowser = dtkMain->documents;
    dtkDocumentTabs = dtkMain->document_tabs;
    dtkDocumentTabs->callback(update_menus_cb);

	UpdatePrevProjMenu();

	// launch the application
	dtkMain->show(/*argc*/1, argv);
	if (argc==2) {
		Fl::flush();
		OpenProject(argv[1]);
    }

	Fl::run();

	// clean the virtual machine
	NewtCleanup();

	dtkPrefs->win_x = dtkMain->x();
	dtkPrefs->win_y = dtkMain->y();
	dtkPrefs->win_w = dtkMain->w();
	dtkPrefs->win_h = dtkMain->h();
	dtkPrefs->set_prefs();

	return 0;
}


// TODOs
// Download Pkg is active even if no project is loaded:
//   When clicked, it gives the user a file chooser to select any package
// Download Pkg is active even if there is no inspector connection. 
//   If clicked, it pops up the Inspector dialog
// When creating a new project, after choosing a filename, the project is saved immediately.

//
// End of "$Id$".
//
