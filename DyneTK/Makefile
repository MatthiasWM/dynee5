
all: dynetk

SRCFILES = \
  dtk/Dtk_Document.cxx \
  dtk/Dtk_Document_List.cxx \
  dtk/Dtk_Error.cxx \
  dtk/Dtk_Layout_Document.cxx \
  dtk/Dtk_Platform.cxx \
  dtk/Dtk_Project.cxx \
  dtk/Dtk_Proto_Slot.cxx \
  dtk/Dtk_Rect_Slot.cxx \
  dtk/Dtk_Script_Document.cxx \
  dtk/Dtk_Script_Slot.cxx \
  dtk/Dtk_Script_Writer.cxx \
  dtk/Dtk_Slot.cxx \
  dtk/Dtk_Slot_List.cxx \
  dtk/Dtk_Template.cxx \
  dtk/Dtk_Template_List.cxx \
  dtk/Dtk_Value_Slot.cxx \
  fltk/Flio_Inspector.cxx \
  fltk/Flio_MNP4_Protocol.cxx \
  fltk/Flio_Pipe.cxx \
  fltk/Flio_Serial_Port.cxx \
  fltk/Flio_Stream.cxx \
  fltk/Flmm_Console.cxx \
  fltk/Flmm_Message.cxx \
  fltk/Flmm_Newt_Script_Editor.cxx \
  fltk/Flmm_Set_Input.cxx \
  fltk/Flmm_Set_Check_Button.cxx \
  fltk/Flmm_Signal.cxx \
  fltk/Flmp_Image.cxx \
  fltk/Fldtk_Document_Browser.cxx \
  fltk/Fldtk_Document_Tabs.cxx \
  fltk/Fldtk_Editor.cxx \
  fltk/Fldtk_Justify_Slot_Editor.cxx \
  fltk/Fldtk_Layout_Drop_Box.cxx \
  fltk/Fldtk_Layout_Editor.cxx \
  fltk/Fldtk_Layout_View.cxx \
  fltk/Fldtk_Proto_Slot_Editor.cxx \
  fltk/Fldtk_Rect_Slot_Editor.cxx \
  fltk/Fldtk_Script_Editor.cxx \
  fltk/Fldtk_Script_Slot_Editor.cxx \
  fltk/Fldtk_Slot_Editor_Group.cxx \
  fltk/Fldtk_Value_Slot_Editor.cxx \
  fluid/Fldtk_Inspector.cxx \
  fluid/Fldtk_Proj_Settings.cxx \
  fluid/Fldtk_Prefs.cxx \
  fluid/Fldtk_Snapshot.cxx \
  fluid/main_ui.cxx \
  flnt/Flnt_Widget.cxx \
  globals.cxx \
  main.cxx \
  sandbox.cxx \
  nt/Nt_Group.cxx

OBJFILES = $(SRCFILES:.cxx=.o)


CXX := $(shell fltk-config --use-images --cxx)

CXXFLAGS := $(shell fltk-config --use-images --cxxflags)

INCLUDES := -I. -Idtk -Ifltk -Ifluid -Int

NEWTINCLUDES := -I../../newt0/src/ -I../../newt0/src/newt_core/incs/

LDFLAGS := $(shell fltk-config --use-images --ldstaticflags) -L../../newt0/build

LIBS := $(shell fltk-config --use-images --libs)


dynetk: $(OBJFILES)
	echo Linking $@...
	$(CXX) -g $(LDFLAGS) -o $@ $(OBJFILES) -lnewt -liconv $(LIBS)

.SUFFIXES: .cxx .o

.cxx.o:
	echo Compiling $<...
	$(CXX) -c -g $(CXXFLAGS) $(INCLUDES) $(NEWTINCLUDES) -o $@ $<

clean:
	rm $(OBJFILES)
	rm dynetk

depend:
	mkdep $(CXXFLAGS) $(INCLUDES) $(NEWTINCLUDES) $(SRCFILES)
	makedepend -- $(CXXFLAGS) $(INCLUDES) $(NEWTINCLUDES) -- $(SRCFILES)


