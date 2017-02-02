//
// DyneTK, the Dyne Toolkit
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

// generated by Fast Light User Interface Designer (fluid) version 1.0300

#ifndef Fldtk_Proj_Settings_h
#define Fldtk_Proj_Settings_h
#include <FL/Fl.H>
#include "fltk/Flmm_Set_Input.h"
#include "fltk/Flmm_Set_Check_Button.h"
#include "fltk/Fldtk_Icon_Dropbox.h"
#include <FL/fl_file_chooser.h>
void set_changed_cb(Fl_Widget*w, void*);
#include <FL/Fl_Group.H>
extern void set_changed_cb(Flmm_Set_Input*, void*);
extern void set_changed_cb(Flmm_Set_Check_Button*, void*);

class Fldtk_Proj_App : public Fl_Group {
public:
  Fldtk_Proj_App(int X, int Y, int W, int H, const char *L = 0);
  Flmm_Set_Input *name;
  Flmm_Set_Input *symbol;
  Flmm_Set_Check_Button *auto_close;
  void updateData();
  void updateDialog();
};
#include <FL/Fl_Button.H>
extern void set_changed_cb(Fldtk_Icon_Dropbox*, void*);
#include <FL/Fl_Box.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Value_Output.H>

class Fldtk_Proj_Icon : public Fl_Group {
public:
  Fldtk_Proj_Icon(int X, int Y, int W, int H, const char *L = 0);
private:
  void cb_Browse_i(Fl_Button*, void*);
  static void cb_Browse(Fl_Button*, void*);
public:
  Fldtk_Icon_Dropbox *wIcon1;
private:
  void cb_Browse1_i(Fl_Button*, void*);
  static void cb_Browse1(Fl_Button*, void*);
public:
  Fldtk_Icon_Dropbox *wMask;
  void updateData();
  void updateDialog();
};
#include <FL/Fl_Check_Button.H>
extern void set_changed_cb(Fl_Check_Button*, void*);

class Fldtk_Proj_Package : public Fl_Group {
public:
  Fldtk_Proj_Package(int X, int Y, int W, int H, const char *L = 0);
  Flmm_Set_Input *name;
  Flmm_Set_Check_Button *deleteOnDownload;
  Flmm_Set_Input *copyright;
  Flmm_Set_Input *version;
  void updateData();
  void updateDialog();
};
#include <FL/Fl_Choice.H>
#include <FL/Fl_Input.H>

class Fldtk_Proj_Project : public Fl_Group {
public:
  Fldtk_Proj_Project(int X, int Y, int W, int H, const char *L = 0);
  static Fl_Menu_Item menu_Platform[];
};
#include <FL/Fl_Text_Editor.H>

class Fldtk_Proj_Output : public Fl_Group {
public:
  Fldtk_Proj_Output(int X, int Y, int W, int H, const char *L = 0);
};
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tabs.H>

class Fldtk_Proj_Settings : public Fl_Double_Window {
  void _Fldtk_Proj_Settings();
public:
  Fldtk_Proj_Settings(int X, int Y, int W, int H, const char *L = 0);
  Fldtk_Proj_Settings(int W, int H, const char *L = 0);
  Fldtk_Proj_App *app;
  Fldtk_Proj_Icon *icon;
  Fldtk_Proj_Package *package;
  Fldtk_Proj_Project *project;
  Fldtk_Proj_Output *output;
  Fl_Button *wOK;
private:
  void cb_wOK_i(Fl_Button*, void*);
  static void cb_wOK(Fl_Button*, void*);
public:
  Fl_Button *wCancel;
private:
  void cb_wCancel_i(Fl_Button*, void*);
  static void cb_wCancel(Fl_Button*, void*);
public:
  Fl_Button *wApply;
private:
  void cb_wApply_i(Fl_Button*, void*);
  static void cb_wApply(Fl_Button*, void*);
public:
  Fldtk_Proj_Settings();
  void updateData();
  void updateDialog();
  void set_changed();
};
#endif

//
// DyneTK, the Dyne Toolkit
// Copyright (C) 2007 Matthias Melcher
//