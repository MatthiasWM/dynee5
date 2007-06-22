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
#include "fluid/Fldtk_Snapshot.h"
#include "fluid/main_ui.h"

#include "dtk/Dtk_Document_List.h"
#include "dtk/Dtk_Document.h"
#include "dtk/Dtk_Layout_Document.h"
#include "dtk/Dtk_Project.h"
#include "dtk/Dtk_Error.h"
#include "dtk/Dtk_Script_Writer.h"

#include "fltk/Flmm_Message.H"
#include "fltk/Flio_Serial_Port.h"
#include "fltk/Fldtk_Editor.h"
#include "fltk/Fldtk_Layout_View.h"

#include "globals.h"
#include "allNewt.h"

#include <assert.h>

#ifndef WIN32
# define Sleep(a) sleep((a)/1000)
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
#else
# include <winsock2.h>
# include <io.h>
# include <fcntl.h>
#endif

extern Fl_Window *wConnect;

/*-v2------------------------------------------------------------------------*/
int NewLayoutFile(const char *filename) {
    // create a filename if we have none
    if (!filename) {
        filename = "layout.lyt";
        /// \todo Make sure we choose a unique name here
        /// \todo Add the current path to the filename
        /// \todo Make sure that no file with that path and name exists
    }
    // add the new document to the global docs list
	Dtk_Document *doc = dtkGlobalDocuments->newLayout(filename);
	doc->edit();
    // keep the menubar in sync
	UpdateMainMenu();
	return 0;
}


/*-v2------------------------------------------------------------------------*/
int NewTextFile(const char *filename) {
    // create a filename if we have none
    if (!filename) {
        filename = "script.txt";
        /// \todo Make sure we choose a unique name here
        /// \todo Add the current path to the filename
        /// \todo Make sure that no file with that path and name exists
    }
    // add the new document to the global docs list
	Dtk_Document *doc = dtkGlobalDocuments->newScript(filename);
	doc->edit();
    // keep the menubar in sync
	UpdateMainMenu();
	return 0;
}


/*-v2------------------------------------------------------------------------*/
int CloseCurrentDocument()
{
    // find the right document
    Dtk_Document *doc = GetCurrentDocument();
    if (!doc)
        return -1;
    // if the document is dirty, we must ask the user before closing
    if (doc->isDirty()) {
        int v = fl_choice(
            "Save changes to document %s?", 
            "Abort", "Yes", "No", doc->name());
        switch (v) {
        case 0: // abort
            return -2; 
        case 1: // yes
            doc->save(); break; 
        case 2:  // no
            break;
        }
    }
    // If we are part of a project, only close the editor, else discard the doc.
    if (doc->project()) {
        doc->close();
    } else {
        delete doc;
    }
    // correct menus
    UpdateMainMenu();
    return 0;
}


/*-v2------------------------------------------------------------------------*/
int OpenDocument(const char *filename)
{
	int ret = -1;
    // if there is no file name, pop up a file dialog
	if (!filename) {
		filename = fl_file_chooser("Open Document...",
            "All Files(*)\t"
			"Layout Files (*.lyt)\tTest Files (*.txt)\tBitmap Files (*.bmp)\t"
			"Sound Files (*.wav)\tPackage Files (*.pkg)\tBook Files (*.lyt)\t"
			"Native Module Files (*.ntm)\tStream Files (*.stm)", 0L);
		if (!filename)
			return -2;
	}
    // go ahead and add the document
    Dtk_Document *doc = dtkGlobalDocuments->add(filename);
    // update the menu bar
	UpdateMainMenu();
    // show an error message if needed
    if (!doc) {
        SystemAlert("Can't open document");
        return -1;
    }
	return ret;
}


/*-v2------------------------------------------------------------------------*/
int SaveCurrentDocument()
{
	int ret = -1;
    // find the current document and save it
	Dtk_Document *doc = GetCurrentDocument();
	if (doc)
		ret = doc->save();
	else 
		ret = -1;
    // activate the correct menus
	UpdateMainMenu();
	return ret;
}


/*-v2------------------------------------------------------------------------*/
int SaveCurrentDocumentAs()
{
	int ret = -1;
    // find the current document and save it
	Dtk_Document *doc = GetCurrentDocument();
	if (doc)
		ret = doc->saveAs();
	else 
		ret = -1;
    // activate the correct menus
	UpdateMainMenu();
	return ret;
}


