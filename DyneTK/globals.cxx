//
// "$Id$"
//
// DyneTK global functions implementation file
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

#include "main.h"

#include <FL/x.H>
#include <FL/Fl.H>
#include <FL/fl_file_chooser.h>

#include "fluid/Fldtk_Prefs.h"
#include "fluid/Fldtk_Proj_Settings.h"
#include "fluid/main_ui.h"

#include "dtk/Dtk_Document_Manager.h"
#include "dtk/Dtk_Document.h"
#include "dtk/Dtk_Project.h"
#include "dtk/Dtk_Error.h"

#include "fltk/Flmm_Message.H"
#include "fltk/Flio_Serial_Port.h"

extern "C" {
#include "NewtCore.h"
#include "NewtBC.h"
#include "NewtNSOF.h"
#include "NewtPkg.h"
#include "NewtPrint.h"
#include "NewtEnv.h"
}

#include <assert.h>

#ifndef WIN32
# define Sleep(a) sleep((a)/1000)
# include <arpa/inet.h>
#else
# include <winsock2.h>
#endif

extern Fl_Window *wConnect;

/*---------------------------------------------------------------------------*/
/**
 * Create a new Layout file and open it for editing.
 */
int NewLayoutFile(const char *filename) {

	if (!filename) {
		filename = "Doc.lyt";
	}

	Dtk_Document *doc = documents->newLayout(filename);
	doc->edit();

	UpdateMainMenu();
	return 0;
}


/*---------------------------------------------------------------------------*/
/** Open a new text file for editing.
 * 
 * If no filename is given, name teh file with a default name for the current 
 * directory, but mark it so that the "Save" function will still pop up a 
 * file chooser dialog.
 *
 * \todo	Write this function.
 *
 * \param filename	[in] path and file name of a .ns file to create
 *
 * \retval	0 of successful
 * \retval  -1 (or an error code <0) if the function failed
 */
int NewTextFile(const char *filename) {

	if (!filename) {
		filename = "Text.txt";
	}

	Dtk_Document *doc = documents->newScript(filename);
	doc->edit();

	UpdateMainMenu();
	return 0;
}


/*---------------------------------------------------------------------------*/
/**
 * Close the current document.
 */
void CloseCurrentDocument()
{
	Dtk_Document *doc = documents->getCurrentDoc();
	if (doc) {
                documents->removeVisibleDoc(doc);
                if (!doc->isInProject())
                        delete doc;
		UpdateMainMenu();
	}
}


/*---------------------------------------------------------------------------*/
/** 
 * Open one of the currently supported document types.
 */
int OpenDocument(const char *filename)
{
	int ret = -1;

	if (!filename) {
		filename = fl_file_chooser("Open Document...",
			"Layout Files (*.lyt)\tTest Files (*.txt)\tBitmap Files (*.bmp)\t"
			"Sound Files (*.wav)\tPackage Files (*.pkg)\tBook Files (*.lyt)\t"
			"Native Module Files (*.ntm)\tStream Files (*.stm)\tAll Files (*)", 0L);
		if (!filename)
			return ret;
	}
	const char *ext = fl_filename_ext(filename);
	if (strcmp(ext, ".ns")==0) { // Newton Script
		Dtk_Document *doc = documents->newScript(filename);
		doc->load();
		doc->edit();
		ret = 0;
	} else {
		// open our default text editor
		Dtk_Document *doc = documents->newScript(filename);
		doc->load();
		doc->edit();
		ret = 0;
	}

	UpdateMainMenu();
	return ret;
}


/*---------------------------------------------------------------------------*/
/**
 * Save the current document.
 */
int SaveCurrentDocument()
{
	int ret = -1;

	Dtk_Document *doc = documents->getCurrentDoc();
	if (doc)
		ret = doc->save();
	else 
		ret = -1;

	UpdateMainMenu();
	return ret;
}


/*---------------------------------------------------------------------------*/
/**
 * As for a filename and save the current document.
 */
int SaveCurrentDocumentAs()
{
	int ret = -1;

	Dtk_Document *doc = documents->getCurrentDoc();
	if (doc)
		ret = doc->saveAs();
	else 
		ret = -1;

	UpdateMainMenu();
	return ret;
}


