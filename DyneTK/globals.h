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
class Dtk_Layout_Document;

/** \file globals.h
 * global functions for general access. The functions are used
 * by the main menu and icon bars.
 */

/*---------------------------------------------------------------------------*/
// File menu

/** Create a new Layout file and open it for editing.
 *
 * If no filename is given, name the file with a default name for the current 
 * directory, but mark it so that the "Save" function will still pop up a 
 * file chooser dialog.
 *
 * \param filename optional filename for the new layout file
 *
 * \retval 0 if succeeded
 * \retval negative if an error occured
 */
int		NewLayoutFile(const char *filename=0L);

//      NewProtoTemplate()

/** Open a new text file for editing.
 * 
 * If no filename is given, name the file with a default name for the current 
 * directory, but mark it so that the "Save" function will still pop up a 
 * file chooser dialog.
 *
 * \param filename optional filename for the new text file
 *
 * \retval	0 of successful
 * \retval  -1 (or an error code <0) if the function failed
 */
int		NewTextFile(const char *filename=0L);

/** Open an existing document.
 *
 * If no filename is given, we will pop up a file chooser.
 * The document is not added to the project.
 *
 * \param filename optional filename
 *
 * \retval	0 of successful
 * \retval  -1 (or an error code <0) if the function failed
 *
 * \todo We must make sure that we never have the same document in the databse twice!
 */
int		OpenDocument(const char *filename=0L);

//      LinkLayout()

/** Close the editor of the current document.
 *
 * If the document belongs to a project, it will remain in memory
 * and can be reopened at any time by clicking onto its entry in the browser.
 * If the document does not belong to a project, it will be closed
 * and removed.
 * 
 * \retval 0 if succeeded
 * \retval negative if an error occured
 * \retval -2 if the user canceled
 */
int     CloseCurrentDocument();

/** Find the current document and save it to disk under its current name.
 * 
 * \retval 0 if succeeded
 * \retval negative if an error occured
 */
int		SaveCurrentDocument();

/** Find the current document and save it to disk with a new name.
 * 
 * \retval 0 if succeeded
 * \retval negative if an error occured
 */
int		SaveCurrentDocumentAs();

/** Save all open documents and projects.
 *
 * \retval 0 if succeeded
 * \retval negative if an error occured
 * \retval -2 if the user canceled
 */
int SaveAllDocuments();

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

/** Create a new project.
 *
 * If no filename is given, we ask the user for a filename by popping up a 
 * file chooser dialog
 *
 * \param filename	[in] path and file name of .ntk file to create
 *
 * \retval	0 of successful
 * \retval  -1 (or an error code <0) if the function failed
 */
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

/** Save the current project to a file.
 *
 * \todo We need much better error handling here!
 *
 * \retval	0 if successful
 * \retval  -1 (or an error code <0) if the function failed
 */
int		SaveProject();

/** Ask for a filename and save the current project to that file.
 *
 * \todo We need much better error handling here!
 * \todo Warn if we are about to overwrite an existing file!
 *
 * \retval	0 if successful
 * \retval  -1 (or an error code <0) if the function failed
 * \retval  -2 if the user aborted anywhere in the process
 */
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

/** Add the current document to the active project.
 *
 * If the doc is already part of a project, this function does nothing.
 *
 * \retval 0 if successful or if doc already in project
 * \retval -1 if no document was active
 */
int		AddCurrentDocToProject();

/** Add an existing document to the project.
 *
 * If no filename is given, we will pop up a file chooser.
 * The document is loaded, opened, and added to the project.
 *
 * \param filename optional filename
 *
 * \retval	0 of successful
 * \retval  -1 (or an error code <0) if the function failed
 *
 * \todo We must make sure that we never have the same document in the databse twice!
 */
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

/** Compile a package file and save it to disk.
 *
 * \todo Unfortunatley there is no real error handling with the Newt/0 compiler.
 *
 * \retval  0, if successful
 * \retval  negative if any part of the operation failed
 */
int		BuildPackage();

/** Send the current package through the Inspector to the Newton.
 *
 * \retval  0, if successful
 * \retval  negative if any part of the operation failed
 */
int		DownloadPackage();

/** Launch the application that corresponds to the current package on the Newton.
 *
 * \todo Better inform the user if anything goes wrong.
 *
 * \retval  0, if successful
 * \retval  negative if any part of the operation failed  
 */
int		LaunchPackage();

/** Export all documents in a project into a single script file.
 * 
 * \retval  0, if successful
 * \retval  negative if any part of the operation failed  
 */
int ExportPackageToText();

//      InstallToolkitApp()

/** Set a document as the main layout.
 *
 * \retval  0 if successful
 * \retval  -1 if no document was given, or none active
 * \retval  -2 if the document is not part of a project
 */
int SetMainLayout(Dtk_Document *doc=0L);

//      ProcessEarlier()
//      ProcessLater()

/**
 * Show the project settings dialog.
 */
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

/** Attempt to connect the Inspector to a Newton.
 *
 * This will pop up a cancelation dialog, and the try to connect to a Newton
 * until either the connection is established or the user cancels the operation.
 */
void InspectorConnect();

//      NewBrowser()

/** Open the layout view for the current layout document.
 *
 * If a Layout Document is selected, this function will pop up the related
 * visual layout editor.
 */
int OpenLayoutView(Dtk_Layout_Document *lyt=0L);

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

void    SetModeEditTemplate();
void    SetModeAddTemplate();

#ifdef WIN32
# define  EnterDebugger() \
            __asm { int 3 }
#else
# define  EnterDebugger() \
            __asm__("\n int3\n")
#endif


#endif


//
// End of "$Id$".
//