/*-v2------------------------------------------------------------------------*/
int OpenProject(const char *filename) 
{
    // close any dirty project first
    if (dtkProject && dtkProject->isDirty()) {
        CloseProject();
        // abort if the user decided to not close the project
        if (dtkProject) {
            return -2;
        }
    }
    // open a file chooser if there was no filename given
	if (!filename) {
		filename = fl_file_chooser("Open Toolkit Project", "*.ntk", 0L);
		if (!filename) 
			return -2;
	}
    // now close the existing project, which can not be dirty
	if (dtkProject) {
        CloseProject();
	}
    // create a new project and load it
	dtkProject = new Dtk_Project();
	dtkProject->setFilename(filename);
	int ret = dtkProject->load();

	UpdateMainMenu();
	return ret;
}

/*-v2------------------------------------------------------------------------*/
int SaveAllDocuments()
{
	if (dtkProject) {
        return dtkProject->saveAll();
	}
    return 0;
}

/*-v2------------------------------------------------------------------------*/
int NewProject(const char *filename) 
{
    // close any dirty project first
    if (dtkProject && dtkProject->isDirty()) {
        CloseProject();
        // abort if the user decided to not close the project
        if (dtkProject) {
            return -2;
        }
    }
    // ask for a new filename where we will save this project, if none was given
	if (!filename) {
		filename = fl_file_chooser("New Toolkit Project", "*.ntk", 0L);
		if (!filename) 
			return 0;
	}
    // confirm that we want to overwrite this project file if it exists already
    if (access(filename, 0004)==0) { // R_OK
        int v = fl_choice(
            "A file with that filename already exist. Creating "
            "a new project will eventually delete the original file.\n\n"
            "Do you want to delete the file\n%s?", 
            "delete file", "keep file", 0L, filename);
        if (v==1)
            return -1;
    }
    // if there is still a (non-dirty) project, close it now
    if (dtkProject)
	    CloseProject();
    // finally we can create a brandnew project 
	dtkProject = new Dtk_Project();
	dtkProject->setFilename(filename);
	dtkProject->setDefaults();
    // make sure the menus show the right settings
	UpdateMainMenu();
	return 0;
}


/*-v2------------------------------------------------------------------------*/
int CloseProject()
{
    // avoid failure
	if (!dtkProject)
		return -1;
    // confirm command if project is dirty
    if (dtkProject->isDirty()) {
        int v = fl_choice(
            "Save changes to project %s?", 
            "Abort", "Yes", "No", dtkProject->name());
        switch (v) {
        case 0: // abort
            return -2; 
        case 1: // yes
            dtkProject->saveAll(); break; 
        case 2:  // no
            break;
        }
    }
    // now close the project and remove all references
    dtkProject->close();
	delete dtkProject;
	dtkProject = 0L;
	UpdateMainMenu();
    return 0;
}


/*-v2------------------------------------------------------------------------*/
void ShowProjectSettings()
{
    if (!dtkProject)
        return;
	dtkProjSettings->updateDialog();
	dtkProjSettings->show();
}


/*-v2------------------------------------------------------------------------*/
int AddFileToProject(const char *filename)
{
    if (!dtkProject)
        return -1;
    // if there is no file name, pop up a file dialog
	if (!filename) {
		filename = fl_file_chooser("Open Document...",
            "All Files(*)\t"
			"Layout Files (*.lyt)\tTest Files (*.txt)\tBitmap Files (*.bmp)\t"
			"Sound Files (*.wav)\tPackage Files (*.pkg)\tBook Files (*.lyt)\t"
			"Native Module Files (*.ntm)\tStream Files (*.stm)", 0L);
		if (!filename)
			return -2;
	}
    // go ahead and add the document to the project
    Dtk_Document *doc = dtkProject->documentList()->add(filename);
    // update the menu bar
	UpdateMainMenu();
    // show an error message if needed
    if (!doc) {
        SystemAlert("Can't open document");
        return -1;
    }
	return 0;
}

/*-v2------------------------------------------------------------------------*/
int AddCurrentDocToProject()
{
    if (!dtkProject)
        return -1;
    // which document are we talking about?
    Dtk_Document *doc = GetCurrentDocument();
    if (!doc)
        return -1;
    // if it is already in the project, don't bother
    if (doc->project())
        return 0;
    // remove from globals and reattach to the project
    dtkGlobalDocuments->remove(doc);
    dtkProject->documentList()->append(doc);
    // update the menus
	UpdateMainMenu();
	return 0;
}


