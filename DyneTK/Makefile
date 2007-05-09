
all: dynetk

SRCFILES = \
  dtk/Dtk_Document.cxx \
  dtk/Dtk_Script_Document.cxx \
  dtk/Dtk_Project.cxx \
  dtk/Dtk_Document_Manager.cxx \
  dtk/Dtk_Error.cxx \
  dtk/Dtk_Layout_Document.cxx \
  fltk/Flio_Inspector.cxx \
  fltk/Fldtk_Layout_Drop_Box.cxx \
  fltk/Flmm_Set_Input.cxx \
  fltk/Flio_MNP4_Protocol.cxx \
  fltk/Flio_Stream.cxx \
  fltk/Flio_Serial_Port.cxx \
  fltk/Flmm_Set_Check_Button.cxx \
  fltk/Fldtk_Script_Editor.cxx \
  fltk/Flmm_Message.cxx \
  fltk/Fldtk_Document_Browser.cxx \
  fltk/Fldtk_Layout_Editor.cxx \
  fltk/Fldtk_Editor.cxx \
  fltk/Fldtk_Document_Tabs.cxx \
  fltk/Flmm_Console.cxx \
  fluid/Fldtk_Proj_Settings.cxx \
  fluid/main_ui.cxx \
  fluid/Fldtk_Prefs.cxx \
  fluid/Fldtk_Inspector.cxx \
  globals.cxx \
  main.cxx \
  sandbox.cxx \
  nt/Nt_Group.cxx

OBJFILES = $(SRCFILES:.cxx=.o)


CXX := $(shell fltk-config --cxx)

CXXFLAGS := $(shell fltk-config --cxxflags)

INCLUDES := -I. -Idtk -Ifltk -Ifluid -Int

NEWTINCLUDES := -I../../newt0/src/ -I../../newt0/src/newt_core/incs/

LDFLAGS := $(shell fltk-config --ldstaticflags) -L../../newt0/build

LIBS := $(shell fltk-config --libs)


dynetk: $(OBJFILES)
	echo Linking $@...
	$(CXX) -g $(LDFLAGS) -o $@ $(OBJFILES) -lnewt $(LIBS)

.SUFFIXES: .cxx .o

.cxx.o:
	echo Compiling $<...
	$(CXX) -c -g $(CXXFLAGS) $(INCLUDES) $(NEWTINCLUDES) -o $@ $<

clean:
	rm $(OBJFILES)
	rm dynetk

depend:
	makedepend -- $(CXXFLAGS) $(INCLUDES) $(NEWTINCLUDES) -- $(SRCFILES)


# DO NOT DELETE