/*---------------------------------------------------------------------------*/
/** Open an existing project,
 *
 * If no filename is given, we ask the user for a filename by popping up a 
 * file chooser dialog
 *
 * \todo	Write this function.
 *
 * \param filename	[in] path and file name of .ntk file to open
 *
 * \retval	0 of successful
 * \retval  -1 (or an error code <0) if the function failed
 */
int OpenProject(const char *filename) 
{
	if (!filename) {
		filename = fl_file_chooser("Open Toolkit Project", "*.ntk", 0L);
		if (!filename) 
			return 0;
	}
	if (project) {
		delete project;
		project = 0L;
	}
	project = new Dtk_Project();
	project->setFilename(filename);
	int ret = project->load();

	UpdateMainMenu();
	return ret;
}


/*---------------------------------------------------------------------------*/
/** Create a new project.
 *
 * If no filename is given, we ask the user for a filename by popping up a 
 * file chooser dialog
 *
 * \todo	Write this function.
 *
 * \param filename	[in] path and file name of .ntk file to create
 *
 * \retval	0 of successful
 * \retval  -1 (or an error code <0) if the function failed
 */
int NewProject(const char *filename) 
{
	if (!filename) {
		filename = fl_file_chooser("New Toolkit Project", "*.ntk", 0L);
		if (!filename) 
			return 0;
	}
	CloseProject();
	project = new Dtk_Project();
	project->setFilename(filename);
	project->setDefaults();

	UpdateMainMenu();
	return 0;
}


/*---------------------------------------------------------------------------*/
/**
 * Close the current project and all associted documents.
 */
void CloseProject()
{
	if (!project)
		return;
	// FIXME empty project browser
	// FIXME save and close all open documentsOA
	delete project;
	project = 0L;
	UpdateMainMenu();
}


/*---------------------------------------------------------------------------*/
/**
 * Show the project setting dialog.
 */
void ShowProjectSettings()
{
	dtkProjSettings->updateDialog();
	dtkProjSettings->show();
}


/*---------------------------------------------------------------------------*/
/**
 * Add a new file to the current project.
 */
int AddFileToProject(const char *filename)
{
	assert(project);
	if (!filename) {
		filename = fl_file_chooser("Add File to Project",
			"Layout Files (*.lyt)\tTest Files (*.txt)\tBitmap Files (*.bmp)\t"
			"Sound Files (*.wav)\tPackage Files (*.pkg)\tBook Files (*.lyt)\t"
			"Native Module Files (*.ntm)\tStream Files (*.stm)\tAll Files (*)", 0L);
		if (!filename) 
			return 0;
	}
	Dtk_Document *doc = documents->newScript(filename);
	int ret = doc->load();
	documents->addToProject(doc);
	doc->edit();

	UpdateMainMenu();
	return ret;
}


/*---------------------------------------------------------------------------*/
/**
 * Add the current document to the project.
 */
int AddCurrentDocToProject()
{
	Dtk_Document *doc = documents->getCurrentDoc();
	if (!doc || doc->isInProject())
		return -1;
	documents->addToProject(doc);

	UpdateMainMenu();
	return 0;
}


/*---------------------------------------------------------------------------*/
/**
 * Remove the current file form the project and close the file.
 */
int	RemoveFileFromProject(const char *)
{
	assert(project);
	Dtk_Document *doc = documents->getCurrentDoc();
	if (!doc)
		return -1;
	if (!doc->isInProject())
		return -1;
	CloseCurrentDocument();
	documents->removeProjectDoc(doc);
	delete doc;
	return 0;
}


/*---------------------------------------------------------------------------*/
/**
 * Save the current project to a file.
 */
int SaveProject()
{
	assert(project);
	int ret = project->save();

	UpdateMainMenu();
	return ret;
}


/*---------------------------------------------------------------------------*/
/**
 * Change the filename of the project, then save it.
 */
int SaveProjectAs()
{
	assert(project);
	const char *filename = fl_file_chooser("Save Toolkit Project", "*.ntk", 0L);
	if (!filename) 
		return 0;
	project->setFilename(filename);
	int ret = project->save();

	UpdateMainMenu();
	return ret;
}


/*---------------------------------------------------------------------------*/
/**
 * Compile a package file and save it to disk.
 */