/*-v2------------------------------------------------------------------------*/
int	RemoveFileFromProject(Dtk_Document *doc)
{
    // use the current document if there is none specified
    if (!doc) {
        doc = GetCurrentDocument();
        if (!doc) 
            return -1;
    }
    // see if it is part of a project
    if (!doc->project())
        return -1;
    // if the document is dirty, we must ask the user before deleting
    if (doc->isDirty()) {
        int v = fl_choice(
            "Save changes to document %s?", 
            "Abort", "Yes", "No", doc->name());
        switch (v) {
        case 0: // abort
            return -2; 
        case 1: // yes
            doc->save(); break; 
        case 2:  // no
            break;
        }
    }
    // Now we can delete the document. It will unhook itself from lists and the GUI
    delete doc;
    return 0;
}


/*-v2------------------------------------------------------------------------*/
int SaveProject()
{
    if (!dtkProject)
        return -1;
    // save it
	int ret = dtkProject->save();
    if (ret!=0)
        SystemAlert("Unable to save prject file!");
    // keep the menus cool
	UpdateMainMenu();
	return ret;
}


/*-v2------------------------------------------------------------------------*/
int SaveProjectAs()
{
    if (!dtkProject)
        return -1;
    // get the filename form the user
	const char *filename = fl_file_chooser("Save Toolkit Project", "*.ntk", 0L);
	if (!filename) 
		return -2;
    // save it
	dtkProject->setFilename(filename);
	int ret = dtkProject->save();
    if (ret!=0)
        SystemAlert("Unable to save project file!");
    // keep the menus cool
	UpdateMainMenu();
	return ret;
}


/*-v2------------------------------------------------------------------------*/
int BuildPackage()
{
    if (!dtkProject)
        return -1;
	int err = dtkProject->buildPackage();
	if (!err) {
		err = dtkProject->savePackage();
	}
	UpdateMainMenu();
	return err;
}


/*-v2------------------------------------------------------------------------*/
int DownloadPackage()
{
	assert(dtkProject);
	int ret = InspectorSendPackage(
			dtkProject->getPackageName(),
			dtkProjSettings->app->symbol->get());
	UpdateMainMenu();
	return ret;
}


