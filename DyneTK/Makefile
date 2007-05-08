
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
  fltk/Flio_MNP4_Serial.cxx \
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
# DO NOT DELETE