int BuildPackage()
{
	assert(project);
	int err = project->buildPackage();
	if (!err) {
		err = project->savePackage();
	}

	UpdateMainMenu();
	return err;
}


/*---------------------------------------------------------------------------*/
/**
 * Send the current package through the Inspector to the Newton.
 */
int DownloadPackage()
{
	assert(project);
	int ret = InspectorSendPackage(
			project->getPackageName(),
			dtkProjSettings->app->symbol->get());

	UpdateMainMenu();
	return ret;
}


/*---------------------------------------------------------------------------*/
/**
 * Compile and send a script command to the Newton.
 */
int InspectorSendScript(const char *script)
{
	// compile the string
	//NEWT_DUMPBC = 1;
	newtRefVar obj = NBCCompileStr((char*)script, true);
	// FIXME test for error
	//NewtPrintObject(stdout, obj);
	newtRefVar nsof = NsMakeNSOF(0, obj, NewtMakeInt30(2));  
	// FIXME test for error
	//NewtPrintObject(stdout, nsof);

	// if it is a binary, send it to the inspector
	if (NewtRefIsBinary(nsof)) {
		uint32_t size = NewtBinaryLength(nsof);
		uint8_t *data = NewtRefToBinary(nsof);

		wInspectorSerial->send_data_block((unsigned char*)"newt", 4);
		wInspectorSerial->send_data_block((unsigned char*)"ntp ", 4);
		wInspectorSerial->send_data_block((unsigned char*)"lscb", 4);
		unsigned char b1[] = { size>>24, size>>16, size>>8, size };
		wInspectorSerial->send_data_block(b1, 4);
		wInspectorSerial->send_data_block((unsigned char*)data, size);
	}

	return 0;
}


/*---------------------------------------------------------------------------*/
/**
 * Launch the application that corresponds to the current package on the Newton.
 */
int LaunchPackage()
{
	return InspectorLaunchPackage(dtkProjSettings->app->symbol->get());
}


/*---------------------------------------------------------------------------*/
/**
 * Send any package to the Newton.
 */
int InspectorSendPackage(const char *filename, const char *symbol) 
{
	// delete the old package first if that is requested by the user
	if (dtkProjSettings->package->deleteOnDownload->get()) {
		// hack to convert the (usually ASCII) symbol into a UTF16BE
		int sLen = 2*strlen(symbol)+2;
		char *sName = (char*)calloc(sLen, 1);
		const char *s = symbol;
		char *d = sName;
		while (*s) { *d++ = 0; *d++ = *s++; }

		// send "Delete Package"
		wInspectorSerial->send_data_block((unsigned char*)"newt", 4);
		wInspectorSerial->send_data_block((unsigned char*)"ntp ", 4);
		wInspectorSerial->send_data_block((unsigned char*)"pkgX", 4);
		uint32_t len = htonl(sLen);
		wInspectorSerial->send_data_block((unsigned char*)(&len), 4);
		wInspectorSerial->send_data_block((unsigned char*)sName, sLen);

		Sleep(1000); // 1 sec.
	}

	// read the package itself from disk
	uint8_t *buffer;
	FILE *f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	int nn = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer = (uint8_t*)malloc(nn);
	int n = fread(buffer, 1, nn, f);
	fclose(f);

	// send the package
	wInspectorSerial->send_data_block((unsigned char*)"newt", 4);
	wInspectorSerial->send_data_block((unsigned char*)"ntp ", 4);
	wInspectorSerial->send_data_block((unsigned char*)"pkg ", 4);
	uint32_t len = htonl(nn);
	wInspectorSerial->send_data_block((unsigned char*)(&len), 4);
	Sleep(1000);
	wInspectorSerial->send_data_block((unsigned char*)buffer, nn);

	free(buffer);

	return 0;
}


/*---------------------------------------------------------------------------*/
/**
 * Launch the package with the given signature.
 */
int InspectorLaunchPackage(const char *symbol) 
{
	char buf[1024];
	sprintf(buf, "GetRoot().|%s|:Open()", symbol);
	return InspectorSendScript(buf);
}


/*---------------------------------------------------------------------------*/
/**
 * Make the Inspector wait for a connection.
 */
