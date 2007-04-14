//
// "$Id$"
//
// DyneTK header file for global functions
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


#ifndef _DTK_GLOBALS_H_
#define _DTK_GLOBALS_H_


// global functions for general access. The functions are used
// by the main menu and icon bars.

int		NewLayoutFile(const char *filename=0L);
int		NewTextFile(const char *filename=0L);
int		OpenDocument(const char *filename=0L);
int		SaveCurrentDocument();
int		SaveCurrentDocumentAs();
void	CloseCurrentDocument();
void	ExitApplication();

void	EditPreferences();

int		NewProject(const char *filename=0L);
int		OpenProject(const char *filename=0L);
int		SaveProject();
int		SaveProjectAs();
void	ShowProjectSettings();
int		AddCurrentDocToProject();
int		AddFileToProject(const char *filename=0L);
int		RemoveFileFromProject(const char *filename=0L);
int		BuildPackage();
int		DownloadPackage();
int		LaunchPackage();

int		InspectorSendScript(const char *script);
int		InspectorSendPackage(const char *filename, const char *symbol);
int		InspectorLaunchPackage(const char *symbol);
void	InspectorConnect();
void	InspectorCancelConnect();
void	InspectorStopOnThrows();
void	InspectorTraceOff();
void	InspectorStackTrace();
void	InspectorExitBreakLoop();
void	InspectorPrintDepth(int);
void	InspectorSnapshot();

void	UpdateMainMenu();

#endif


//
// End of "$Id$".
//
