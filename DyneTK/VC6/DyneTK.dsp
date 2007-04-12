# Microsoft Developer Studio Project File - Name="DyneTK" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DyneTK - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DyneTK.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DyneTK.mak" CFG="DyneTK - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DyneTK - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DyneTK - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DyneTK - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DyneTK_Release"
# PROP Intermediate_Dir "DyneTK_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /GX /Os /Ob2 /I "." /I ".." /I "..\..\..\Newt0\src\newt_core\incs" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /D "VC_EXTRA_LEAN" /D "WIN32_EXTRA_LEAN" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 newt.lib fltk.lib wsock32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386 /out:"../test/DyneTK.exe" /libpath:"..\lib" /libpath:"..\..\..\NEWT0\build"
# SUBTRACT LINK32 /pdb:none /incremental:yes

!ELSEIF  "$(CFG)" == "DyneTK - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DyneTK_"
# PROP BASE Intermediate_Dir "DyneTK_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DyneTK_Debug"
# PROP Intermediate_Dir "DyneTK_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /Gm /GX /ZI /Od /I "." /I ".." /I "..\..\..\Newt0\src\newt_core\incs" /I "..\..\..\Newt0\src\newt_core\incs\VC6" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /D "VC_EXTRA_LEAN" /D "WIN32_EXTRA_LEAN" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 newtd.lib fltkd.lib wsock32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd" /out:"../test/DyneTKd.exe" /pdbtype:sept /libpath:"..\lib" /libpath:"..\..\..\NEWT0\build"
# SUBTRACT LINK32 /pdb:none /incremental:no

!ENDIF 

# Begin Target

# Name "DyneTK - Win32 Release"
# Name "DyneTK - Win32 Debug"
# Begin Group "fluid"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\fluid\Fldtk_Inspector.cxx
# End Source File
# Begin Source File

SOURCE=..\fluid\Fldtk_Inspector.h
# End Source File
# Begin Source File

SOURCE=..\fluid\Fldtk_Prefs.cxx
# End Source File
# Begin Source File

SOURCE=..\fluid\Fldtk_Prefs.h
# End Source File
# Begin Source File

SOURCE=..\fluid\Fldtk_Proj_Settings.cxx
# End Source File
# Begin Source File

SOURCE=..\fluid\Fldtk_Proj_Settings.h
# End Source File
# Begin Source File

SOURCE=..\fluid\main_ui.cxx
# End Source File
# Begin Source File

SOURCE=..\fluid\main_ui.h
# End Source File
# End Group
# Begin Group "fltk"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\fltk\Fldtk_Document_Browser.cxx
# End Source File
# Begin Source File

SOURCE=..\fltk\Fldtk_Document_Browser.h
# End Source File
# Begin Source File

SOURCE=..\fltk\Fldtk_Document_Tabs.cxx
# End Source File
# Begin Source File

SOURCE=..\fltk\Fldtk_Document_Tabs.h
# End Source File
# Begin Source File

SOURCE=..\fltk\Fldtk_Editor.cxx
# End Source File
# Begin Source File

SOURCE=..\fltk\Fldtk_Editor.h
# End Source File
# Begin Source File

SOURCE=..\fltk\Fldtk_Layout_Drop_Box.cxx
# End Source File
# Begin Source File

SOURCE=..\fltk\Fldtk_Layout_Drop_Box.h
# End Source File
# Begin Source File

SOURCE=..\fltk\Fldtk_Layout_Editor.cxx
# End Source File
# Begin Source File

SOURCE=..\fltk\Fldtk_Layout_Editor.h
# End Source File
# Begin Source File

SOURCE=..\fltk\Fldtk_Script_Editor.cxx
# End Source File
# Begin Source File

SOURCE=..\fltk\Fldtk_Script_Editor.h
# End Source File
# Begin Source File

SOURCE=..\fltk\Flio_Inspector.cxx
# End Source File
# Begin Source File

SOURCE=..\fltk\Flio_Inspector.h
# End Source File
# Begin Source File

SOURCE=..\fltk\Flio_MNP4_Serial.cxx
# End Source File
# Begin Source File

SOURCE=..\fltk\Flio_MNP4_Serial.h
# End Source File
# Begin Source File

SOURCE=..\fltk\Flio_Serial_Port.cxx
# End Source File
# Begin Source File

SOURCE=..\fltk\Flio_Serial_Port.h
# End Source File
# Begin Source File

SOURCE=..\fltk\Flmm_Console.cxx
# End Source File
# Begin Source File

SOURCE=..\fltk\Flmm_Console.h
# End Source File
# Begin Source File

SOURCE=..\fltk\Flmm_Set_Check_Button.cxx
# End Source File
# Begin Source File

SOURCE=..\fltk\Flmm_Set_Check_Button.h
# End Source File
# Begin Source File

SOURCE=..\fltk\Flmm_Set_Input.cxx
# End Source File
# Begin Source File

SOURCE=..\fltk\Flmm_Set_Input.h
# End Source File
# End Group
# Begin Group "Images"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\images\toolbox_closed.xpm
# End Source File
# Begin Source File

SOURCE=..\images\toolbox_inactive.xpm
# End Source File
# Begin Source File

SOURCE=..\images\toolbox_open.xpm
# End Source File
# End Group
# Begin Group "dtk"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\dtk\Dtk_Document.cxx
# End Source File
# Begin Source File

SOURCE=..\dtk\Dtk_Document.h
# End Source File
# Begin Source File

SOURCE=..\Dtk\Dtk_Document_Manager.cxx
# End Source File
# Begin Source File

SOURCE=..\Dtk\Dtk_Document_Manager.h
# End Source File
# Begin Source File

SOURCE=..\dtk\Dtk_Layout_Document.cxx
# End Source File
# Begin Source File

SOURCE=..\dtk\Dtk_Layout_Document.h
# End Source File
# Begin Source File

SOURCE=..\dtk\Dtk_Project.cxx
# End Source File
# Begin Source File

SOURCE=..\dtk\Dtk_Project.h
# End Source File
# Begin Source File

SOURCE=..\dtk\Dtk_Script_Document.cxx
# End Source File
# Begin Source File

SOURCE=..\dtk\Dtk_Script_Document.h
# End Source File
# End Group
# Begin Group "documentation"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\CHANGES
# End Source File
# Begin Source File

SOURCE=..\COPYING
# End Source File
# Begin Source File

SOURCE=..\CREDITS
# End Source File
# Begin Source File

SOURCE=..\README
# End Source File
# End Group
# Begin Group "nt"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\nt\Nt_Group.cxx
# End Source File
# Begin Source File

SOURCE=..\nt\Nt_Group.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\globals.cxx
# End Source File
# Begin Source File

SOURCE=..\globals.h
# End Source File
# Begin Source File

SOURCE=..\main.cxx
# End Source File
# Begin Source File

SOURCE=..\main.h
# End Source File
# Begin Source File

SOURCE=..\sandbox.cxx
# End Source File
# Begin Source File

SOURCE=..\sandbox.h
# End Source File
# End Target
# End Project
