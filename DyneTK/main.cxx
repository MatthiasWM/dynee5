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
#include <FL/filename.h>

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

#ifdef WIN32
#include "resource.h"
#endif

/*---------------------------------------------------------------------------*/
/**
 * Global variables that must be exterminated or at least renamed.
 */
Fldtk_Prefs *dtkPrefs;
Fldtk_Proj_Settings *dtkProjSettings;
Fldtk_Main_Window *dtkMain;

Flio_Inspector *wInspectorSerial; 
Flmm_Console *    wConsole; 
Fl_Button *wInspectorConnect; 

Fl_Window *wConnect;
Fl_Image *toolbox_open_pixmap;
Fl_Image *toolbox_closed_pixmap;

Fldtk_Document_Browser  * dtkDocumentBrowser;
Fldtk_Document_Tabs     * dtkDocumentTabs;
Fl_Tabs                 * dtkBrowserTabs;
Fldtk_New_Slot_Dialog   * dtkNewSlotDialog;
Fldtk_Declare_To_Dialog * dtkDeclareToDialog;

// global dtk project and document roots
Dtk_Project         * dtkProject;
Dtk_Document_List   * dtkGlobalDocuments;
Dtk_Platform        * dtkPlatform;


/*---------------------------------------------------------------------------*/
extern nps_env_t nps_env;
/**
 * Output message from the compiler.
 * \todo	Move this where it belongs and be a bit more verbose.
 */
extern "C" void yyerror(char * s)
{
  /*
  bool err = 1;
	if (s[0]=='W' && s[1]==':')
    err = 0; // it is only a warning

  if (err)
    nps_env.numerrs++;
  else
    nps_env.numwarns++;
  */
	if (nps_env.fname != NULL)
		InspectorPrintf("\"%s\" ", nps_env.fname);
  InspectorPrintf("line %d: %s:\n%s\n", nps_env.lineno, s, nps_env.linebuf);
  InspectorPrintf("%*s\n", nps_env.tokenpos - nps_env.yyleng + 1, "^");
}


static void update_menus_cb(Fl_Widget*, void*)
{
  UpdateMainMenu();
}


newtRef nsMakeBinaryFromHex(newtRefArg rcvr, newtRefArg nHexStr, newtRefArg nSym)
{
  if (!NewtRefIsString(nHexStr)) {
    return kNewtRefNIL;
  }
  if (!NewtRefIsSymbol(nSym)) {
    return kNewtRefNIL;
  }
  const char *hexStr = NewtRefToString(nHexStr), *src = hexStr;
  char c;
  if (hexStr && *hexStr) {
    uint32_t i, size = strlen(hexStr)/2;
    uint8_t *data = (uint8_t*)malloc(size), *dst = data, v;
    for (i=0; i<size; i++) {
      c = *src++;
      if (c>='a') v = c+10-'a';
      else if (c>='A') v = c+10-'A';
      else v = c-'0';
      c = *src++;
      if (c>='a') v = v*16 + (c+10-'a');
      else if (c>='A') v = v*16 + (c+10-'A');
      else v = v*16 + (c-'0');
      *dst++ = v;
    }
    newtRef ret = NewtMakeBinary(nSym, data, size, true); // true means: copy the array over!
    free(data);
    return ret;
  } else {
    return kNewtRefNIL;
  }
}

newtRef nsAddStepForm(newtRefArg rcvr, newtRefArg parentTemplate, newtRefArg childTemplate)
{
  if (!NewtRefIsFrame(parentTemplate)) {
    return kNewtRefNIL;
  }
  if (!NewtRefIsFrame(childTemplate)) {
    return kNewtRefNIL;
  }
  newtObjRef pt = NewtRefToPointer(parentTemplate);

  // find the 'stepChild array in the parent template
  newtRef stepChildren;
  int ix = NewtFindSlotIndex(parentTemplate, NSSYM(stepChildren));
  if (ix==-1) {
    stepChildren = NewtMakeArray(NSSYM(stepChildren), 0);
    NewtObjSetSlot(pt, NSSYM(stepChildren), stepChildren);
  } else {
    stepChildren = NewtSlotsGetSlot(parentTemplate, ix);
  }

  // add the child template to it
  newtObjRef sc = NewtRefToPointer(stepChildren);
  NewtObjAddArraySlot(sc, childTemplate);

  return kNewtRefNIL;
}

