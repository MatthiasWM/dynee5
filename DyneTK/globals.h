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

#include "allNewt.h"

class Dtk_Document;

// global functions for general access. The functions are used
// by the main menu and icon bars.

/*---------------------------------------------------------------------------*/
// File menu
int		NewLayout(const char *filename=0L);
//      NewProtoTemplate()
int		NewTextFile(const char *filename=0L);
int		OpenDocument(const char *filename=0L);
//      LinkLayout()
void	CloseCurrentDocument();
int		SaveCurrentDocument();
int		SaveCurrentDocumentAs();
//      SaveAllDocuments()
//      RevertCurrentDocument()
//      PrintSetup()
//      PrintOne()
//      PrintCurrentDocument()
//      open previous file
void	ExitApplication();

/*---------------------------------------------------------------------------*/
// Edit menu
//      Undo()
//      Redo()
//      Cut()
//      Copy()
//      Paste()
//      Clear()
//      Duplicate()
//      ShiftLeft()
//      ShiftRight()
//      SelectAll()
//      SelectHierarchy()
//      SelectInLayout()
//      Search()
//      Find()
//      FindNext()
//      FindInherited()
//      NewtScreenShot()
void	EditPreferences();

/*---------------------------------------------------------------------------*/
// Project menu
int		NewProject(const char *filename=0L);

/** Open an existing project.
 *
 * If no filename is given, we ask the user for a filename by popping up a 
 * file chooser dialog. We then close the current project and all its
 * documents, and finally load the new project and all dependents.
 *
 * \param filename [in] path and file name of .ntk file to open
 *
 * \retval	0 if successful
 * \retval  -1 (or an error code <0) if the function failed
 * \retval  -2 if the user aborted anywhere in the process
 */
int		OpenProject(const char *filename=0L);

int		SaveProject();
int		SaveProjectAs();

/** Close the current project and all associted documents.
 *
 * Ask user for confimation if the project is dirty.
 * Close and delete all associated resources.
 * Delete the project and remove all remaining links.
 *
 * \retval  0 if successful
 * \retval  -1 (or an error code <0) if the function failed
 * \retval  -2 if the user canceled
 */
int     CloseProject();

int		AddCurrentDocToProject();
int		AddFileToProject(const char *filename=0L);

/** Remove a document from its project.
 *
 * Confirm removal if document is dirty.
 * Then close the document and remove it from the list.
 *
 * \param   [in] document address
 *
 * \retval  0, if successful
 * \retval  negative if any part of the operation failed
 */
int		RemoveFileFromProject(Dtk_Document *document=0L);
//      UpdateFiles()
int		BuildPackage();
int		DownloadPackage();
int		LaunchPackage();
//      ExportPackageToText()
//      InstallToolkitApp()
//      MarkAsMainLayout()
//      ProcessEarlier()
//      ProcessLater()
void	ShowProjectSettings();

// Layout menu
//      LayoutSize()
//      AutogridToggle()
//      SetGrid()
//      MoveToFront()
//      MoveForward()
//      MoveToBack()
//      MoveBackward()
//      SetAlignment()
//      Align()
//      PreviewLayout()

/*---------------------------------------------------------------------------*/
// Browser menu
//      TemplateInfo()
//      NewSlot()
//      RenameSlot()
//      TemplatedByType()
//      TemplatesByHierarchy()
//      SlotsByName()
//      SlotsByType()
//      ShowSlotValues()
//      ApplyChanges()
//      RevertChanges()

/*---------------------------------------------------------------------------*/
// Window menu
//      OpenInspector()
void	InspectorConnect();
//      NewBrowser()
//      OpenLayout()
//      Cascade()
//      Tile()
//      ArrangeIcons()
//      SetDefaultWindowPosition()

/*---------------------------------------------------------------------------*/
// Help menu
//      HelpContents()
//      HelpErrorCode()
//      UsingHelp()
//      HelpAbout()

/*---------------------------------------------------------------------------*/

/** Find the document that was last touched by the user.
 *
 * \retval   address of document or NULL
 */
Dtk_Document *GetCurrentDocument();

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
void	InspectorSnapshotUpdate(newtRef);

void	UpdateMainMenu();

void	NewtAlert(int err);
void	SystemAlert(const char *msg, int err=0xdecaff);

void	DebugDumpPackage();
void	DebugDumpNSOF();
void	DebugDumpRsrc();
void	DebugDumpBuffer(uint8_t*, int);

#endif


//
// End of "$Id$".
//