dtk/Dtk_Document.o: dtk/Dtk_Document.h
dtk/Dtk_Document.o: ../../newt0/src/newt_core/incs/NewtType.h
dtk/Dtk_Document.o: ../../newt0/src/newt_core/incs/platform.h
dtk/Dtk_Document.o: ../../newt0/src/config.h /usr/include/stdint.h
dtk/Dtk_Document.o: /usr/include/features.h /usr/include/sys/cdefs.h
dtk/Dtk_Document.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
dtk/Dtk_Document.o: /usr/include/gnu/stubs-32.h /usr/include/bits/wchar.h
dtk/Dtk_Document.o: /usr/include/stdlib.h /usr/include/sys/types.h
dtk/Dtk_Document.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
dtk/Dtk_Document.o: /usr/include/time.h /usr/include/endian.h
dtk/Dtk_Document.o: /usr/include/bits/endian.h /usr/include/sys/select.h
dtk/Dtk_Document.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
dtk/Dtk_Document.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
dtk/Dtk_Document.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
dtk/Dtk_Document.o: ../../newt0/src/newt_core/incs/NewtConf.h
dtk/Dtk_Document.o: dtk/Dtk_Document_Manager.h dtk/Dtk_Project.h
dtk/Dtk_Document.o: fltk/Fldtk_Editor.h /usr/local/include/FL/Fl_Group.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Widget.H
dtk/Dtk_Document.o: /usr/local/include/FL/Enumerations.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Export.H
dtk/Dtk_Document.o: fltk/Fldtk_Document_Tabs.h
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Tabs.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Group.H fluid/main_ui.h
dtk/Dtk_Document.o: /usr/local/include/FL/Fl.H fltk/Flio_Inspector.h
dtk/Dtk_Document.o: fltk/Flio_MNP4_Serial.h fltk/Flio_Serial_Port.h
dtk/Dtk_Document.o: /usr/local/include/FL/x.H /usr/include/X11/Xlib.h
dtk/Dtk_Document.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
dtk/Dtk_Document.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
dtk/Dtk_Document.o: /usr/include/X11/keysym.h /usr/include/X11/keysymdef.h
dtk/Dtk_Document.o: /usr/include/X11/Xatom.h
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Window.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Box.H fltk/Flmm_Console.h
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Text_Editor.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Text_Display.H
dtk/Dtk_Document.o: /usr/local/include/FL/fl_draw.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Scrollbar.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Slider.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Valuator.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Text_Buffer.H
dtk/Dtk_Document.o: fluid/Fldtk_Inspector.h main.h globals.h
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Button.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Choice.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Menu_.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Menu_Item.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Image.H
dtk/Dtk_Document.o: fltk/Fldtk_Document_Browser.h
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Select_Browser.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Browser.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Browser_.H
dtk/Dtk_Document.o: fltk/Fldtk_Layout_Drop_Box.h
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Window.H nt/Nt_Group.h
dtk/Dtk_Document.o: ../../newt0/src/newt_core/incs/NewtCore.h
dtk/Dtk_Document.o: ../../newt0/src/newt_core/incs/NewtErrs.h
dtk/Dtk_Document.o: ../../newt0/src/newt_core/incs/NewtFns.h
dtk/Dtk_Document.o: ../../newt0/src/newt_core/incs/NewtEnv.h
dtk/Dtk_Document.o: ../../newt0/src/newt_core/incs/NewtMem.h
dtk/Dtk_Document.o: ../../newt0/src/newt_core/incs/NewtObj.h
dtk/Dtk_Document.o: /usr/local/include/FL/fl_ask.h
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Double_Window.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Menu_Bar.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Input.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Input_.H
dtk/Dtk_Document.o: /usr/local/include/FL/Fl_Tile.H
dtk/Dtk_Document.o: /usr/local/include/FL/filename.h /usr/include/dirent.h
dtk/Dtk_Document.o: /usr/include/bits/dirent.h /usr/include/bits/posix1_lim.h
dtk/Dtk_Document.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
dtk/Dtk_Document.o: /usr/include/stdio.h /usr/include/libio.h
dtk/Dtk_Document.o: /usr/include/_G_config.h /usr/include/wchar.h
dtk/Dtk_Document.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
dtk/Dtk_Document.o: /usr/include/bits/sys_errlist.h /usr/include/string.h
dtk/Dtk_Document.o: ../../newt0/src/newt_core/incs/NewtBC.h
dtk/Dtk_Document.o: ../../newt0/src/newt_core/incs/NewtParser.h
dtk/Dtk_Document.o: ../../newt0/src/newt_core/incs/NewtNSOF.h
dtk/Dtk_Document.o: ../../newt0/src/newt_core/incs/NewtPkg.h
dtk/Dtk_Document.o: ../../newt0/src/newt_core/incs/NewtPrint.h
dtk/Dtk_Script_Document.o: dtk/Dtk_Script_Document.h ./dtk/Dtk_Document.h
dtk/Dtk_Script_Document.o: ../../newt0/src/newt_core/incs/NewtType.h
dtk/Dtk_Script_Document.o: ../../newt0/src/newt_core/incs/platform.h
dtk/Dtk_Script_Document.o: ../../newt0/src/config.h /usr/include/stdint.h
dtk/Dtk_Script_Document.o: /usr/include/features.h /usr/include/sys/cdefs.h
dtk/Dtk_Script_Document.o: /usr/include/bits/wordsize.h
dtk/Dtk_Script_Document.o: /usr/include/gnu/stubs.h
dtk/Dtk_Script_Document.o: /usr/include/gnu/stubs-32.h
dtk/Dtk_Script_Document.o: /usr/include/bits/wchar.h /usr/include/stdlib.h
dtk/Dtk_Script_Document.o: /usr/include/sys/types.h /usr/include/bits/types.h
dtk/Dtk_Script_Document.o: /usr/include/bits/typesizes.h /usr/include/time.h
dtk/Dtk_Script_Document.o: /usr/include/endian.h /usr/include/bits/endian.h
dtk/Dtk_Script_Document.o: /usr/include/sys/select.h
dtk/Dtk_Script_Document.o: /usr/include/bits/select.h
dtk/Dtk_Script_Document.o: /usr/include/bits/sigset.h
dtk/Dtk_Script_Document.o: /usr/include/bits/time.h
dtk/Dtk_Script_Document.o: /usr/include/sys/sysmacros.h
dtk/Dtk_Script_Document.o: /usr/include/bits/pthreadtypes.h
dtk/Dtk_Script_Document.o: /usr/include/alloca.h
dtk/Dtk_Script_Document.o: ../../newt0/src/newt_core/incs/NewtConf.h
dtk/Dtk_Script_Document.o: dtk/Dtk_Error.h fltk/Fldtk_Script_Editor.h
dtk/Dtk_Script_Document.o: fltk/Fldtk_Editor.h
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Group.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Widget.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Enumerations.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Export.H
dtk/Dtk_Script_Document.o: fltk/Fldtk_Document_Tabs.h
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Tabs.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Group.H fluid/main_ui.h
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl.H fltk/Flio_Inspector.h
dtk/Dtk_Script_Document.o: fltk/Flio_MNP4_Serial.h fltk/Flio_Serial_Port.h
dtk/Dtk_Script_Document.o: /usr/local/include/FL/x.H /usr/include/X11/Xlib.h
dtk/Dtk_Script_Document.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
dtk/Dtk_Script_Document.o: /usr/include/X11/Xosdefs.h
dtk/Dtk_Script_Document.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
dtk/Dtk_Script_Document.o: /usr/include/X11/keysymdef.h
dtk/Dtk_Script_Document.o: /usr/include/X11/Xatom.h
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Window.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Box.H fltk/Flmm_Console.h
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Text_Editor.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Text_Display.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/fl_draw.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Scrollbar.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Slider.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Valuator.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Text_Buffer.H
dtk/Dtk_Script_Document.o: fluid/Fldtk_Inspector.h main.h globals.h
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Button.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Choice.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Menu_.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Menu_Item.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Image.H
dtk/Dtk_Script_Document.o: fltk/Fldtk_Document_Browser.h
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Select_Browser.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Browser.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Browser_.H
dtk/Dtk_Script_Document.o: fltk/Fldtk_Layout_Drop_Box.h
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Window.H nt/Nt_Group.h
dtk/Dtk_Script_Document.o: ../../newt0/src/newt_core/incs/NewtCore.h
dtk/Dtk_Script_Document.o: ../../newt0/src/newt_core/incs/NewtErrs.h
dtk/Dtk_Script_Document.o: ../../newt0/src/newt_core/incs/NewtFns.h
dtk/Dtk_Script_Document.o: ../../newt0/src/newt_core/incs/NewtEnv.h
dtk/Dtk_Script_Document.o: ../../newt0/src/newt_core/incs/NewtMem.h
dtk/Dtk_Script_Document.o: ../../newt0/src/newt_core/incs/NewtObj.h
dtk/Dtk_Script_Document.o: /usr/local/include/FL/fl_ask.h
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Double_Window.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Menu_Bar.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Input.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Input_.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Tile.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/filename.h
dtk/Dtk_Script_Document.o: /usr/include/dirent.h /usr/include/bits/dirent.h
dtk/Dtk_Script_Document.o: /usr/include/bits/posix1_lim.h
dtk/Dtk_Script_Document.o: /usr/include/bits/local_lim.h
dtk/Dtk_Script_Document.o: /usr/include/linux/limits.h
dtk/Dtk_Script_Document.o: /usr/local/include/FL/fl_file_chooser.h
dtk/Dtk_Script_Document.o: /usr/include/stdio.h /usr/include/libio.h
dtk/Dtk_Script_Document.o: /usr/include/_G_config.h /usr/include/wchar.h
dtk/Dtk_Script_Document.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
dtk/Dtk_Script_Document.o: /usr/include/bits/sys_errlist.h
dtk/Dtk_Script_Document.o: /usr/include/string.h
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Menu_Button.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Preferences.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_File_Browser.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_File_Icon.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/filename.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Check_Button.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Light_Button.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Button.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_File_Input.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/Fl_Return_Button.H
dtk/Dtk_Script_Document.o: /usr/local/include/FL/fl_ask.H
dtk/Dtk_Script_Document.o: ../../newt0/src/newt_core/incs/NewtBC.h
dtk/Dtk_Script_Document.o: ../../newt0/src/newt_core/incs/NewtParser.h
dtk/Dtk_Script_Document.o: ../../newt0/src/newt_core/incs/NewtNSOF.h
dtk/Dtk_Script_Document.o: ../../newt0/src/newt_core/incs/NewtPkg.h
dtk/Dtk_Script_Document.o: ../../newt0/src/newt_core/incs/NewtPrint.h
dtk/Dtk_Script_Document.o: ../../newt0/src/newt_core/incs/NewtVM.h
dtk/Dtk_Project.o: dtk/Dtk_Project.h
dtk/Dtk_Project.o: ../../newt0/src/newt_core/incs/NewtType.h
dtk/Dtk_Project.o: ../../newt0/src/newt_core/incs/platform.h
dtk/Dtk_Project.o: ../../newt0/src/config.h /usr/include/stdint.h
dtk/Dtk_Project.o: /usr/include/features.h /usr/include/sys/cdefs.h
dtk/Dtk_Project.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
dtk/Dtk_Project.o: /usr/include/gnu/stubs-32.h /usr/include/bits/wchar.h
dtk/Dtk_Project.o: /usr/include/stdlib.h /usr/include/sys/types.h
dtk/Dtk_Project.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
dtk/Dtk_Project.o: /usr/include/time.h /usr/include/endian.h
dtk/Dtk_Project.o: /usr/include/bits/endian.h /usr/include/sys/select.h
dtk/Dtk_Project.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
dtk/Dtk_Project.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
dtk/Dtk_Project.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
dtk/Dtk_Project.o: ../../newt0/src/newt_core/incs/NewtConf.h
dtk/Dtk_Project.o: dtk/Dtk_Document.h dtk/Dtk_Document_Manager.h
dtk/Dtk_Project.o: fluid/Fldtk_Proj_Settings.h /usr/local/include/FL/Fl.H
dtk/Dtk_Project.o: /usr/local/include/FL/Enumerations.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Export.H fltk/Flmm_Set_Input.h
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Input.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Input_.H
dtk/Dtk_Project.o: fltk/Flmm_Set_Check_Button.h
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Check_Button.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Light_Button.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Button.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Group.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Widget.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Button.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Box.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Round_Button.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Value_Output.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Valuator.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Choice.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Menu_.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Menu_Item.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Image.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Text_Editor.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Text_Display.H
dtk/Dtk_Project.o: /usr/local/include/FL/fl_draw.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Group.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Scrollbar.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Slider.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Text_Buffer.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Double_Window.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Window.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Tabs.H fluid/main_ui.h
dtk/Dtk_Project.o: fltk/Flio_Inspector.h fltk/Flio_MNP4_Serial.h
dtk/Dtk_Project.o: fltk/Flio_Serial_Port.h /usr/local/include/FL/x.H
dtk/Dtk_Project.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
dtk/Dtk_Project.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
dtk/Dtk_Project.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
dtk/Dtk_Project.o: /usr/include/X11/keysymdef.h /usr/include/X11/Xatom.h
dtk/Dtk_Project.o: fltk/Flmm_Console.h fluid/Fldtk_Inspector.h main.h
dtk/Dtk_Project.o: globals.h fltk/Fldtk_Document_Tabs.h
dtk/Dtk_Project.o: fltk/Fldtk_Document_Browser.h
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Select_Browser.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Browser.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Browser_.H
dtk/Dtk_Project.o: fltk/Fldtk_Layout_Drop_Box.h
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Window.H nt/Nt_Group.h
dtk/Dtk_Project.o: ../../newt0/src/newt_core/incs/NewtCore.h
dtk/Dtk_Project.o: ../../newt0/src/newt_core/incs/NewtErrs.h
dtk/Dtk_Project.o: ../../newt0/src/newt_core/incs/NewtFns.h
dtk/Dtk_Project.o: ../../newt0/src/newt_core/incs/NewtEnv.h
dtk/Dtk_Project.o: ../../newt0/src/newt_core/incs/NewtMem.h
dtk/Dtk_Project.o: ../../newt0/src/newt_core/incs/NewtObj.h
dtk/Dtk_Project.o: /usr/local/include/FL/fl_ask.h
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Menu_Bar.H
dtk/Dtk_Project.o: /usr/local/include/FL/Fl_Tile.H
dtk/Dtk_Project.o: /usr/local/include/FL/filename.h /usr/include/dirent.h
dtk/Dtk_Project.o: /usr/include/bits/dirent.h /usr/include/bits/posix1_lim.h
dtk/Dtk_Project.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
dtk/Dtk_Project.o: /usr/include/assert.h /usr/include/stdio.h
dtk/Dtk_Project.o: /usr/include/libio.h /usr/include/_G_config.h
dtk/Dtk_Project.o: /usr/include/wchar.h /usr/include/gconv.h
dtk/Dtk_Project.o: /usr/include/bits/stdio_lim.h
dtk/Dtk_Project.o: /usr/include/bits/sys_errlist.h /usr/include/string.h
dtk/Dtk_Project.o: ../../newt0/src/newt_core/incs/NewtBC.h
dtk/Dtk_Project.o: ../../newt0/src/newt_core/incs/NewtParser.h
dtk/Dtk_Project.o: ../../newt0/src/newt_core/incs/NewtNSOF.h
dtk/Dtk_Project.o: ../../newt0/src/newt_core/incs/NewtPkg.h
dtk/Dtk_Project.o: ../../newt0/src/newt_core/incs/NewtPrint.h
dtk/Dtk_Document_Manager.o: dtk/Dtk_Document_Manager.h
dtk/Dtk_Document_Manager.o: ../../newt0/src/newt_core/incs/NewtType.h
dtk/Dtk_Document_Manager.o: ../../newt0/src/newt_core/incs/platform.h
dtk/Dtk_Document_Manager.o: ../../newt0/src/config.h /usr/include/stdint.h
dtk/Dtk_Document_Manager.o: /usr/include/features.h /usr/include/sys/cdefs.h
dtk/Dtk_Document_Manager.o: /usr/include/bits/wordsize.h
dtk/Dtk_Document_Manager.o: /usr/include/gnu/stubs.h
dtk/Dtk_Document_Manager.o: /usr/include/gnu/stubs-32.h
dtk/Dtk_Document_Manager.o: /usr/include/bits/wchar.h /usr/include/stdlib.h
dtk/Dtk_Document_Manager.o: /usr/include/sys/types.h
dtk/Dtk_Document_Manager.o: /usr/include/bits/types.h
dtk/Dtk_Document_Manager.o: /usr/include/bits/typesizes.h /usr/include/time.h
dtk/Dtk_Document_Manager.o: /usr/include/endian.h /usr/include/bits/endian.h
dtk/Dtk_Document_Manager.o: /usr/include/sys/select.h
dtk/Dtk_Document_Manager.o: /usr/include/bits/select.h
dtk/Dtk_Document_Manager.o: /usr/include/bits/sigset.h
dtk/Dtk_Document_Manager.o: /usr/include/bits/time.h
dtk/Dtk_Document_Manager.o: /usr/include/sys/sysmacros.h
dtk/Dtk_Document_Manager.o: /usr/include/bits/pthreadtypes.h
dtk/Dtk_Document_Manager.o: /usr/include/alloca.h
dtk/Dtk_Document_Manager.o: ../../newt0/src/newt_core/incs/NewtConf.h
dtk/Dtk_Document_Manager.o: dtk/Dtk_Document.h dtk/Dtk_Layout_Document.h
dtk/Dtk_Document_Manager.o: ./dtk/Dtk_Document.h dtk/Dtk_Script_Document.h
dtk/Dtk_Document_Manager.o: fltk/Fldtk_Document_Browser.h
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Select_Browser.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Browser.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Browser_.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Scrollbar.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Slider.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Valuator.H
dtk/Dtk_Document_Manager.o: fluid/main_ui.h /usr/local/include/FL/Fl.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Enumerations.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Export.H
dtk/Dtk_Document_Manager.o: fltk/Flio_Inspector.h fltk/Flio_MNP4_Serial.h
dtk/Dtk_Document_Manager.o: fltk/Flio_Serial_Port.h /usr/local/include/FL/x.H
dtk/Dtk_Document_Manager.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
dtk/Dtk_Document_Manager.o: /usr/include/X11/Xfuncproto.h
dtk/Dtk_Document_Manager.o: /usr/include/X11/Xosdefs.h
dtk/Dtk_Document_Manager.o: /usr/include/X11/Xutil.h
dtk/Dtk_Document_Manager.o: /usr/include/X11/keysym.h
dtk/Dtk_Document_Manager.o: /usr/include/X11/keysymdef.h
dtk/Dtk_Document_Manager.o: /usr/include/X11/Xatom.h
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Window.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Group.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Box.H
dtk/Dtk_Document_Manager.o: fltk/Flmm_Console.h
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Text_Editor.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Text_Display.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/fl_draw.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Widget.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Text_Buffer.H
dtk/Dtk_Document_Manager.o: fluid/Fldtk_Inspector.h main.h globals.h
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Group.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Button.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Choice.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Menu_.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Menu_Item.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Image.H
dtk/Dtk_Document_Manager.o: fltk/Fldtk_Document_Tabs.h
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Tabs.H
dtk/Dtk_Document_Manager.o: fltk/Fldtk_Layout_Drop_Box.h
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Window.H nt/Nt_Group.h
dtk/Dtk_Document_Manager.o: ../../newt0/src/newt_core/incs/NewtCore.h
dtk/Dtk_Document_Manager.o: ../../newt0/src/newt_core/incs/NewtErrs.h
dtk/Dtk_Document_Manager.o: ../../newt0/src/newt_core/incs/NewtFns.h
dtk/Dtk_Document_Manager.o: ../../newt0/src/newt_core/incs/NewtEnv.h
dtk/Dtk_Document_Manager.o: ../../newt0/src/newt_core/incs/NewtMem.h
dtk/Dtk_Document_Manager.o: ../../newt0/src/newt_core/incs/NewtObj.h
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/fl_ask.h
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Double_Window.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Menu_Bar.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Input.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Input_.H
dtk/Dtk_Document_Manager.o: /usr/local/include/FL/Fl_Tile.H
dtk/Dtk_Document_Manager.o: /usr/include/assert.h /usr/include/stdio.h
dtk/Dtk_Document_Manager.o: /usr/include/libio.h /usr/include/_G_config.h
dtk/Dtk_Document_Manager.o: /usr/include/wchar.h /usr/include/gconv.h
dtk/Dtk_Document_Manager.o: /usr/include/bits/stdio_lim.h
dtk/Dtk_Document_Manager.o: /usr/include/bits/sys_errlist.h
dtk/Dtk_Document_Manager.o: ../../newt0/src/newt_core/incs/NewtBC.h
dtk/Dtk_Document_Manager.o: ../../newt0/src/newt_core/incs/NewtParser.h
dtk/Dtk_Document_Manager.o: ../../newt0/src/newt_core/incs/NewtNSOF.h
dtk/Dtk_Document_Manager.o: ../../newt0/src/newt_core/incs/NewtPkg.h
dtk/Dtk_Document_Manager.o: ../../newt0/src/newt_core/incs/NewtPrint.h
dtk/Dtk_Error.o: dtk/Dtk_Error.h fltk/Flmm_Message.H /usr/include/stdio.h
dtk/Dtk_Error.o: /usr/include/features.h /usr/include/sys/cdefs.h
dtk/Dtk_Error.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
dtk/Dtk_Error.o: /usr/include/gnu/stubs-32.h /usr/include/bits/types.h
dtk/Dtk_Error.o: /usr/include/bits/typesizes.h /usr/include/libio.h
dtk/Dtk_Error.o: /usr/include/_G_config.h /usr/include/wchar.h
dtk/Dtk_Error.o: /usr/include/bits/wchar.h /usr/include/gconv.h
dtk/Dtk_Error.o: /usr/include/bits/stdio_lim.h
dtk/Dtk_Error.o: /usr/include/bits/sys_errlist.h
dtk/Dtk_Layout_Document.o: dtk/Dtk_Layout_Document.h ./dtk/Dtk_Document.h
dtk/Dtk_Layout_Document.o: ../../newt0/src/newt_core/incs/NewtType.h
dtk/Dtk_Layout_Document.o: ../../newt0/src/newt_core/incs/platform.h
dtk/Dtk_Layout_Document.o: ../../newt0/src/config.h /usr/include/stdint.h
dtk/Dtk_Layout_Document.o: /usr/include/features.h /usr/include/sys/cdefs.h
dtk/Dtk_Layout_Document.o: /usr/include/bits/wordsize.h
dtk/Dtk_Layout_Document.o: /usr/include/gnu/stubs.h
dtk/Dtk_Layout_Document.o: /usr/include/gnu/stubs-32.h
dtk/Dtk_Layout_Document.o: /usr/include/bits/wchar.h /usr/include/stdlib.h
dtk/Dtk_Layout_Document.o: /usr/include/sys/types.h /usr/include/bits/types.h
dtk/Dtk_Layout_Document.o: /usr/include/bits/typesizes.h /usr/include/time.h
dtk/Dtk_Layout_Document.o: /usr/include/endian.h /usr/include/bits/endian.h
dtk/Dtk_Layout_Document.o: /usr/include/sys/select.h
dtk/Dtk_Layout_Document.o: /usr/include/bits/select.h
dtk/Dtk_Layout_Document.o: /usr/include/bits/sigset.h
dtk/Dtk_Layout_Document.o: /usr/include/bits/time.h
dtk/Dtk_Layout_Document.o: /usr/include/sys/sysmacros.h
dtk/Dtk_Layout_Document.o: /usr/include/bits/pthreadtypes.h
dtk/Dtk_Layout_Document.o: /usr/include/alloca.h
dtk/Dtk_Layout_Document.o: ../../newt0/src/newt_core/incs/NewtConf.h
dtk/Dtk_Layout_Document.o: fltk/Fldtk_Layout_Editor.h fltk/Fldtk_Editor.h
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Group.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Widget.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Enumerations.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Export.H
dtk/Dtk_Layout_Document.o: fltk/Fldtk_Document_Tabs.h
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Tabs.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Group.H fluid/main_ui.h
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl.H fltk/Flio_Inspector.h
dtk/Dtk_Layout_Document.o: fltk/Flio_MNP4_Serial.h fltk/Flio_Serial_Port.h
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/x.H /usr/include/X11/Xlib.h
dtk/Dtk_Layout_Document.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
dtk/Dtk_Layout_Document.o: /usr/include/X11/Xosdefs.h
dtk/Dtk_Layout_Document.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
dtk/Dtk_Layout_Document.o: /usr/include/X11/keysymdef.h
dtk/Dtk_Layout_Document.o: /usr/include/X11/Xatom.h
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Window.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Box.H fltk/Flmm_Console.h
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Text_Editor.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Text_Display.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/fl_draw.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Scrollbar.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Slider.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Valuator.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Text_Buffer.H
dtk/Dtk_Layout_Document.o: fluid/Fldtk_Inspector.h main.h globals.h
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Button.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Choice.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Menu_.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Menu_Item.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Image.H
dtk/Dtk_Layout_Document.o: fltk/Fldtk_Document_Browser.h
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Select_Browser.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Browser.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Browser_.H
dtk/Dtk_Layout_Document.o: fltk/Fldtk_Layout_Drop_Box.h
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Window.H nt/Nt_Group.h
dtk/Dtk_Layout_Document.o: ../../newt0/src/newt_core/incs/NewtCore.h
dtk/Dtk_Layout_Document.o: ../../newt0/src/newt_core/incs/NewtErrs.h
dtk/Dtk_Layout_Document.o: ../../newt0/src/newt_core/incs/NewtFns.h
dtk/Dtk_Layout_Document.o: ../../newt0/src/newt_core/incs/NewtEnv.h
dtk/Dtk_Layout_Document.o: ../../newt0/src/newt_core/incs/NewtMem.h
dtk/Dtk_Layout_Document.o: ../../newt0/src/newt_core/incs/NewtObj.h
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/fl_ask.h
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Double_Window.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Menu_Bar.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Input.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Input_.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Tile.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/filename.h
dtk/Dtk_Layout_Document.o: /usr/include/dirent.h /usr/include/bits/dirent.h
dtk/Dtk_Layout_Document.o: /usr/include/bits/posix1_lim.h
dtk/Dtk_Layout_Document.o: /usr/include/bits/local_lim.h
dtk/Dtk_Layout_Document.o: /usr/include/linux/limits.h
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/fl_file_chooser.h
dtk/Dtk_Layout_Document.o: /usr/include/stdio.h /usr/include/libio.h
dtk/Dtk_Layout_Document.o: /usr/include/_G_config.h /usr/include/wchar.h
dtk/Dtk_Layout_Document.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
dtk/Dtk_Layout_Document.o: /usr/include/bits/sys_errlist.h
dtk/Dtk_Layout_Document.o: /usr/include/string.h
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Menu_Button.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Preferences.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_File_Browser.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_File_Icon.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/filename.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Check_Button.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Light_Button.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Button.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_File_Input.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/Fl_Return_Button.H
dtk/Dtk_Layout_Document.o: /usr/local/include/FL/fl_ask.H
fltk/Flio_Inspector.o: fltk/Flio_Inspector.h fltk/Flio_MNP4_Serial.h
fltk/Flio_Inspector.o: fltk/Flio_Serial_Port.h /usr/local/include/FL/x.H
fltk/Flio_Inspector.o: /usr/local/include/FL/Enumerations.H
fltk/Flio_Inspector.o: /usr/local/include/FL/Fl_Export.H
fltk/Flio_Inspector.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
fltk/Flio_Inspector.o: /usr/include/features.h /usr/include/sys/cdefs.h
fltk/Flio_Inspector.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
fltk/Flio_Inspector.o: /usr/include/gnu/stubs-32.h /usr/include/bits/types.h
fltk/Flio_Inspector.o: /usr/include/bits/typesizes.h /usr/include/time.h
fltk/Flio_Inspector.o: /usr/include/endian.h /usr/include/bits/endian.h
fltk/Flio_Inspector.o: /usr/include/sys/select.h /usr/include/bits/select.h
fltk/Flio_Inspector.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
fltk/Flio_Inspector.o: /usr/include/sys/sysmacros.h
fltk/Flio_Inspector.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
fltk/Flio_Inspector.o: /usr/include/X11/Xfuncproto.h
fltk/Flio_Inspector.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
fltk/Flio_Inspector.o: /usr/include/X11/keysym.h /usr/include/X11/keysymdef.h
fltk/Flio_Inspector.o: /usr/include/X11/Xatom.h
fltk/Flio_Inspector.o: /usr/local/include/FL/Fl_Window.H
fltk/Flio_Inspector.o: /usr/local/include/FL/Fl_Group.H
fltk/Flio_Inspector.o: /usr/local/include/FL/Fl_Box.H globals.h
fltk/Flio_Inspector.o: /usr/include/stdlib.h /usr/include/alloca.h
fltk/Flio_Inspector.o: /usr/include/stdio.h /usr/include/libio.h
fltk/Flio_Inspector.o: /usr/include/_G_config.h /usr/include/wchar.h
fltk/Flio_Inspector.o: /usr/include/bits/wchar.h /usr/include/gconv.h
fltk/Flio_Inspector.o: /usr/include/bits/stdio_lim.h
fltk/Flio_Inspector.o: /usr/include/bits/sys_errlist.h /usr/include/string.h
fltk/Flio_Inspector.o: /usr/include/ctype.h /usr/local/include/FL/Fl_Button.H
fltk/Flio_Inspector.o: /usr/local/include/FL/Fl_Image.H fltk/Flmm_Console.h
fltk/Flio_Inspector.o: /usr/local/include/FL/Fl_Text_Editor.H
fltk/Flio_Inspector.o: /usr/local/include/FL/Fl_Text_Display.H
fltk/Flio_Inspector.o: /usr/local/include/FL/fl_draw.H
fltk/Flio_Inspector.o: /usr/local/include/FL/Fl_Widget.H
fltk/Flio_Inspector.o: /usr/local/include/FL/Fl_Scrollbar.H
fltk/Flio_Inspector.o: /usr/local/include/FL/Fl_Slider.H
fltk/Flio_Inspector.o: /usr/local/include/FL/Fl_Valuator.H
fltk/Flio_Inspector.o: /usr/local/include/FL/Fl_Text_Buffer.H
fltk/Fldtk_Layout_Drop_Box.o: fltk/Fldtk_Layout_Drop_Box.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Button.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Window.H nt/Nt_Group.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Group.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Widget.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Enumerations.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Export.H
fltk/Fldtk_Layout_Drop_Box.o: ../../newt0/src/newt_core/incs/NewtCore.h
fltk/Fldtk_Layout_Drop_Box.o: ../../newt0/src/newt_core/incs/NewtType.h
fltk/Fldtk_Layout_Drop_Box.o: ../../newt0/src/newt_core/incs/platform.h
fltk/Fldtk_Layout_Drop_Box.o: ../../newt0/src/config.h /usr/include/stdint.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/features.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/sys/cdefs.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/bits/wordsize.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/gnu/stubs.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/gnu/stubs-32.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/bits/wchar.h /usr/include/stdlib.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/sys/types.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/bits/types.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/bits/typesizes.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/time.h /usr/include/endian.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/bits/endian.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/sys/select.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/bits/select.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/bits/sigset.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/bits/time.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/sys/sysmacros.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/bits/pthreadtypes.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/alloca.h
fltk/Fldtk_Layout_Drop_Box.o: ../../newt0/src/newt_core/incs/NewtConf.h
fltk/Fldtk_Layout_Drop_Box.o: ../../newt0/src/newt_core/incs/NewtErrs.h
fltk/Fldtk_Layout_Drop_Box.o: ../../newt0/src/newt_core/incs/NewtFns.h
fltk/Fldtk_Layout_Drop_Box.o: ../../newt0/src/newt_core/incs/NewtEnv.h
fltk/Fldtk_Layout_Drop_Box.o: ../../newt0/src/newt_core/incs/NewtMem.h
fltk/Fldtk_Layout_Drop_Box.o: ../../newt0/src/newt_core/incs/NewtObj.h
fltk/Fldtk_Layout_Drop_Box.o: globals.h /usr/local/include/FL/Fl.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/names.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/fl_file_chooser.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Double_Window.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Window.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Group.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/stdio.h /usr/include/libio.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/_G_config.h /usr/include/wchar.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/gconv.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/bits/stdio_lim.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/bits/sys_errlist.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/include/string.h
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Choice.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Menu_.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Menu_Item.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Image.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Menu_Button.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Preferences.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Tile.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_File_Browser.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Browser.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Browser_.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Scrollbar.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Slider.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Valuator.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_File_Icon.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/filename.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Box.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Check_Button.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Light_Button.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Button.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_File_Input.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Input.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Input_.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/Fl_Return_Button.H
fltk/Fldtk_Layout_Drop_Box.o: /usr/local/include/FL/fl_ask.H
fltk/Fldtk_Layout_Drop_Box.o: ../../newt0/src/newt_core/incs/NewtBC.h
fltk/Fldtk_Layout_Drop_Box.o: ../../newt0/src/newt_core/incs/NewtParser.h
fltk/Fldtk_Layout_Drop_Box.o: ../../newt0/src/newt_core/incs/NewtNSOF.h
fltk/Fldtk_Layout_Drop_Box.o: ../../newt0/src/newt_core/incs/NewtPkg.h
fltk/Fldtk_Layout_Drop_Box.o: ../../newt0/src/newt_core/incs/NewtPrint.h
fltk/Flmm_Set_Input.o: fltk/Flmm_Set_Input.h /usr/local/include/FL/Fl_Input.H
fltk/Flmm_Set_Input.o: /usr/local/include/FL/Fl_Input_.H
fltk/Flmm_Set_Input.o: /usr/include/stdlib.h /usr/include/features.h
fltk/Flmm_Set_Input.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
fltk/Flmm_Set_Input.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
fltk/Flmm_Set_Input.o: /usr/include/sys/types.h /usr/include/bits/types.h
fltk/Flmm_Set_Input.o: /usr/include/bits/typesizes.h /usr/include/time.h
fltk/Flmm_Set_Input.o: /usr/include/endian.h /usr/include/bits/endian.h
fltk/Flmm_Set_Input.o: /usr/include/sys/select.h /usr/include/bits/select.h
fltk/Flmm_Set_Input.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
fltk/Flmm_Set_Input.o: /usr/include/sys/sysmacros.h
fltk/Flmm_Set_Input.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
fltk/Flmm_Set_Input.o: /usr/include/string.h
fltk/Flio_MNP4_Serial.o: fltk/Flio_MNP4_Serial.h fltk/Flio_Serial_Port.h
fltk/Flio_MNP4_Serial.o: /usr/local/include/FL/x.H
fltk/Flio_MNP4_Serial.o: /usr/local/include/FL/Enumerations.H
fltk/Flio_MNP4_Serial.o: /usr/local/include/FL/Fl_Export.H
fltk/Flio_MNP4_Serial.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
fltk/Flio_MNP4_Serial.o: /usr/include/features.h /usr/include/sys/cdefs.h
fltk/Flio_MNP4_Serial.o: /usr/include/bits/wordsize.h
fltk/Flio_MNP4_Serial.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
fltk/Flio_MNP4_Serial.o: /usr/include/bits/types.h
fltk/Flio_MNP4_Serial.o: /usr/include/bits/typesizes.h /usr/include/time.h
fltk/Flio_MNP4_Serial.o: /usr/include/endian.h /usr/include/bits/endian.h
fltk/Flio_MNP4_Serial.o: /usr/include/sys/select.h /usr/include/bits/select.h
fltk/Flio_MNP4_Serial.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
fltk/Flio_MNP4_Serial.o: /usr/include/sys/sysmacros.h
fltk/Flio_MNP4_Serial.o: /usr/include/bits/pthreadtypes.h
fltk/Flio_MNP4_Serial.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
fltk/Flio_MNP4_Serial.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
fltk/Flio_MNP4_Serial.o: /usr/include/X11/keysym.h
fltk/Flio_MNP4_Serial.o: /usr/include/X11/keysymdef.h
fltk/Flio_MNP4_Serial.o: /usr/include/X11/Xatom.h
fltk/Flio_MNP4_Serial.o: /usr/local/include/FL/Fl_Window.H
fltk/Flio_MNP4_Serial.o: /usr/local/include/FL/Fl_Group.H
fltk/Flio_MNP4_Serial.o: /usr/local/include/FL/Fl_Box.H
fltk/Flio_MNP4_Serial.o: /usr/local/include/FL/Fl.H /usr/include/stdlib.h
fltk/Flio_MNP4_Serial.o: /usr/include/alloca.h /usr/include/stdio.h
fltk/Flio_MNP4_Serial.o: /usr/include/libio.h /usr/include/_G_config.h
fltk/Flio_MNP4_Serial.o: /usr/include/wchar.h /usr/include/bits/wchar.h
fltk/Flio_MNP4_Serial.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
fltk/Flio_MNP4_Serial.o: /usr/include/bits/sys_errlist.h
fltk/Flio_Serial_Port.o: fltk/Flio_Serial_Port.h /usr/local/include/FL/x.H
fltk/Flio_Serial_Port.o: /usr/local/include/FL/Enumerations.H
fltk/Flio_Serial_Port.o: /usr/local/include/FL/Fl_Export.H
fltk/Flio_Serial_Port.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
fltk/Flio_Serial_Port.o: /usr/include/features.h /usr/include/sys/cdefs.h
fltk/Flio_Serial_Port.o: /usr/include/bits/wordsize.h
fltk/Flio_Serial_Port.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
fltk/Flio_Serial_Port.o: /usr/include/bits/types.h
fltk/Flio_Serial_Port.o: /usr/include/bits/typesizes.h /usr/include/time.h
fltk/Flio_Serial_Port.o: /usr/include/endian.h /usr/include/bits/endian.h
fltk/Flio_Serial_Port.o: /usr/include/sys/select.h /usr/include/bits/select.h
fltk/Flio_Serial_Port.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
fltk/Flio_Serial_Port.o: /usr/include/sys/sysmacros.h
fltk/Flio_Serial_Port.o: /usr/include/bits/pthreadtypes.h
fltk/Flio_Serial_Port.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
fltk/Flio_Serial_Port.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
fltk/Flio_Serial_Port.o: /usr/include/X11/keysym.h
fltk/Flio_Serial_Port.o: /usr/include/X11/keysymdef.h
fltk/Flio_Serial_Port.o: /usr/include/X11/Xatom.h
fltk/Flio_Serial_Port.o: /usr/local/include/FL/Fl_Window.H
fltk/Flio_Serial_Port.o: /usr/local/include/FL/Fl_Group.H
fltk/Flio_Serial_Port.o: /usr/local/include/FL/Fl_Box.H
fltk/Flio_Serial_Port.o: /usr/local/include/FL/Fl.H
fltk/Flio_Serial_Port.o: /usr/local/include/FL/fl_draw.h
fltk/Flio_Serial_Port.o: /usr/include/stdlib.h /usr/include/alloca.h
fltk/Flio_Serial_Port.o: /usr/include/stdio.h /usr/include/libio.h
fltk/Flio_Serial_Port.o: /usr/include/_G_config.h /usr/include/wchar.h
fltk/Flio_Serial_Port.o: /usr/include/bits/wchar.h /usr/include/gconv.h
fltk/Flio_Serial_Port.o: /usr/include/bits/stdio_lim.h
fltk/Flio_Serial_Port.o: /usr/include/bits/sys_errlist.h
fltk/Flio_Serial_Port.o: /usr/include/string.h /usr/include/sys/uio.h
fltk/Flio_Serial_Port.o: /usr/include/bits/uio.h /usr/include/sys/ioctl.h
fltk/Flio_Serial_Port.o: /usr/include/bits/ioctls.h /usr/include/asm/ioctls.h
fltk/Flio_Serial_Port.o: /usr/include/asm-i386/ioctls.h
fltk/Flio_Serial_Port.o: /usr/include/asm/ioctl.h
fltk/Flio_Serial_Port.o: /usr/include/asm-i386/ioctl.h
fltk/Flio_Serial_Port.o: /usr/include/asm-generic/ioctl.h
fltk/Flio_Serial_Port.o: /usr/include/bits/ioctl-types.h
fltk/Flio_Serial_Port.o: /usr/include/sys/ttydefaults.h /usr/include/unistd.h
fltk/Flio_Serial_Port.o: /usr/include/bits/posix_opt.h
fltk/Flio_Serial_Port.o: /usr/include/bits/confname.h /usr/include/getopt.h
fltk/Flio_Serial_Port.o: /usr/include/termios.h /usr/include/bits/termios.h
fltk/Flio_Serial_Port.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
fltk/Flio_Serial_Port.o: /usr/include/ctype.h /usr/include/errno.h
fltk/Flio_Serial_Port.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
fltk/Flio_Serial_Port.o: /usr/include/asm/errno.h
fltk/Flio_Serial_Port.o: /usr/include/asm-i386/errno.h
fltk/Flio_Serial_Port.o: /usr/include/asm-generic/errno.h
fltk/Flio_Serial_Port.o: /usr/include/asm-generic/errno-base.h
fltk/Flmm_Set_Check_Button.o: fltk/Flmm_Set_Check_Button.h
fltk/Flmm_Set_Check_Button.o: /usr/local/include/FL/Fl_Check_Button.H
fltk/Flmm_Set_Check_Button.o: /usr/local/include/FL/Fl_Light_Button.H
fltk/Flmm_Set_Check_Button.o: /usr/local/include/FL/Fl_Button.H
fltk/Flmm_Set_Check_Button.o: /usr/include/stdlib.h /usr/include/features.h
fltk/Flmm_Set_Check_Button.o: /usr/include/sys/cdefs.h
fltk/Flmm_Set_Check_Button.o: /usr/include/bits/wordsize.h
fltk/Flmm_Set_Check_Button.o: /usr/include/gnu/stubs.h
fltk/Flmm_Set_Check_Button.o: /usr/include/gnu/stubs-32.h
fltk/Flmm_Set_Check_Button.o: /usr/include/sys/types.h
fltk/Flmm_Set_Check_Button.o: /usr/include/bits/types.h
fltk/Flmm_Set_Check_Button.o: /usr/include/bits/typesizes.h
fltk/Flmm_Set_Check_Button.o: /usr/include/time.h /usr/include/endian.h
fltk/Flmm_Set_Check_Button.o: /usr/include/bits/endian.h
fltk/Flmm_Set_Check_Button.o: /usr/include/sys/select.h
fltk/Flmm_Set_Check_Button.o: /usr/include/bits/select.h
fltk/Flmm_Set_Check_Button.o: /usr/include/bits/sigset.h
fltk/Flmm_Set_Check_Button.o: /usr/include/bits/time.h
fltk/Flmm_Set_Check_Button.o: /usr/include/sys/sysmacros.h
fltk/Flmm_Set_Check_Button.o: /usr/include/bits/pthreadtypes.h
fltk/Flmm_Set_Check_Button.o: /usr/include/alloca.h /usr/include/string.h
fltk/Fldtk_Script_Editor.o: fltk/Fldtk_Script_Editor.h fltk/Fldtk_Editor.h
fltk/Fldtk_Script_Editor.o: /usr/local/include/FL/Fl_Group.H
fltk/Fldtk_Script_Editor.o: /usr/local/include/FL/Fl_Widget.H
fltk/Fldtk_Script_Editor.o: /usr/local/include/FL/Enumerations.H
fltk/Fldtk_Script_Editor.o: /usr/local/include/FL/Fl_Export.H
fltk/Fldtk_Script_Editor.o: /usr/local/include/FL/Fl.H
fltk/Fldtk_Script_Editor.o: /usr/local/include/FL/Fl_Text_Editor.H
fltk/Fldtk_Script_Editor.o: /usr/local/include/FL/Fl_Text_Display.H
fltk/Fldtk_Script_Editor.o: /usr/local/include/FL/fl_draw.H
fltk/Fldtk_Script_Editor.o: /usr/local/include/FL/Fl_Group.H
fltk/Fldtk_Script_Editor.o: /usr/local/include/FL/Fl_Scrollbar.H
fltk/Fldtk_Script_Editor.o: /usr/local/include/FL/Fl_Slider.H
fltk/Fldtk_Script_Editor.o: /usr/local/include/FL/Fl_Valuator.H
fltk/Fldtk_Script_Editor.o: /usr/local/include/FL/Fl_Text_Buffer.H
fltk/Fldtk_Script_Editor.o: /usr/local/include/FL/Fl_Text_Buffer.H
fltk/Fldtk_Script_Editor.o: /usr/include/stdio.h /usr/include/features.h
fltk/Fldtk_Script_Editor.o: /usr/include/sys/cdefs.h
fltk/Fldtk_Script_Editor.o: /usr/include/bits/wordsize.h
fltk/Fldtk_Script_Editor.o: /usr/include/gnu/stubs.h
fltk/Fldtk_Script_Editor.o: /usr/include/gnu/stubs-32.h
fltk/Fldtk_Script_Editor.o: /usr/include/bits/types.h
fltk/Fldtk_Script_Editor.o: /usr/include/bits/typesizes.h
fltk/Fldtk_Script_Editor.o: /usr/include/libio.h /usr/include/_G_config.h
fltk/Fldtk_Script_Editor.o: /usr/include/wchar.h /usr/include/bits/wchar.h
fltk/Fldtk_Script_Editor.o: /usr/include/gconv.h
fltk/Fldtk_Script_Editor.o: /usr/include/bits/stdio_lim.h
fltk/Fldtk_Script_Editor.o: /usr/include/bits/sys_errlist.h
fltk/Fldtk_Script_Editor.o: /usr/include/stdlib.h /usr/include/sys/types.h
fltk/Fldtk_Script_Editor.o: /usr/include/time.h /usr/include/endian.h
fltk/Fldtk_Script_Editor.o: /usr/include/bits/endian.h
fltk/Fldtk_Script_Editor.o: /usr/include/sys/select.h
fltk/Fldtk_Script_Editor.o: /usr/include/bits/select.h
fltk/Fldtk_Script_Editor.o: /usr/include/bits/sigset.h
fltk/Fldtk_Script_Editor.o: /usr/include/bits/time.h
fltk/Fldtk_Script_Editor.o: /usr/include/sys/sysmacros.h
fltk/Fldtk_Script_Editor.o: /usr/include/bits/pthreadtypes.h
fltk/Fldtk_Script_Editor.o: /usr/include/alloca.h
fltk/Flmm_Message.o: fltk/Flmm_Message.H /usr/include/stdlib.h
fltk/Flmm_Message.o: /usr/include/features.h /usr/include/sys/cdefs.h
fltk/Flmm_Message.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
fltk/Flmm_Message.o: /usr/include/gnu/stubs-32.h /usr/include/sys/types.h
fltk/Flmm_Message.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
fltk/Flmm_Message.o: /usr/include/time.h /usr/include/endian.h
fltk/Flmm_Message.o: /usr/include/bits/endian.h /usr/include/sys/select.h
fltk/Flmm_Message.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
fltk/Flmm_Message.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
fltk/Flmm_Message.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
fltk/Flmm_Message.o: /usr/include/string.h /usr/include/stdio.h
fltk/Flmm_Message.o: /usr/include/libio.h /usr/include/_G_config.h
fltk/Flmm_Message.o: /usr/include/wchar.h /usr/include/bits/wchar.h
fltk/Flmm_Message.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
fltk/Flmm_Message.o: /usr/include/bits/sys_errlist.h /usr/include/errno.h
fltk/Flmm_Message.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
fltk/Flmm_Message.o: /usr/include/asm/errno.h /usr/include/asm-i386/errno.h
fltk/Flmm_Message.o: /usr/include/asm-generic/errno.h
fltk/Flmm_Message.o: /usr/include/asm-generic/errno-base.h
fltk/Flmm_Message.o: /usr/local/include/FL/x.H
fltk/Flmm_Message.o: /usr/local/include/FL/Enumerations.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Export.H
fltk/Flmm_Message.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
fltk/Flmm_Message.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
fltk/Flmm_Message.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
fltk/Flmm_Message.o: /usr/include/X11/keysymdef.h /usr/include/X11/Xatom.h
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Window.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Group.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Menu_Item.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Widget.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Image.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Window.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Box.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Browser.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Browser_.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Scrollbar.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Slider.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Valuator.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Button.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Return_Button.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Button.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Output.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Input.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Input_.H
fltk/Flmm_Message.o: /usr/local/include/FL/Fl_Scroll.H
fltk/Flmm_Message.o: /usr/local/include/FL/fl_ask.H
fltk/Flmm_Message.o: /usr/local/include/FL/fl_draw.H
fltk/Fldtk_Document_Browser.o: fltk/Fldtk_Document_Browser.h
fltk/Fldtk_Document_Browser.o: /usr/local/include/FL/Fl_Select_Browser.H
fltk/Fldtk_Document_Browser.o: /usr/local/include/FL/Fl_Browser.H
fltk/Fldtk_Document_Browser.o: /usr/local/include/FL/Fl_Browser_.H
fltk/Fldtk_Document_Browser.o: /usr/local/include/FL/Fl_Scrollbar.H
fltk/Fldtk_Document_Browser.o: /usr/local/include/FL/Fl_Slider.H
fltk/Fldtk_Document_Browser.o: /usr/local/include/FL/Fl_Valuator.H
fltk/Fldtk_Document_Browser.o: /usr/include/stdlib.h /usr/include/features.h
fltk/Fldtk_Document_Browser.o: /usr/include/sys/cdefs.h
fltk/Fldtk_Document_Browser.o: /usr/include/bits/wordsize.h
fltk/Fldtk_Document_Browser.o: /usr/include/gnu/stubs.h
fltk/Fldtk_Document_Browser.o: /usr/include/gnu/stubs-32.h
fltk/Fldtk_Document_Browser.o: /usr/include/sys/types.h
fltk/Fldtk_Document_Browser.o: /usr/include/bits/types.h
fltk/Fldtk_Document_Browser.o: /usr/include/bits/typesizes.h
fltk/Fldtk_Document_Browser.o: /usr/include/time.h /usr/include/endian.h
fltk/Fldtk_Document_Browser.o: /usr/include/bits/endian.h
fltk/Fldtk_Document_Browser.o: /usr/include/sys/select.h
fltk/Fldtk_Document_Browser.o: /usr/include/bits/select.h
fltk/Fldtk_Document_Browser.o: /usr/include/bits/sigset.h
fltk/Fldtk_Document_Browser.o: /usr/include/bits/time.h
fltk/Fldtk_Document_Browser.o: /usr/include/sys/sysmacros.h
fltk/Fldtk_Document_Browser.o: /usr/include/bits/pthreadtypes.h
fltk/Fldtk_Document_Browser.o: /usr/include/alloca.h /usr/include/string.h
fltk/Fldtk_Layout_Editor.o: fltk/Fldtk_Layout_Editor.h fltk/Fldtk_Editor.h
fltk/Fldtk_Layout_Editor.o: /usr/local/include/FL/Fl_Group.H
fltk/Fldtk_Layout_Editor.o: /usr/local/include/FL/Fl_Widget.H
fltk/Fldtk_Layout_Editor.o: /usr/local/include/FL/Enumerations.H
fltk/Fldtk_Layout_Editor.o: /usr/local/include/FL/Fl_Export.H
fltk/Fldtk_Layout_Editor.o: /usr/local/include/FL/Fl.H
fltk/Fldtk_Layout_Editor.o: /usr/local/include/FL/Fl_Text_Editor.H
fltk/Fldtk_Layout_Editor.o: /usr/local/include/FL/Fl_Text_Display.H
fltk/Fldtk_Layout_Editor.o: /usr/local/include/FL/fl_draw.H
fltk/Fldtk_Layout_Editor.o: /usr/local/include/FL/Fl_Group.H
fltk/Fldtk_Layout_Editor.o: /usr/local/include/FL/Fl_Scrollbar.H
fltk/Fldtk_Layout_Editor.o: /usr/local/include/FL/Fl_Slider.H
fltk/Fldtk_Layout_Editor.o: /usr/local/include/FL/Fl_Valuator.H
fltk/Fldtk_Layout_Editor.o: /usr/local/include/FL/Fl_Text_Buffer.H
fltk/Fldtk_Layout_Editor.o: /usr/local/include/FL/Fl_Text_Buffer.H
fltk/Fldtk_Layout_Editor.o: /usr/include/stdio.h /usr/include/features.h
fltk/Fldtk_Layout_Editor.o: /usr/include/sys/cdefs.h
fltk/Fldtk_Layout_Editor.o: /usr/include/bits/wordsize.h
fltk/Fldtk_Layout_Editor.o: /usr/include/gnu/stubs.h
fltk/Fldtk_Layout_Editor.o: /usr/include/gnu/stubs-32.h
fltk/Fldtk_Layout_Editor.o: /usr/include/bits/types.h
fltk/Fldtk_Layout_Editor.o: /usr/include/bits/typesizes.h
fltk/Fldtk_Layout_Editor.o: /usr/include/libio.h /usr/include/_G_config.h
fltk/Fldtk_Layout_Editor.o: /usr/include/wchar.h /usr/include/bits/wchar.h
fltk/Fldtk_Layout_Editor.o: /usr/include/gconv.h
fltk/Fldtk_Layout_Editor.o: /usr/include/bits/stdio_lim.h
fltk/Fldtk_Layout_Editor.o: /usr/include/bits/sys_errlist.h
fltk/Fldtk_Layout_Editor.o: /usr/include/stdlib.h /usr/include/sys/types.h
fltk/Fldtk_Layout_Editor.o: /usr/include/time.h /usr/include/endian.h
fltk/Fldtk_Layout_Editor.o: /usr/include/bits/endian.h
fltk/Fldtk_Layout_Editor.o: /usr/include/sys/select.h
fltk/Fldtk_Layout_Editor.o: /usr/include/bits/select.h
fltk/Fldtk_Layout_Editor.o: /usr/include/bits/sigset.h
fltk/Fldtk_Layout_Editor.o: /usr/include/bits/time.h
fltk/Fldtk_Layout_Editor.o: /usr/include/sys/sysmacros.h
fltk/Fldtk_Layout_Editor.o: /usr/include/bits/pthreadtypes.h
fltk/Fldtk_Layout_Editor.o: /usr/include/alloca.h
fltk/Fldtk_Editor.o: fltk/Fldtk_Editor.h /usr/local/include/FL/Fl_Group.H
fltk/Fldtk_Editor.o: /usr/local/include/FL/Fl_Widget.H
fltk/Fldtk_Editor.o: /usr/local/include/FL/Enumerations.H
fltk/Fldtk_Editor.o: /usr/local/include/FL/Fl_Export.H
fltk/Fldtk_Editor.o: /usr/local/include/FL/Fl.H
fltk/Fldtk_Editor.o: /usr/local/include/FL/Fl_Text_Editor.H
fltk/Fldtk_Editor.o: /usr/local/include/FL/Fl_Text_Display.H
fltk/Fldtk_Editor.o: /usr/local/include/FL/fl_draw.H
fltk/Fldtk_Editor.o: /usr/local/include/FL/Fl_Group.H
fltk/Fldtk_Editor.o: /usr/local/include/FL/Fl_Scrollbar.H
fltk/Fldtk_Editor.o: /usr/local/include/FL/Fl_Slider.H
fltk/Fldtk_Editor.o: /usr/local/include/FL/Fl_Valuator.H
fltk/Fldtk_Editor.o: /usr/local/include/FL/Fl_Text_Buffer.H
fltk/Fldtk_Editor.o: /usr/local/include/FL/Fl_Text_Buffer.H
fltk/Fldtk_Editor.o: /usr/include/stdio.h /usr/include/features.h
fltk/Fldtk_Editor.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
fltk/Fldtk_Editor.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
fltk/Fldtk_Editor.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
fltk/Fldtk_Editor.o: /usr/include/libio.h /usr/include/_G_config.h
fltk/Fldtk_Editor.o: /usr/include/wchar.h /usr/include/bits/wchar.h
fltk/Fldtk_Editor.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
fltk/Fldtk_Editor.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
fltk/Fldtk_Editor.o: /usr/include/sys/types.h /usr/include/time.h
fltk/Fldtk_Editor.o: /usr/include/endian.h /usr/include/bits/endian.h
fltk/Fldtk_Editor.o: /usr/include/sys/select.h /usr/include/bits/select.h
fltk/Fldtk_Editor.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
fltk/Fldtk_Editor.o: /usr/include/sys/sysmacros.h
fltk/Fldtk_Editor.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
fltk/Fldtk_Document_Tabs.o: fltk/Fldtk_Document_Tabs.h
fltk/Fldtk_Document_Tabs.o: /usr/local/include/FL/Fl_Tabs.H
fltk/Fldtk_Document_Tabs.o: /usr/local/include/FL/Fl_Group.H
fltk/Fldtk_Document_Tabs.o: fltk/Fldtk_Editor.h
fltk/Fldtk_Document_Tabs.o: /usr/local/include/FL/Fl_Group.H
fltk/Fldtk_Document_Tabs.o: /usr/local/include/FL/Fl_Widget.H
fltk/Fldtk_Document_Tabs.o: /usr/local/include/FL/Enumerations.H
fltk/Fldtk_Document_Tabs.o: /usr/local/include/FL/Fl_Export.H
fltk/Fldtk_Document_Tabs.o: /usr/local/include/FL/Fl.H
fltk/Fldtk_Document_Tabs.o: /usr/local/include/FL/fl_draw.h
fltk/Fldtk_Document_Tabs.o: /usr/include/stdio.h /usr/include/features.h
fltk/Fldtk_Document_Tabs.o: /usr/include/sys/cdefs.h
fltk/Fldtk_Document_Tabs.o: /usr/include/bits/wordsize.h
fltk/Fldtk_Document_Tabs.o: /usr/include/gnu/stubs.h
fltk/Fldtk_Document_Tabs.o: /usr/include/gnu/stubs-32.h
fltk/Fldtk_Document_Tabs.o: /usr/include/bits/types.h
fltk/Fldtk_Document_Tabs.o: /usr/include/bits/typesizes.h
fltk/Fldtk_Document_Tabs.o: /usr/include/libio.h /usr/include/_G_config.h
fltk/Fldtk_Document_Tabs.o: /usr/include/wchar.h /usr/include/bits/wchar.h
fltk/Fldtk_Document_Tabs.o: /usr/include/gconv.h
fltk/Fldtk_Document_Tabs.o: /usr/include/bits/stdio_lim.h
fltk/Fldtk_Document_Tabs.o: /usr/include/bits/sys_errlist.h
fltk/Fldtk_Document_Tabs.o: /usr/include/stdlib.h /usr/include/sys/types.h
fltk/Fldtk_Document_Tabs.o: /usr/include/time.h /usr/include/endian.h
fltk/Fldtk_Document_Tabs.o: /usr/include/bits/endian.h
fltk/Fldtk_Document_Tabs.o: /usr/include/sys/select.h
fltk/Fldtk_Document_Tabs.o: /usr/include/bits/select.h
fltk/Fldtk_Document_Tabs.o: /usr/include/bits/sigset.h
fltk/Fldtk_Document_Tabs.o: /usr/include/bits/time.h
fltk/Fldtk_Document_Tabs.o: /usr/include/sys/sysmacros.h
fltk/Fldtk_Document_Tabs.o: /usr/include/bits/pthreadtypes.h
fltk/Fldtk_Document_Tabs.o: /usr/include/alloca.h
fltk/Flmm_Console.o: fltk/Flmm_Console.h
fltk/Flmm_Console.o: /usr/local/include/FL/Fl_Text_Editor.H
fltk/Flmm_Console.o: /usr/local/include/FL/Fl_Text_Display.H
fltk/Flmm_Console.o: /usr/local/include/FL/fl_draw.H
fltk/Flmm_Console.o: /usr/local/include/FL/Enumerations.H
fltk/Flmm_Console.o: /usr/local/include/FL/Fl_Export.H
fltk/Flmm_Console.o: /usr/local/include/FL/Fl_Group.H
fltk/Flmm_Console.o: /usr/local/include/FL/Fl_Widget.H
fltk/Flmm_Console.o: /usr/local/include/FL/Fl_Scrollbar.H
fltk/Flmm_Console.o: /usr/local/include/FL/Fl_Slider.H
fltk/Flmm_Console.o: /usr/local/include/FL/Fl_Valuator.H
fltk/Flmm_Console.o: /usr/local/include/FL/Fl_Text_Buffer.H
fltk/Flmm_Console.o: /usr/local/include/FL/Fl.H
fltk/Flmm_Console.o: /usr/local/include/FL/fl_draw.h /usr/include/stdio.h
fltk/Flmm_Console.o: /usr/include/features.h /usr/include/sys/cdefs.h
fltk/Flmm_Console.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
fltk/Flmm_Console.o: /usr/include/gnu/stubs-32.h /usr/include/bits/types.h
fltk/Flmm_Console.o: /usr/include/bits/typesizes.h /usr/include/libio.h
fltk/Flmm_Console.o: /usr/include/_G_config.h /usr/include/wchar.h
fltk/Flmm_Console.o: /usr/include/bits/wchar.h /usr/include/gconv.h
fltk/Flmm_Console.o: /usr/include/bits/stdio_lim.h
fltk/Flmm_Console.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
fltk/Flmm_Console.o: /usr/include/sys/types.h /usr/include/time.h
fltk/Flmm_Console.o: /usr/include/endian.h /usr/include/bits/endian.h
fltk/Flmm_Console.o: /usr/include/sys/select.h /usr/include/bits/select.h
fltk/Flmm_Console.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
fltk/Flmm_Console.o: /usr/include/sys/sysmacros.h
fltk/Flmm_Console.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
fluid/Fldtk_Proj_Settings.o: fluid/Fldtk_Proj_Settings.h
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Enumerations.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Export.H
fluid/Fldtk_Proj_Settings.o: fltk/Flmm_Set_Input.h
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Input.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Input_.H
fluid/Fldtk_Proj_Settings.o: fltk/Flmm_Set_Check_Button.h
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Check_Button.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Light_Button.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Button.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Group.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Widget.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Button.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Box.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Round_Button.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Value_Output.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Valuator.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Choice.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Menu_.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Menu_Item.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Image.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Text_Editor.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Text_Display.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/fl_draw.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Group.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Scrollbar.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Slider.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Text_Buffer.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Double_Window.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Window.H
fluid/Fldtk_Proj_Settings.o: /usr/local/include/FL/Fl_Tabs.H
fluid/Fldtk_Proj_Settings.o: /usr/include/string.h /usr/include/features.h
fluid/Fldtk_Proj_Settings.o: /usr/include/sys/cdefs.h
fluid/Fldtk_Proj_Settings.o: /usr/include/bits/wordsize.h
fluid/Fldtk_Proj_Settings.o: /usr/include/gnu/stubs.h
fluid/Fldtk_Proj_Settings.o: /usr/include/gnu/stubs-32.h
fluid/Fldtk_Proj_Settings.o: /usr/include/stdlib.h /usr/include/sys/types.h
fluid/Fldtk_Proj_Settings.o: /usr/include/bits/types.h
fluid/Fldtk_Proj_Settings.o: /usr/include/bits/typesizes.h
fluid/Fldtk_Proj_Settings.o: /usr/include/time.h /usr/include/endian.h
fluid/Fldtk_Proj_Settings.o: /usr/include/bits/endian.h
fluid/Fldtk_Proj_Settings.o: /usr/include/sys/select.h
fluid/Fldtk_Proj_Settings.o: /usr/include/bits/select.h
fluid/Fldtk_Proj_Settings.o: /usr/include/bits/sigset.h
fluid/Fldtk_Proj_Settings.o: /usr/include/bits/time.h
fluid/Fldtk_Proj_Settings.o: /usr/include/sys/sysmacros.h
fluid/Fldtk_Proj_Settings.o: /usr/include/bits/pthreadtypes.h
fluid/Fldtk_Proj_Settings.o: /usr/include/alloca.h /usr/include/stdio.h
fluid/Fldtk_Proj_Settings.o: /usr/include/libio.h /usr/include/_G_config.h
fluid/Fldtk_Proj_Settings.o: /usr/include/wchar.h /usr/include/bits/wchar.h
fluid/Fldtk_Proj_Settings.o: /usr/include/gconv.h
fluid/Fldtk_Proj_Settings.o: /usr/include/bits/stdio_lim.h
fluid/Fldtk_Proj_Settings.o: /usr/include/bits/sys_errlist.h
fluid/main_ui.o: fluid/main_ui.h /usr/local/include/FL/Fl.H
fluid/main_ui.o: /usr/local/include/FL/Enumerations.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Export.H fltk/Flio_Inspector.h
fluid/main_ui.o: fltk/Flio_MNP4_Serial.h fltk/Flio_Serial_Port.h
fluid/main_ui.o: /usr/local/include/FL/x.H /usr/include/X11/Xlib.h
fluid/main_ui.o: /usr/include/sys/types.h /usr/include/features.h
fluid/main_ui.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
fluid/main_ui.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
fluid/main_ui.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
fluid/main_ui.o: /usr/include/time.h /usr/include/endian.h
fluid/main_ui.o: /usr/include/bits/endian.h /usr/include/sys/select.h
fluid/main_ui.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
fluid/main_ui.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
fluid/main_ui.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
fluid/main_ui.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
fluid/main_ui.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
fluid/main_ui.o: /usr/include/X11/keysymdef.h /usr/include/X11/Xatom.h
fluid/main_ui.o: /usr/local/include/FL/Fl_Window.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Group.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Box.H fltk/Flmm_Console.h
fluid/main_ui.o: /usr/local/include/FL/Fl_Text_Editor.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Text_Display.H
fluid/main_ui.o: /usr/local/include/FL/fl_draw.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Widget.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Scrollbar.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Slider.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Valuator.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Text_Buffer.H
fluid/main_ui.o: fluid/Fldtk_Inspector.h main.h globals.h
fluid/main_ui.o: /usr/local/include/FL/Fl_Group.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Button.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Choice.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Menu_.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Menu_Item.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Image.H fltk/Fldtk_Document_Tabs.h
fluid/main_ui.o: /usr/local/include/FL/Fl_Tabs.H
fluid/main_ui.o: fltk/Fldtk_Document_Browser.h
fluid/main_ui.o: /usr/local/include/FL/Fl_Select_Browser.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Browser.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Browser_.H
fluid/main_ui.o: fltk/Fldtk_Layout_Drop_Box.h
fluid/main_ui.o: /usr/local/include/FL/Fl_Window.H nt/Nt_Group.h
fluid/main_ui.o: ../../newt0/src/newt_core/incs/NewtCore.h
fluid/main_ui.o: ../../newt0/src/newt_core/incs/NewtType.h
fluid/main_ui.o: ../../newt0/src/newt_core/incs/platform.h
fluid/main_ui.o: ../../newt0/src/config.h /usr/include/stdint.h
fluid/main_ui.o: /usr/include/bits/wchar.h /usr/include/stdlib.h
fluid/main_ui.o: /usr/include/alloca.h
fluid/main_ui.o: ../../newt0/src/newt_core/incs/NewtConf.h
fluid/main_ui.o: ../../newt0/src/newt_core/incs/NewtErrs.h
fluid/main_ui.o: ../../newt0/src/newt_core/incs/NewtFns.h
fluid/main_ui.o: ../../newt0/src/newt_core/incs/NewtEnv.h
fluid/main_ui.o: ../../newt0/src/newt_core/incs/NewtMem.h
fluid/main_ui.o: ../../newt0/src/newt_core/incs/NewtObj.h
fluid/main_ui.o: /usr/local/include/FL/fl_ask.h
fluid/main_ui.o: /usr/local/include/FL/Fl_Double_Window.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Menu_Bar.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Input.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Input_.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Tile.H
fluid/main_ui.o: /usr/local/include/FL/Fl_Image.H
fluid/Fldtk_Prefs.o: fluid/Fldtk_Prefs.h /usr/local/include/FL/Fl.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Enumerations.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Export.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Preferences.H
fluid/Fldtk_Prefs.o: /usr/include/stdio.h /usr/include/features.h
fluid/Fldtk_Prefs.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
fluid/Fldtk_Prefs.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
fluid/Fldtk_Prefs.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
fluid/Fldtk_Prefs.o: /usr/include/libio.h /usr/include/_G_config.h
fluid/Fldtk_Prefs.o: /usr/include/wchar.h /usr/include/bits/wchar.h
fluid/Fldtk_Prefs.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
fluid/Fldtk_Prefs.o: /usr/include/bits/sys_errlist.h
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Group.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Widget.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Check_Button.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Light_Button.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Button.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Box.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Choice.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Menu_.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Menu_Item.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Image.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Input.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Input_.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Button.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Double_Window.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Window.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Group.H
fluid/Fldtk_Prefs.o: /usr/local/include/FL/Fl_Tabs.H
fluid/Fldtk_Inspector.o: fluid/Fldtk_Inspector.h /usr/local/include/FL/Fl.H
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Enumerations.H
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Fl_Export.H
fluid/Fldtk_Inspector.o: fltk/Flio_Inspector.h fltk/Flio_MNP4_Serial.h
fluid/Fldtk_Inspector.o: fltk/Flio_Serial_Port.h /usr/local/include/FL/x.H
fluid/Fldtk_Inspector.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
fluid/Fldtk_Inspector.o: /usr/include/features.h /usr/include/sys/cdefs.h
fluid/Fldtk_Inspector.o: /usr/include/bits/wordsize.h
fluid/Fldtk_Inspector.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
fluid/Fldtk_Inspector.o: /usr/include/bits/types.h
fluid/Fldtk_Inspector.o: /usr/include/bits/typesizes.h /usr/include/time.h
fluid/Fldtk_Inspector.o: /usr/include/endian.h /usr/include/bits/endian.h
fluid/Fldtk_Inspector.o: /usr/include/sys/select.h /usr/include/bits/select.h
fluid/Fldtk_Inspector.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
fluid/Fldtk_Inspector.o: /usr/include/sys/sysmacros.h
fluid/Fldtk_Inspector.o: /usr/include/bits/pthreadtypes.h
fluid/Fldtk_Inspector.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
fluid/Fldtk_Inspector.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
fluid/Fldtk_Inspector.o: /usr/include/X11/keysym.h
fluid/Fldtk_Inspector.o: /usr/include/X11/keysymdef.h
fluid/Fldtk_Inspector.o: /usr/include/X11/Xatom.h
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Fl_Window.H
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Fl_Group.H
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Fl_Box.H fltk/Flmm_Console.h
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Fl_Text_Editor.H
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Fl_Text_Display.H
fluid/Fldtk_Inspector.o: /usr/local/include/FL/fl_draw.H
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Fl_Widget.H
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Fl_Scrollbar.H
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Fl_Slider.H
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Fl_Valuator.H
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Fl_Text_Buffer.H main.h
fluid/Fldtk_Inspector.o: globals.h /usr/local/include/FL/Fl_Group.H
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Fl_Button.H
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Fl_Choice.H
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Fl_Menu_.H
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Fl_Menu_Item.H
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Fl_Image.H
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Fl_Pixmap.H
fluid/Fldtk_Inspector.o: /usr/local/include/FL/Fl_Image.H
globals.o: main.h globals.h /usr/local/include/FL/x.H
globals.o: /usr/local/include/FL/Enumerations.H
globals.o: /usr/local/include/FL/Fl_Export.H /usr/include/X11/Xlib.h
globals.o: /usr/include/sys/types.h /usr/include/features.h
globals.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
globals.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
globals.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
globals.o: /usr/include/time.h /usr/include/endian.h
globals.o: /usr/include/bits/endian.h /usr/include/sys/select.h
globals.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
globals.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
globals.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
globals.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
globals.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
globals.o: /usr/include/X11/keysymdef.h /usr/include/X11/Xatom.h
globals.o: /usr/local/include/FL/Fl_Window.H /usr/local/include/FL/Fl_Group.H
globals.o: /usr/local/include/FL/Fl.H /usr/local/include/FL/fl_file_chooser.h
globals.o: /usr/local/include/FL/Fl_Double_Window.H /usr/include/stdio.h
globals.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
globals.o: /usr/include/bits/wchar.h /usr/include/gconv.h
globals.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
globals.o: /usr/include/stdlib.h /usr/include/alloca.h /usr/include/string.h
globals.o: /usr/local/include/FL/Fl_Group.H /usr/local/include/FL/Fl_Widget.H
globals.o: /usr/local/include/FL/Fl_Choice.H /usr/local/include/FL/Fl_Menu_.H
globals.o: /usr/local/include/FL/Fl_Menu_Item.H
globals.o: /usr/local/include/FL/Fl_Image.H
globals.o: /usr/local/include/FL/Fl_Menu_Button.H
globals.o: /usr/local/include/FL/Fl_Button.H
globals.o: /usr/local/include/FL/Fl_Preferences.H
globals.o: /usr/local/include/FL/Fl_Tile.H
globals.o: /usr/local/include/FL/Fl_File_Browser.H
globals.o: /usr/local/include/FL/Fl_Browser.H
globals.o: /usr/local/include/FL/Fl_Browser_.H
globals.o: /usr/local/include/FL/Fl_Scrollbar.H
globals.o: /usr/local/include/FL/Fl_Slider.H
globals.o: /usr/local/include/FL/Fl_Valuator.H
globals.o: /usr/local/include/FL/Fl_File_Icon.H /usr/local/include/FL/Fl.H
globals.o: /usr/local/include/FL/filename.H /usr/local/include/FL/Fl_Box.H
globals.o: /usr/local/include/FL/Fl_Check_Button.H
globals.o: /usr/local/include/FL/Fl_Light_Button.H
globals.o: /usr/local/include/FL/Fl_Button.H
globals.o: /usr/local/include/FL/Fl_File_Input.H
globals.o: /usr/local/include/FL/Fl_Input.H /usr/local/include/FL/Fl_Input_.H
globals.o: /usr/local/include/FL/Fl_Return_Button.H
globals.o: /usr/local/include/FL/fl_ask.H fluid/Fldtk_Prefs.h
globals.o: /usr/local/include/FL/Fl_Tabs.H fluid/Fldtk_Proj_Settings.h
globals.o: fltk/Flmm_Set_Input.h fltk/Flmm_Set_Check_Button.h
globals.o: /usr/local/include/FL/Fl_Round_Button.H
globals.o: /usr/local/include/FL/Fl_Value_Output.H
globals.o: /usr/local/include/FL/Fl_Text_Editor.H
globals.o: /usr/local/include/FL/Fl_Text_Display.H
globals.o: /usr/local/include/FL/fl_draw.H
globals.o: /usr/local/include/FL/Fl_Text_Buffer.H fluid/main_ui.h
globals.o: fltk/Flio_Inspector.h fltk/Flio_MNP4_Serial.h
globals.o: fltk/Flio_Serial_Port.h fltk/Flmm_Console.h
globals.o: fluid/Fldtk_Inspector.h fltk/Fldtk_Document_Tabs.h
globals.o: fltk/Fldtk_Document_Browser.h
globals.o: /usr/local/include/FL/Fl_Select_Browser.H
globals.o: fltk/Fldtk_Layout_Drop_Box.h /usr/local/include/FL/Fl_Window.H
globals.o: nt/Nt_Group.h ../../newt0/src/newt_core/incs/NewtCore.h
globals.o: ../../newt0/src/newt_core/incs/NewtType.h
globals.o: ../../newt0/src/newt_core/incs/platform.h ../../newt0/src/config.h
globals.o: /usr/include/stdint.h ../../newt0/src/newt_core/incs/NewtConf.h
globals.o: ../../newt0/src/newt_core/incs/NewtErrs.h
globals.o: ../../newt0/src/newt_core/incs/NewtFns.h
globals.o: ../../newt0/src/newt_core/incs/NewtEnv.h
globals.o: ../../newt0/src/newt_core/incs/NewtMem.h
globals.o: ../../newt0/src/newt_core/incs/NewtObj.h
globals.o: /usr/local/include/FL/fl_ask.h /usr/local/include/FL/Fl_Menu_Bar.H
globals.o: dtk/Dtk_Document_Manager.h ./dtk/Dtk_Document.h dtk/Dtk_Project.h
globals.o: dtk/Dtk_Error.h fltk/Flmm_Message.H fltk/Flio_Serial_Port.h
globals.o: ../../newt0/src/newt_core/incs/NewtBC.h
globals.o: ../../newt0/src/newt_core/incs/NewtParser.h
globals.o: ../../newt0/src/newt_core/incs/NewtNSOF.h
globals.o: ../../newt0/src/newt_core/incs/NewtPkg.h
globals.o: ../../newt0/src/newt_core/incs/NewtPrint.h /usr/include/assert.h
globals.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
globals.o: /usr/include/sys/socket.h /usr/include/sys/uio.h
globals.o: /usr/include/bits/uio.h /usr/include/bits/socket.h
globals.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
globals.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
globals.o: /usr/include/bits/posix2_lim.h /usr/include/bits/sockaddr.h
globals.o: /usr/include/asm/socket.h /usr/include/asm-i386/socket.h
globals.o: /usr/include/asm/sockios.h /usr/include/asm-i386/sockios.h
globals.o: /usr/include/bits/in.h /usr/include/bits/byteswap.h
main.o: main.h globals.h /usr/local/include/FL/Fl_Window.H
main.o: /usr/local/include/FL/Fl_Tooltip.H /usr/local/include/FL/Fl.H
main.o: /usr/local/include/FL/Enumerations.H
main.o: /usr/local/include/FL/Fl_Export.H /usr/local/include/FL/Fl_Widget.H
main.o: /usr/local/include/FL/Fl_Button.H
main.o: /usr/local/include/FL/Fl_XPM_Image.H
main.o: /usr/local/include/FL/Fl_Pixmap.H /usr/local/include/FL/Fl_Image.H
main.o: fltk/Flio_Serial_Port.h fluid/Fldtk_Prefs.h
main.o: /usr/local/include/FL/Fl_Preferences.H /usr/include/stdio.h
main.o: /usr/include/features.h /usr/include/sys/cdefs.h
main.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
main.o: /usr/include/gnu/stubs-32.h /usr/include/bits/types.h
main.o: /usr/include/bits/typesizes.h /usr/include/libio.h
main.o: /usr/include/_G_config.h /usr/include/wchar.h
main.o: /usr/include/bits/wchar.h /usr/include/gconv.h
main.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
main.o: /usr/local/include/FL/Fl_Group.H /usr/local/include/FL/Fl_Widget.H
main.o: /usr/local/include/FL/Fl_Check_Button.H
main.o: /usr/local/include/FL/Fl_Light_Button.H
main.o: /usr/local/include/FL/Fl_Button.H /usr/local/include/FL/Fl_Box.H
main.o: /usr/local/include/FL/Fl_Choice.H /usr/local/include/FL/Fl_Menu_.H
main.o: /usr/local/include/FL/Fl_Menu_Item.H /usr/local/include/FL/Fl_Input.H
main.o: /usr/local/include/FL/Fl_Input_.H
main.o: /usr/local/include/FL/Fl_Double_Window.H
main.o: /usr/local/include/FL/Fl_Window.H /usr/local/include/FL/Fl_Group.H
main.o: /usr/local/include/FL/Fl_Tabs.H fluid/Fldtk_Proj_Settings.h
main.o: fltk/Flmm_Set_Input.h fltk/Flmm_Set_Check_Button.h
main.o: /usr/local/include/FL/Fl_Round_Button.H
main.o: /usr/local/include/FL/Fl_Value_Output.H
main.o: /usr/local/include/FL/Fl_Valuator.H
main.o: /usr/local/include/FL/Fl_Text_Editor.H
main.o: /usr/local/include/FL/Fl_Text_Display.H
main.o: /usr/local/include/FL/fl_draw.H /usr/local/include/FL/Fl_Scrollbar.H
main.o: /usr/local/include/FL/Fl_Slider.H
main.o: /usr/local/include/FL/Fl_Text_Buffer.H fluid/main_ui.h
main.o: fltk/Flio_Inspector.h fltk/Flio_MNP4_Serial.h fltk/Flio_Serial_Port.h
main.o: /usr/local/include/FL/x.H /usr/include/X11/Xlib.h
main.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
main.o: /usr/include/bits/endian.h /usr/include/sys/select.h
main.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
main.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
main.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
main.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
main.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
main.o: /usr/include/X11/keysymdef.h /usr/include/X11/Xatom.h
main.o: fltk/Flmm_Console.h fluid/Fldtk_Inspector.h
main.o: fltk/Fldtk_Document_Tabs.h fltk/Fldtk_Document_Browser.h
main.o: /usr/local/include/FL/Fl_Select_Browser.H
main.o: /usr/local/include/FL/Fl_Browser.H
main.o: /usr/local/include/FL/Fl_Browser_.H fltk/Fldtk_Layout_Drop_Box.h
main.o: nt/Nt_Group.h ../../newt0/src/newt_core/incs/NewtCore.h
main.o: ../../newt0/src/newt_core/incs/NewtType.h
main.o: ../../newt0/src/newt_core/incs/platform.h ../../newt0/src/config.h
main.o: /usr/include/stdint.h /usr/include/stdlib.h /usr/include/alloca.h
main.o: ../../newt0/src/newt_core/incs/NewtConf.h
main.o: ../../newt0/src/newt_core/incs/NewtErrs.h
main.o: ../../newt0/src/newt_core/incs/NewtFns.h
main.o: ../../newt0/src/newt_core/incs/NewtEnv.h
main.o: ../../newt0/src/newt_core/incs/NewtMem.h
main.o: ../../newt0/src/newt_core/incs/NewtObj.h
main.o: /usr/local/include/FL/fl_ask.h /usr/local/include/FL/Fl_Menu_Bar.H
main.o: /usr/local/include/FL/Fl_Tile.H images/toolbox_open.xpm
main.o: images/toolbox_closed.xpm dtk/Dtk_Document_Manager.h
main.o: ../../newt0/src/newt_core/incs/NewtBC.h
main.o: ../../newt0/src/newt_core/incs/NewtParser.h
main.o: ../../newt0/src/newt_core/incs/NewtNSOF.h
main.o: ../../newt0/src/newt_core/incs/NewtPkg.h
main.o: ../../newt0/src/newt_core/incs/NewtPrint.h
sandbox.o: main.h globals.h /usr/local/include/FL/x.H
sandbox.o: /usr/local/include/FL/Enumerations.H
sandbox.o: /usr/local/include/FL/Fl_Export.H /usr/include/X11/Xlib.h
sandbox.o: /usr/include/sys/types.h /usr/include/features.h
sandbox.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
sandbox.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
sandbox.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
sandbox.o: /usr/include/time.h /usr/include/endian.h
sandbox.o: /usr/include/bits/endian.h /usr/include/sys/select.h
sandbox.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
sandbox.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
sandbox.o: /usr/include/bits/pthreadtypes.h /usr/include/X11/X.h
sandbox.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
sandbox.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
sandbox.o: /usr/include/X11/keysymdef.h /usr/include/X11/Xatom.h
sandbox.o: /usr/local/include/FL/Fl_Window.H /usr/local/include/FL/Fl_Group.H
sandbox.o: /usr/local/include/FL/Fl.H /usr/local/include/FL/Fl_Window.H
sandbox.o: /usr/local/include/FL/Fl_Tooltip.H
sandbox.o: /usr/local/include/FL/Fl_Widget.H
sandbox.o: /usr/local/include/FL/Fl_Button.H
sandbox.o: /usr/local/include/FL/Fl_XPM_Image.H
sandbox.o: /usr/local/include/FL/Fl_Pixmap.H /usr/local/include/FL/Fl_Image.H
sandbox.o: /usr/local/include/FL/Fl_Preferences.H /usr/include/stdio.h
sandbox.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
sandbox.o: /usr/include/bits/wchar.h /usr/include/gconv.h
sandbox.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
sandbox.o: /usr/local/include/FL/Fl_Text_Editor.H
sandbox.o: /usr/local/include/FL/Fl_Text_Display.H
sandbox.o: /usr/local/include/FL/fl_draw.H /usr/local/include/FL/Fl_Widget.H
sandbox.o: /usr/local/include/FL/Fl_Scrollbar.H
sandbox.o: /usr/local/include/FL/Fl_Slider.H
sandbox.o: /usr/local/include/FL/Fl_Valuator.H
sandbox.o: /usr/local/include/FL/Fl_Text_Buffer.H
sandbox.o: /usr/local/include/FL/Fl_Text_Buffer.H
sandbox.o: /usr/local/include/FL/fl_file_chooser.h
sandbox.o: /usr/local/include/FL/Fl_Double_Window.H /usr/include/stdlib.h
sandbox.o: /usr/include/alloca.h /usr/include/string.h
sandbox.o: /usr/local/include/FL/Fl_Group.H /usr/local/include/FL/Fl_Choice.H
sandbox.o: /usr/local/include/FL/Fl_Menu_.H
sandbox.o: /usr/local/include/FL/Fl_Menu_Item.H
sandbox.o: /usr/local/include/FL/Fl_Menu_Button.H
sandbox.o: /usr/local/include/FL/Fl_Tile.H
sandbox.o: /usr/local/include/FL/Fl_File_Browser.H
sandbox.o: /usr/local/include/FL/Fl_Browser.H
sandbox.o: /usr/local/include/FL/Fl_Browser_.H
sandbox.o: /usr/local/include/FL/Fl_File_Icon.H /usr/local/include/FL/Fl.H
sandbox.o: /usr/local/include/FL/filename.H /usr/local/include/FL/Fl_Box.H
sandbox.o: /usr/local/include/FL/Fl_Check_Button.H
sandbox.o: /usr/local/include/FL/Fl_Light_Button.H
sandbox.o: /usr/local/include/FL/Fl_Button.H
sandbox.o: /usr/local/include/FL/Fl_File_Input.H
sandbox.o: /usr/local/include/FL/Fl_Input.H /usr/local/include/FL/Fl_Input_.H
sandbox.o: /usr/local/include/FL/Fl_Return_Button.H
sandbox.o: /usr/local/include/FL/fl_ask.H fluid/Fldtk_Prefs.h
sandbox.o: /usr/local/include/FL/Fl_Tabs.H fluid/Fldtk_Proj_Settings.h
sandbox.o: fltk/Flmm_Set_Input.h fltk/Flmm_Set_Check_Button.h
sandbox.o: /usr/local/include/FL/Fl_Round_Button.H
sandbox.o: /usr/local/include/FL/Fl_Value_Output.H dtk/Dtk_Document_Manager.h
sandbox.o: ../../newt0/src/newt_core/incs/NewtType.h
sandbox.o: ../../newt0/src/newt_core/incs/platform.h ../../newt0/src/config.h
sandbox.o: /usr/include/stdint.h ../../newt0/src/newt_core/incs/NewtConf.h
sandbox.o: ./dtk/Dtk_Document.h dtk/Dtk_Project.h fluid/main_ui.h
sandbox.o: fltk/Flio_Inspector.h fltk/Flio_MNP4_Serial.h
sandbox.o: fltk/Flio_Serial_Port.h fltk/Flmm_Console.h
sandbox.o: fluid/Fldtk_Inspector.h fltk/Fldtk_Document_Tabs.h
sandbox.o: fltk/Fldtk_Document_Browser.h
sandbox.o: /usr/local/include/FL/Fl_Select_Browser.H
sandbox.o: fltk/Fldtk_Layout_Drop_Box.h nt/Nt_Group.h
sandbox.o: ../../newt0/src/newt_core/incs/NewtCore.h
sandbox.o: ../../newt0/src/newt_core/incs/NewtErrs.h
sandbox.o: ../../newt0/src/newt_core/incs/NewtFns.h
sandbox.o: ../../newt0/src/newt_core/incs/NewtEnv.h
sandbox.o: ../../newt0/src/newt_core/incs/NewtMem.h
sandbox.o: ../../newt0/src/newt_core/incs/NewtObj.h
sandbox.o: /usr/local/include/FL/fl_ask.h /usr/local/include/FL/Fl_Menu_Bar.H
sandbox.o: fltk/Flio_Serial_Port.h images/toolbox_open.xpm
sandbox.o: images/toolbox_closed.xpm /usr/include/assert.h
sandbox.o: ../../newt0/src/newt_core/incs/NewtBC.h
sandbox.o: ../../newt0/src/newt_core/incs/NewtParser.h
sandbox.o: ../../newt0/src/newt_core/incs/NewtNSOF.h
sandbox.o: ../../newt0/src/newt_core/incs/NewtPkg.h
sandbox.o: ../../newt0/src/newt_core/incs/NewtPrint.h
nt/Nt_Group.o: nt/Nt_Group.h /usr/local/include/FL/Fl_Group.H
nt/Nt_Group.o: /usr/local/include/FL/Fl_Widget.H
nt/Nt_Group.o: /usr/local/include/FL/Enumerations.H
nt/Nt_Group.o: /usr/local/include/FL/Fl_Export.H /usr/include/stdio.h
nt/Nt_Group.o: /usr/include/features.h /usr/include/sys/cdefs.h
nt/Nt_Group.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
nt/Nt_Group.o: /usr/include/gnu/stubs-32.h /usr/include/bits/types.h
nt/Nt_Group.o: /usr/include/bits/typesizes.h /usr/include/libio.h
nt/Nt_Group.o: /usr/include/_G_config.h /usr/include/wchar.h
nt/Nt_Group.o: /usr/include/bits/wchar.h /usr/include/gconv.h
nt/Nt_Group.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
nt/Nt_Group.o: /usr/include/stdlib.h /usr/include/sys/types.h
nt/Nt_Group.o: /usr/include/time.h /usr/include/endian.h
nt/Nt_Group.o: /usr/include/bits/endian.h /usr/include/sys/select.h
nt/Nt_Group.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
nt/Nt_Group.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
nt/Nt_Group.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
nt/Nt_Group.o: /usr/include/string.h
