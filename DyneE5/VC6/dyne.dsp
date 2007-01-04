# Microsoft Developer Studio Project File - Name="dyne" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=dyne - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dyne.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dyne.mak" CFG="dyne - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dyne - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "dyne - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dyne - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /GX /Os /Ob2 /I "." /I "../src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /D "VC_EXTRA_LEAN" /D "WIN32_EXTRA_LEAN" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 fltkgl.lib opengl32.lib flmm.lib fltk.lib wsock32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcd" /out:"../dyne.exe" /libpath:"..\lib"
# SUBTRACT LINK32 /pdb:none /incremental:yes

!ELSEIF  "$(CFG)" == "dyne - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dyne_"
# PROP BASE Intermediate_Dir "dyne_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dyne_"
# PROP Intermediate_Dir "dyne_"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /Gm /GX /ZI /Od /I "." /I "../src" /D "MODET" /D "NO_DBCT" /D "MODE32" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /D "VC_EXTRA_LEAN" /D "WIN32_EXTRA_LEAN" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 fltkgld.lib opengl32.lib flmmd.lib fltkgld.lib fltkd.lib opengl32.lib wsock32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd" /out:"../src/dyned.exe" /pdbtype:sept /libpath:"..\lib"
# SUBTRACT LINK32 /pdb:none /incremental:no

!ENDIF 

# Begin Target

# Name "dyne - Win32 Release"
# Name "dyne - Win32 Debug"
# Begin Group "dyne"

# PROP Default_Filter ""
# Begin Group "dyne_headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\asmview.h
# End Source File
# Begin Source File

SOURCE=..\src\cpu.h
# End Source File
# Begin Source File

SOURCE=..\src\dyne.h
# End Source File
# Begin Source File

SOURCE=..\src\flash.h
# End Source File
# Begin Source File

SOURCE=..\src\hexview.h
# End Source File
# Begin Source File

SOURCE=..\src\main.h
# End Source File
# Begin Source File

SOURCE=..\src\mainc.h
# End Source File
# Begin Source File

SOURCE=..\src\memory.h
# End Source File
# Begin Source File

SOURCE=..\src\registerview.h
# End Source File
# Begin Source File

SOURCE=..\src\screen.h
# End Source File
# Begin Source File

SOURCE=..\src\symbols.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\asmview.cxx
# End Source File
# Begin Source File

SOURCE=..\src\cpu.cxx
# End Source File
# Begin Source File

SOURCE=..\src\desym.cxx
# End Source File
# Begin Source File

SOURCE=..\src\dyne.cxx
# End Source File
# Begin Source File

SOURCE=..\src\flash.cxx
# End Source File
# Begin Source File

SOURCE=..\src\hexview.cxx
# End Source File
# Begin Source File

SOURCE=..\src\main.cxx
# End Source File
# Begin Source File

SOURCE=..\src\memory.cxx
# End Source File
# Begin Source File

SOURCE=..\src\native.cxx
# End Source File
# Begin Source File

SOURCE=..\src\registerview.cxx
# End Source File
# Begin Source File

SOURCE=..\src\screen.cxx
# End Source File
# Begin Source File

SOURCE=..\src\symbols.cxx
# End Source File
# End Group
# Begin Group "armulator"

# PROP Default_Filter ""
# Begin Group "armul_headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\armulator\armdefs.h
# End Source File
# Begin Source File

SOURCE=..\src\armulator\armemu.h
# End Source File
# Begin Source File

SOURCE=..\src\armulator\armmem.h
# End Source File
# Begin Source File

SOURCE=..\src\armulator\armmmu.h
# End Source File
# Begin Source File

SOURCE=..\src\armulator\armos.h
# End Source File
# Begin Source File

SOURCE=..\src\armulator\mmu\cache.h
# End Source File
# Begin Source File

SOURCE=..\src\armulator\mmu\rb.h
# End Source File
# Begin Source File

SOURCE=..\src\armulator\mmu\sa_mmu.h
# End Source File
# Begin Source File

SOURCE=..\src\armulator\mmu\tlb.h
# End Source File
# Begin Source File

SOURCE=..\src\armulator\mmu\wb.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\armulator\armcopro.c
# End Source File
# Begin Source File

SOURCE=..\src\armulator\armemu.c
# End Source File
# Begin Source File

SOURCE=..\src\armulator\arminit.c
# End Source File
# Begin Source File

SOURCE=..\src\armulator\armmem.c
# End Source File
# Begin Source File

SOURCE=..\src\armulator\armmmu.c
# End Source File
# Begin Source File

SOURCE=..\src\armulator\armos.c
# End Source File
# Begin Source File

SOURCE=..\src\armulator\armsupp.c
# End Source File
# Begin Source File

SOURCE=..\src\armulator\armvirt.c
# End Source File
# Begin Source File

SOURCE=..\src\armulator\mmu\cache.c
# End Source File
# Begin Source File

SOURCE=..\src\armulator\mmu\rb.c
# End Source File
# Begin Source File

SOURCE=..\src\armulator\mmu\sa_mmu.c
# End Source File
# Begin Source File

SOURCE=..\src\armulator\thumbemu.c
# End Source File
# Begin Source File

SOURCE=..\src\armulator\mmu\tlb.c
# End Source File
# Begin Source File

SOURCE=..\src\armulator\mmu\wb.c
# End Source File
# Begin Source File

SOURCE=..\src\armulator\wrapper.c
# End Source File
# End Group
# Begin Group "disarm"

# PROP Default_Filter ""
# Begin Group "dis_headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\src\disarm\arm-opc-cc.h"
# End Source File
# Begin Source File

SOURCE="..\src\disarm\arm-opc.h"
# End Source File
# End Group
# Begin Source File

SOURCE="..\src\disarm\roughdisarm-cc.c"
# End Source File
# Begin Source File

SOURCE=..\src\disarm\roughdisarm.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\breakpoints

!IF  "$(CFG)" == "dyne - Win32 Release"

!ELSEIF  "$(CFG)" == "dyne - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gpio.txt

!IF  "$(CFG)" == "dyne - Win32 Release"

!ELSEIF  "$(CFG)" == "dyne - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gpio_doc.txt

!IF  "$(CFG)" == "dyne - Win32 Release"

!ELSEIF  "$(CFG)" == "dyne - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\docs\MP2100Image.sym

!IF  "$(CFG)" == "dyne - Win32 Release"

!ELSEIF  "$(CFG)" == "dyne - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Target
# End Project