newtRef nsStepDeclare(newtRefArg rcvr, newtRefArg parentTemplate, newtRefArg childTemplate, newtRefArg childSymbol)
{
  if (!NewtRefIsFrame(parentTemplate)) {
    return kNewtRefNIL;
  }
  if (!NewtRefIsFrame(childTemplate)) {
    return kNewtRefNIL;
  }
  if (!NewtRefIsSymbol(childSymbol)) {
    return kNewtRefNIL;
  }
  newtObjRef pt = NewtRefToPointer(parentTemplate);

  // add a slot with the symbol to the parent and set it to nil
  NewtObjSetSlot(pt, childSymbol, kNewtRefNIL);

  // find the 'stepAllocateContext array in the parent template
  newtRef stepAllocateContext;
  int ix = NewtFindSlotIndex(parentTemplate, NSSYM(stepAllocateContext));
  if (ix==-1) {
    stepAllocateContext = NewtMakeArray(NSSYM(stepAllocateContext), 0);
    NewtObjSetSlot(pt, NSSYM(stepAllocateContext), stepAllocateContext);
  } else {
    stepAllocateContext = NewtSlotsGetSlot(parentTemplate, ix);
  }

  // add the symbol and the child template to it
  newtObjRef sc = NewtRefToPointer(stepAllocateContext);
  NewtObjAddArraySlot(sc, childSymbol);
  NewtObjAddArraySlot(sc, childTemplate);

//NewtPrintObject(stdout, parentTemplate);
  return kNewtRefNIL;

//  NewtObjSetSlot(np, NSSYM(__ntId), NewtMakeSymbol(id()));
/*
  if (!NewtRefIsString(nHexStr)) {
    return kNewtRefNIL;
  }
  if (!NewtRefIsSymbol(nSym)) {
    return kNewtRefNIL;
  }
  const char *hexStr = NewtRefToString(nHexStr), *src = hexStr;
  char c;
  if (hexStr && *hexStr) {
    uint32_t i, size = strlen(hexStr)/2;
    uint8_t *data = (uint8_t*)malloc(size), *dst = data, v;
    for (i=0; i<size; i++) {
      c = *src++;
      if (c>='a') v = c+10-'a';
      else if (c>='A') v = c+10-'A';
      else v = c-'0';
      c = *src++;
      if (c>='a') v = v*16 + (c+10-'a');
      else if (c>='A') v = v*16 + (c+10-'A');
      else v = v*16 + (c-'0');
      *dst++ = v;
    }
    newtRef ret = NewtMakeBinary(nSym, data, size, true); // true means: copy the array over!
    free(data);
    return ret;
  } else {
    return kNewtRefNIL;
  }
  */
    return kNewtRefNIL;
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
  
#ifdef USING_APPLE_BUNDLE
  argc = 1;
  {
    char currpath[2048];
    getcwd(currpath, 2048);
    if (currpath && strcmp(currpath, "/")==0) {
      char *homepath = getenv("HOME");
      if (homepath)
        chdir(homepath);
    }
  }
#endif
  
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
  fl_register_images();
  
	toolbox_open_pixmap = new Fl_Pixmap(toolbox_open_xpm);
	toolbox_closed_pixmap = new Fl_Pixmap(toolbox_closed_xpm);
  
	// initialize the local interpreter and compiler
	NewtInit(argc, (const char**)argv, 0);
	NcSetGlobalVar(NSSYM(printLength), NSINT(9999));
	NcSetGlobalVar(NSSYM(printDepth), NSINT(30));
  NewtDefGlobalFunc(NSSYM(MakeBinaryFromHex), (void*)nsMakeBinaryFromHex, 2, "MakeBinaryFromHex(hexString, class)");
  NewtDefGlobalFunc(NSSYM(AddStepForm), (void*)nsAddStepForm, 2, "AddStepForm(parentTemplate, childTemplate)");
  NewtDefGlobalFunc(NSSYM(StepDeclare), (void*)nsStepDeclare, 3, "StepDeclare(parentTemplate, childTemplate, childSymbol)");
  // GetSoundFrame   MakeBinaryFromHex   MakeDitheredPattern 
  // MakeExtrasIcons   MakePixFamily   UnpackRGB
	NEWT_INDENT = 1;
	NEWT_DUMPBC = 1;
  NEWT_MODE_NOS2 = true;
  
	// create some global classes 
	dtkGlobalDocuments = new Dtk_Document_List();
  
	// create various dialog panels
	dtkPrefs = new Fldtk_Prefs();
	dtkProjSettings = new Fldtk_Proj_Settings();
  
  // load the standard platform file.
#ifdef USING_APPLE_BUNDLE
  char ptf_buf[2048], const_buf[2048];
  CFStringRef str;
  CFURLRef    seagullURL;
  CFBundleRef mainBundle = CFBundleGetMainBundle();
  // find the path to the platform interface file
  seagullURL = CFBundleCopyResourceURL( mainBundle, CFSTR("Newton21"), CFSTR("ptf"), NULL );
  str = CFURLCopyFileSystemPath(seagullURL, kCFURLPOSIXPathStyle);
  CFStringGetCString(str, ptf_buf, 2048, 0);
  // find the path to the platform constants file
  seagullURL = CFBundleCopyResourceURL( mainBundle, CFSTR("Newton21"), CFSTR("txt"), NULL );
  str = CFURLCopyFileSystemPath(seagullURL, kCFURLPOSIXPathStyle);
  CFStringGetCString(str, const_buf, 2048, 0);
  // create the platform environment
  dtkPlatform = new Dtk_Platform(ptf_buf, const_buf);
#else
  char ptf_buf[2048], const_buf[2048];
  // find the path to the platform interface file
  fl_filename_absolute(ptf_buf, 2047, "platfrms/Newton21.ptf");
  // find the path to the platform constants file
  fl_filename_absolute(const_buf, 2047, "platfrms/Newton21.txt");
  // create the platform environment
  dtkPlatform = new Dtk_Platform(ptf_buf, const_buf);
#endif
  
	// create the main window
	dtkMain = new Fldtk_Main_Window(785, 595);
#ifdef WIN32
  dtkMain->icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(IDI_ICON1)));
