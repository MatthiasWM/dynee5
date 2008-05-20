//
// DyneTK, The Dyne Toolkit
// Copyright (C) 2007 Matthias Melcher
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

// generated by Fast Light User Interface Designer (fluid) version 1.0108

#ifndef main_ui_h
#define main_ui_h
#include <FL/Fl.H>
#include "fltk/Flio_Inspector.h"
#include "fltk/Flmm_Console.h"
#include "fluid/Fldtk_Inspector.h"
#include "fltk/Fldtk_Document_Tabs.h"
#include "fltk/Fldtk_Document_Browser.h"
#include <FL/fl_ask.h>
#include "main.h"
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Tabs.H>

class Fldtk_Main_Window : public Fl_Double_Window {
  void _Fldtk_Main_Window();
public:
  Fldtk_Main_Window(int X, int Y, int W, int H, const char *L = 0);
  Fldtk_Main_Window(int W, int H, const char *L = 0);
  static Fl_Menu_Item menu_[];
  static Fl_Menu_Item *mFile;
  static Fl_Menu_Item *mFileNewLayout;
private:
  void cb_mFileNewLayout_i(Fl_Menu_*, void*);
  static void cb_mFileNewLayout(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mFileNewText;
private:
  void cb_mFileNewText_i(Fl_Menu_*, void*);
  static void cb_mFileNewText(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mFileOpen;
private:
  void cb_mFileOpen_i(Fl_Menu_*, void*);
  static void cb_mFileOpen(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mFileClose;
private:
  void cb_mFileClose_i(Fl_Menu_*, void*);
  static void cb_mFileClose(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mFileSave;
private:
  void cb_mFileSave_i(Fl_Menu_*, void*);
  static void cb_mFileSave(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mFileSaveAs;
private:
  void cb_mFileSaveAs_i(Fl_Menu_*, void*);
  static void cb_mFileSaveAs(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mFileSaveAll;
private:
  void cb_mFileSaveAll_i(Fl_Menu_*, void*);
  static void cb_mFileSaveAll(Fl_Menu_*, void*);
  void cb_mPrevProj_i(Fl_Menu_*, void*);
  static void cb_mPrevProj(Fl_Menu_*, void*);
  void cb_mPrevProj1_i(Fl_Menu_*, void*);
  static void cb_mPrevProj1(Fl_Menu_*, void*);
  void cb_mPrevProj2_i(Fl_Menu_*, void*);
  static void cb_mPrevProj2(Fl_Menu_*, void*);
  void cb_mPrevProj3_i(Fl_Menu_*, void*);
  static void cb_mPrevProj3(Fl_Menu_*, void*);
  void cb_mPrevProj4_i(Fl_Menu_*, void*);
  static void cb_mPrevProj4(Fl_Menu_*, void*);
  void cb_mPrevProj5_i(Fl_Menu_*, void*);
  static void cb_mPrevProj5(Fl_Menu_*, void*);
  void cb_mPrevProj6_i(Fl_Menu_*, void*);
  static void cb_mPrevProj6(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mPrevProj[8];
private:
  void cb_mPrevProj7_i(Fl_Menu_*, void*);
  static void cb_mPrevProj7(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mFileExit;
private:
  void cb_mFileExit_i(Fl_Menu_*, void*);
  static void cb_mFileExit(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mEdit;
  static Fl_Menu_Item *mEditNewtScreenshot;
private:
  void cb_mEditNewtScreenshot_i(Fl_Menu_*, void*);
  static void cb_mEditNewtScreenshot(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mEditPreferences;
private:
  void cb_mEditPreferences_i(Fl_Menu_*, void*);
  static void cb_mEditPreferences(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mProject;
  static Fl_Menu_Item *mProjectNew;
private:
  void cb_mProjectNew_i(Fl_Menu_*, void*);
  static void cb_mProjectNew(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mProjectOpen;
private:
  void cb_mProjectOpen_i(Fl_Menu_*, void*);
  static void cb_mProjectOpen(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mProjectSave;
private:
  void cb_mProjectSave_i(Fl_Menu_*, void*);
  static void cb_mProjectSave(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mProjectSaveAs;
private:
  void cb_mProjectSaveAs_i(Fl_Menu_*, void*);
  static void cb_mProjectSaveAs(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mProjectClose;
private:
  void cb_mProjectClose_i(Fl_Menu_*, void*);
  static void cb_mProjectClose(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mProjectAddWindow;
private:
  void cb_mProjectAddWindow_i(Fl_Menu_*, void*);
  static void cb_mProjectAddWindow(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mProjectAddFile;
private:
  void cb_mProjectAddFile_i(Fl_Menu_*, void*);
  static void cb_mProjectAddFile(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mProjectRemoveFile;
private:
  void cb_mProjectRemoveFile_i(Fl_Menu_*, void*);
  static void cb_mProjectRemoveFile(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mProjectBuild;
private:
  void cb_mProjectBuild_i(Fl_Menu_*, void*);
  static void cb_mProjectBuild(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mProjectDownload;
private:
  void cb_mProjectDownload_i(Fl_Menu_*, void*);
  static void cb_mProjectDownload(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mProjectLaunch;
private:
  void cb_mProjectLaunch_i(Fl_Menu_*, void*);
  static void cb_mProjectLaunch(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mProjectExportToText;
private:
  void cb_mProjectExportToText_i(Fl_Menu_*, void*);
  static void cb_mProjectExportToText(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mProjectMainLayout;
private:
  void cb_mProjectMainLayout_i(Fl_Menu_*, void*);
  static void cb_mProjectMainLayout(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mProjectSettings;
private:
  void cb_mProjectSettings_i(Fl_Menu_*, void*);
  static void cb_mProjectSettings(Fl_Menu_*, void*);
  void cb_Dump_i(Fl_Menu_*, void*);
  static void cb_Dump(Fl_Menu_*, void*);
  void cb_Dump1_i(Fl_Menu_*, void*);
  static void cb_Dump1(Fl_Menu_*, void*);
  void cb_Dump2_i(Fl_Menu_*, void*);
  static void cb_Dump2(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mLayout;
  static Fl_Menu_Item *mBrowser;
  static Fl_Menu_Item *mBrowserTemplateInfo;
private:
  void cb_mBrowserTemplateInfo_i(Fl_Menu_*, void*);
  static void cb_mBrowserTemplateInfo(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mBrowserRenameSlot;
private:
  void cb_mBrowserRenameSlot_i(Fl_Menu_*, void*);
  static void cb_mBrowserRenameSlot(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mWindow;
  static Fl_Menu_Item *mWindowConnectInspector;
private:
  void cb_mWindowConnectInspector_i(Fl_Menu_*, void*);
  static void cb_mWindowConnectInspector(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mWindowOpenLayout;
private:
  void cb_mWindowOpenLayout_i(Fl_Menu_*, void*);
  static void cb_mWindowOpenLayout(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mHelp;
  static Fl_Menu_Item *mHelpAbout;
private:
  void cb_mHelpAbout_i(Fl_Menu_*, void*);
  static void cb_mHelpAbout(Fl_Menu_*, void*);
public:
  Fl_Button *tFileNewLayout;
private:
  void cb_tFileNewLayout_i(Fl_Button*, void*);
  static void cb_tFileNewLayout(Fl_Button*, void*);
public:
  Fl_Button *tFileNewText;
private:
  void cb_tFileNewText_i(Fl_Button*, void*);
  static void cb_tFileNewText(Fl_Button*, void*);
public:
  Fl_Button *tFileOpen;
private:
  void cb_tFileOpen_i(Fl_Button*, void*);
  static void cb_tFileOpen(Fl_Button*, void*);
public:
  Fl_Button *tFileSave;
private:
  void cb_tFileSave_i(Fl_Button*, void*);
  static void cb_tFileSave(Fl_Button*, void*);
public:
  Fl_Button *tFileSaveAll;
private:
  void cb_tFileSaveAll_i(Fl_Button*, void*);
  static void cb_tFileSaveAll(Fl_Button*, void*);
public:
  Fl_Button *tEditNewtScreenshot;
private:
  void cb_tEditNewtScreenshot_i(Fl_Button*, void*);
  static void cb_tEditNewtScreenshot(Fl_Button*, void*);
public:
  Fl_Button *tProjectBuild;
private:
  void cb_tProjectBuild_i(Fl_Button*, void*);
  static void cb_tProjectBuild(Fl_Button*, void*);
public:
  Fl_Button *tProjectDownload;
private:
  void cb_tProjectDownload_i(Fl_Button*, void*);
  static void cb_tProjectDownload(Fl_Button*, void*);
public:
  Fl_Button *tWindowOpenLayout;
private:
  void cb_tWindowOpenLayout_i(Fl_Button*, void*);
  static void cb_tWindowOpenLayout(Fl_Button*, void*);
  void cb__i(Fl_Input*, void*);
  static void cb_(Fl_Input*, void*);
public:
  Fl_Button *tProjectLaunch;
private:
  void cb_tProjectLaunch_i(Fl_Button*, void*);
  static void cb_tProjectLaunch(Fl_Button*, void*);
public:
  Fl_Button *tLayoutModeEdit;
private:
  void cb_tLayoutModeEdit_i(Fl_Button*, void*);
  static void cb_tLayoutModeEdit(Fl_Button*, void*);
public:
  Fl_Button *tLayoutModeAdd;
private:
  void cb_tLayoutModeAdd_i(Fl_Button*, void*);
  static void cb_tLayoutModeAdd(Fl_Button*, void*);
public:
  Fl_Choice *tTemplateChoice;
private:
  void cb_tTemplateChoice_i(Fl_Choice*, void*);
  static void cb_tTemplateChoice(Fl_Choice*, void*);
  static Fl_Menu_Item menu_1[];
public:
  Fl_Tile *center;
  Fl_Tabs *browsers;
  Fldtk_Document_Browser *documents;
  Fldtk_Document_Tabs *document_tabs;
  Fl_Tabs *consoles;
  Fldtk_Inspector *inspector;
  void activate_menus(unsigned int mask);
};
Fl_Double_Window* create_connect_dialog();
extern Fl_Double_Window *wAboutDialog;
Fl_Double_Window* create_about_dialog();
#endif

//
// Copyright (C) 2007 Matthias Melcher
//