void InspectorConnect()
{
	if (wInspectorSerial->is_open()) {
		wInspectorSerial->close();
	} else {
		char buf[32];
#ifdef WIN32
		sprintf(buf, "\\\\.\\COM%d", dtkPrefs->packages->port()+1);
#else
		sprintf(buf, "/dev/ttyS%d", dtkPrefs->packages->port());
#endif
		wInspectorSerial->open(buf, 38400);
		if (!wInspectorSerial->is_open()) {
			wInspectorSerial->close();
			if (wConnect)
				wConnect->hide();
			SystemAlert("Can't open serial port");
			return;
		}
		if (!wConnect) {
			wConnect = create_connect_dialog();
		}
		wConnect->show();
	}
}


/*---------------------------------------------------------------------------*/
/**
 * Connection was canceled.
 */
void InspectorCancelConnect()
{
	wInspectorSerial->close();
	wInspectorSerial->Flio_Mnp4_Serial::close();
	wConnect->hide();
}


/*---------------------------------------------------------------------------*/
/**
 * Quit and exit.
 * \todo	this should ask for verification if there are unsaved changes.
 */
void ExitApplication() {
	dtkMain->hide();
	while (Fl::first_window()) {
		Fl::first_window()->hide();
	}
}


/*---------------------------------------------------------------------------*/
/**
 * User wants to see the preferences dialog.
 */
void EditPreferences()
{
	dtkPrefs->show();
}

/*---------------------------------------------------------------------------*/
/**
 * Update activation and deactivation in the main menu bar.
 */
void UpdateMainMenu()
{
	unsigned int mask = 0;
	if (project)
		mask |= 1;
	if (documents->numVisibleDocs())
		mask |= 2;
	Dtk_Document *doc = documents->getCurrentDoc();
	if (doc) {
		if (doc->isInProject())
			mask |= 4;
	}
	if (wInspectorSerial->is_open()) {
		mask |= 8;
	}
	dtkMain->activate_menus(mask);
	dtkMain->inspector->activate_menus(mask);
}

/*---------------------------------------------------------------------------*/
/**
 * Toggle the 'breakOnThrows' flag on and off.
 * \todo should be a toggle
 */
void InspectorStopOnThrows()
{
	InspectorSendScript("breakOnThrows := 1"); // TRUE or NIL
}

/*---------------------------------------------------------------------------*/
/**
 * Set the 'trace' flag to "off".
 */
void InspectorTraceOff()
{
	InspectorSendScript("trace := nil");
	// user may have set it to 'functions or true
}

/*---------------------------------------------------------------------------*/
/**
 * Print the contents of the stack.
 */
void InspectorStackTrace()
{
	InspectorSendScript("StackTrace()");
}

/*---------------------------------------------------------------------------*/
/**
 * Leave this level in the break loop.
 * \todo How can we know how deep we are in the break loop?
 */
void InspectorExitBreakLoop()
{
	InspectorSendScript("ExitBreakLoop()");
}

/*---------------------------------------------------------------------------*/
/**
 * Change the depth for printing all kinds of frames.
 * nil shows all levels
 */
void InspectorPrintDepth(int d)
{
	char buf[80];
	sprintf(buf, "printDepth := %d", d);
	InspectorSendScript(buf);
}

/*---------------------------------------------------------------------------*/
/**
 * Take a snapshot from the current screen content and send it to the Inspector.
 */
void InspectorSnapshot()
{
	InspectorSendScript("|Screenshot:ntk|()");
	fl_message("Snapshot is not yet implemented");
}

/*---------------------------------------------------------------------------*/
/**
 * Show a dialog box with the text for the given NewtonOS error code.
 */
void NewtAlert(int err)
{
	newt_alert(err);
}

/*---------------------------------------------------------------------------*/
/**
 * Show a dialog box with the text for the given operrating system error code.
 */
void SystemAlert(const char *msg, int err)
{
	Flmm_Message::alert("%s\n\n%s", msg, Flmm_Message::system_message(err));
}


extern void testPkgReader(const char *filename);
void DebugDumpPackage() {
	NcSetGlobalVar(NSSYM(printLength), NSINT(100));
	NcSetGlobalVar(NSSYM(printDepth), NSINT(20));
	NEWT_INDENT = 1;
	NEWT_DUMPBC = 1;
	testPkgReader(0);
}

//
// End of "$Id$".
//