#endif
	int w = dtkPrefs->win_w, h = dtkPrefs->win_h;
	int x = dtkPrefs->win_x, y = dtkPrefs->win_y;
	if (x!=0xDECAFF && y!=0xDECAFF)
		dtkMain->position(x, y);
	if (w!=0xDECAFF && h!=0xDECAFF)
		dtkMain->size(w, h);
  
	// link the functional elements to the visual elements
	dtkDocumentBrowser = dtkMain->wDocumentBrowser;
  dtkDocumentTabs = dtkMain->wDocumentTabs;
  dtkDocumentTabs->callback(update_menus_cb);
  dtkBrowserTabs = dtkMain->wBrowserTabs;
  
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

char *dtkReleaseNotes =
  "<html><head>DyneTK Release Notes</head><body>\n"
  "<h2>DyneTK Release Notes</h2>\n"
  "<h3>DyneTK 1.7.3</h3>\n"
  "<ul>"
  "<li>fixed bytecode generation bug in Newt/0</li>\n"
  "<li>added NewtonScript functions to correctly build view template inheritance</li>\n"
  "<li>added \"Declare To\" dialog and underlay</li>\n"
  "</ul>\n"
  "<h3>DyneTK 1.7.2</h3>\n"
  "<ul>"
  "<li>added script and package support for Einstein on OS X and MSWindows</li>\n"
  "<li>added <b>New Slot</b> dialog box which allows the addition of slots with many types</li>\n"
  "<li>added b&w application icons with mask</li>\n"
  "<li>fixed deleting Templates</li>\n"
  "<li>fixed deleting Slots</li>\n"
  "<li>fixed resizing of Slot editors</li>\n"
  "</ul>\n"
  "<h3>DyneTK 1.7.1</h3>\n"
  "<ul>\n"
  "<li>added smarter paths presets for file choosers</li>\n"
  "<li>fixed wrong handshake for MSWindows serial connection</li>\n"
  "<li>fixed missing handshake on OS X serial connection</li>\n"
  "<li>added compile-time function <tt>MakeBinaryFromHex</tt></li>\n"
  "<li>added application icon</li>\n"
  "<li>added function to delete templates</li>\n"
  "<li>added function to delete slots</li>\n"
  "<li>added automatic file extension</li>\n"
  "<li>fixed Previous Project menu</li>\n"
  "<li>fixed syntax highlighting of complex symbols</li>\n"
  "</ul>"
  "<h3>DyneTK 1.7.0</h3>\n"
  "<ul>\n"
  "<li>initial public release</li>\n"
  "</ul>\n"
  "</body></html>";

// TODOs
// Download Pkg is active even if no project is loaded:
//   When clicked, it gives the user a file chooser to select any package
// Download Pkg is active even if there is no inspector connection. 
//   If clicked, it pops up the Inspector dialog
// When creating a new project, after choosing a filename, the project is saved immediately.

//
// End of "$Id$".
//