/*-v2------------------------------------------------------------------------*/
int ExportPackageToText()
{
	assert(dtkProject);
    Dtk_Script_Writer sw(dtkProject);
    int ret = sw.open("testing_script_writer.txt");
    if (ret) {
        SystemAlert("Can't open file.");
        return ret;
    }
    ret = dtkProject->write(sw);
    if (ret) {
        SystemAlert("Can't write to file.");
        return ret;
    }
	return 0;
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


/*-v2------------------------------------------------------------------------*/
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
		wInspectorSerial->open(dtkPrefs->packages->port(), 38400);
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
	wInspectorSerial->Flio_Mnp4_Protocol::close();
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
    // bits in the mask have the following meanings:
    //	bit 0: a project is loaded
    //	bit 1: a document is active for editing
    //	bit 2: the active document is part of the project
    //  bit 3: Inspector is connected to a Newton device
    //  bit 4: a layout is active for editing
    //  bit 5: the current layout has a visible layout view
	unsigned int mask = 0;
	if (dtkProject)
		mask |= 1;
    Dtk_Document *doc = GetCurrentDocument();
    if (doc) {
		mask |= 2;
        if (doc->project()) {
			mask |= 4;
        }
        if (doc->isLayout()) {
            mask |= 16;
            Dtk_Layout_Document *layout = (Dtk_Layout_Document*)doc;
            if (layout->editViewShown())
                mask |= 32;
        }
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
	show_snapshot_window();
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

/*---------------------------------------------------------------------------*/
/**
 * Convert the Newt Frame into an Fl_Image and update the snapshot dialog.
 */
void InspectorSnapshotUpdate(newtRef snapshot) 
{
	if (NewtRefIsFrame(snapshot)) {
		newtRef data = NewtGetFrameSlot(snapshot, NewtFindSlotIndex(snapshot, NSSYM(data)));
		if (NewtRefIsFrame(data)) {

      // get the dimensions and data
      newtRef nRowbytes = NewtGetFrameSlot(data, NewtFindSlotIndex(data, NSSYM(rowbytes)));
      newtRef nTop      = NewtGetFrameSlot(data, NewtFindSlotIndex(data, NSSYM(top)));
      newtRef nLeft     = NewtGetFrameSlot(data, NewtFindSlotIndex(data, NSSYM(left)));
      newtRef nBottom   = NewtGetFrameSlot(data, NewtFindSlotIndex(data, NSSYM(bottom)));
      newtRef nRight    = NewtGetFrameSlot(data, NewtFindSlotIndex(data, NSSYM(right)));
      newtRef nDepth    = NewtGetFrameSlot(data, NewtFindSlotIndex(data, NSSYM(depth)));
      newtRef nTheBits  = NewtGetFrameSlot(data, NewtFindSlotIndex(data, NSSYM(theBits)));
      int rowbytes = NewtRefToInteger(nRowbytes);
      int top      = NewtRefToInteger(nTop);
      int left     = NewtRefToInteger(nLeft);
      int bottom   = NewtRefToInteger(nBottom);
      int right    = NewtRefToInteger(nRight);
      int depth    = NewtRefToInteger(nDepth);
      unsigned char *theBits = (unsigned char*)NewtRefToData(nTheBits);

      if (depth!=4) {
        printf("Unsupported pixel depth of %d bits\n", depth);
        return;
      }
      // convert this to RGB (ah well)
      int wdt = right - left, hgt = bottom - top, x, y;
      unsigned char *dst = new unsigned char[wdt*hgt*1], *imgData = dst;
      for (y=0; y<hgt; y++) {
        unsigned char *src = theBits + y*rowbytes;
        for (x=0; x<wdt; x+=2) {
          unsigned char d = *src++;
          *dst++ = ((d&0xf0)|(d>>4))^0xff;
          *dst++ = ((d<<4)|(d&0x0f))^0xff;
        }
      }
      Flmp_Image *img = (Flmp_Image*)new Fl_RGB_Image(imgData, wdt, hgt, 1);
      img->alloc_array = 1;
      update_snapshot_window(img);
      //free(imgData);
		}
	}
}

extern void testPkgReader(const char *filename);
void DebugDumpPackage() {
	NcSetGlobalVar(NSSYM(printLength), NSINT(100));
	NcSetGlobalVar(NSSYM(printDepth), NSINT(20));
	NEWT_INDENT = 1;
	NEWT_DUMPBC = 1;
	testPkgReader(0);
}

extern void testNSOFReader(const char *filename);
void DebugDumpNSOF() {
	testNSOFReader(0);
}

void DebugDumpRsrc() {
  const char *filename = fl_file_chooser("Read a resource fork", 0, 0);
  if (!filename)
    return;
  char buf[1024];
  sprintf(buf, "%s/rsrc", filename);
  int fd = open(buf, O_RDONLY /*| O_NO_MFS*/, 0);
  if (fd==-1) {
    SystemAlert("Can't open resource fork");
  }
  for (;;) {
    unsigned char c;
    int n = read(fd, &c, 1);
    if (n!=1) break;
    printf("%02x ", c);
  }
  close(fd);
}

void DebugDumpBuffer(uint8_t *src, int n)
{
        int i, j, nLine = n/16;
        for (i=0; i<n; i+=16) {
                char buf[78];
                memset(buf, 32, 78); buf[76] = ' '; buf[77] = 0;
                sprintf(buf, "%08x", i); buf[8] = ' ';
                for (j=i; (j<i+16) && (j<n); j++) {
                        uint8_t c = src[j];
                        sprintf(buf+10+3*(j-i), "%02x", c); buf[12+3*(j-i)] = ' ';
                        if (isprint(c))
                                buf[59+j-i] = c;
                        else
                                buf[59+j-i] = '.';
                }
                puts(buf);
        }
}

/*---------------------------------------------------------------------------*/
Dtk_Document *GetCurrentDocument()
{
    Fldtk_Editor *ed = (Fldtk_Editor*)dtkDocumentTabs->value();
    if (!ed) 
        return 0L;
    return ed->document();
}

/*---------------------------------------------------------------------------*/
int OpenLayoutView(Dtk_Layout_Document *lyt)
{
    if (!lyt) {
        Dtk_Document *doc = GetCurrentDocument();
        if (!doc || !doc->isLayout())
            return -1;
        lyt = (Dtk_Layout_Document*)doc;
    }
    lyt->editView();
    return 0;
}

/*---------------------------------------------------------------------------*/
int SetMainLayout(Dtk_Document *doc)
{
    if (!doc) {
        doc = GetCurrentDocument();
        if (!doc) 
            return -1;
    }
    if (!doc->project())
        return -2;
    doc->setMain();
    return 0;
}

/*---------------------------------------------------------------------------*/
void SetModeEditTemplate()
{
    Fldtk_Layout_View::mode(0);
    UpdateMainMenu();
}

/*---------------------------------------------------------------------------*/
void SetModeAddTemplate()
{
    Fldtk_Layout_View::mode(1);
    UpdateMainMenu();
}


//
// End of "$Id$".
//
