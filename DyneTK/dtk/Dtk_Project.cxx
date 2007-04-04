//
// "$Id$"
//
// Dtk_Project implementation for the Dyne Toolkit.
//
// Copyright 2007 by Matthias Melcher.
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
// Please report all bugs and problems to "dtk@matthiasm.com".
//

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include "Dtk_Project.H"
#include "Dtk_Document.H"
#include "Dtk_Document_Manager.H"

#include "fluid/Fldtk_Proj_Settings.h"
#include "fluid/main_ui.h"
#include "main.h"

#include <FL/filename.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>

extern "C" {
#include "NewtCore.h"
#include "NewtBC.h"
#include "NewtNSOF.h"
#include "NewtPkg.h"
#include "NewtPrint.h"
#include "NewtEnv.h"
}


/*---------------------------------------------------------------------------*/
/**
 * Constructor.
 */
Dtk_Project::Dtk_Project()
:	packagename_(0L),
	shortname_(0L),
	filename_(0L),
	name_(0L),
	package_(kNewtRefNIL)
{
	dtkMain->documents->activate();
	dtkMain->browsers->redraw();
}


/*---------------------------------------------------------------------------*/
/**
 * Destructor.
 */
Dtk_Project::~Dtk_Project()
{
	dtkMain->documents->deactivate();
	dtkMain->browsers->redraw();
	if (shortname_)
		free(shortname_);
	if (filename_)
		free(filename_);
	if (packagename_)
		free(packagename_);
}


/*---------------------------------------------------------------------------*/
/** 
 * Change the file name and path for this project.
 */
void Dtk_Project::setFilename(const char *filename)
{
	if (shortname_)
		free(shortname_);
	if (filename_)
		free(filename_);
	if (packagename_)
		free(packagename_);
	if (filename) {
		// save the full filename
		filename_ = strdup(filename);
		// save the full filename, but with .pkg extension
		const char *ext = fl_filename_ext(filename);
		int n = ext-filename;
		packagename_ = (char*)malloc(n+6);
		memcpy(packagename_, filename, n);
		memcpy(packagename_+n, ".pkg", 5);
		// save the name only without the extension
		name_ = (char*)fl_filename_name(filename_);
		ext = fl_filename_ext(name_);
		n = ext-name_;
		shortname_ = (char*)calloc(n+1, 1);
		memcpy(shortname_, name_, n);
	}
	// FIXME I want the name of the current project in the main window title bar!
}


/*---------------------------------------------------------------------------*/
/**
 * Return the name of the package that this project will generate.
 */
char *Dtk_Project::getPackageName()
{
	return packagename_;
}

/* .ntk file format
 * R -> reading this item is completed
 * W -> writing this item is completed
 * D -> setting the default for this item is completed
 * X -> item is fully implemented 
 * U -> will remain unused
	{
RWDU	ntkPlatform: 1, 
RWDU	fileVersion: 2, 
		projectItems: {
			sortOrder: 0, 
			items: [ 
				{ 
					file: { 'fileReference, }, 
					type: 0, 
					isMainLayout: NIL}
				]
			}, 
		projectSettings: {
			PLATFORM: "Newton21", 
			language: "English", 
			debugBuild: TRUE, 
			ignoreNative: NIL, 
			checkglobalfunctions: TRUE, 
			oldBuildRules: NIL, 
			usestepchildren: TRUE, 
			suppressbytecodes: NIL,
			fasterFunctions: TRUE }, 
		outputSettings: {
RWDX		applicationName: "minimal", 
RWDX		applicationSymbol: "Minimal:SIG", 
			partType: 0, 
			topFrameExpression: "", 
RWDU		autoClose: TRUE,  // I can not find the corresponding flag in the Package file format
			customPartType: "UNKN", 
			fasterSoups: NIL, 
			iconpronormal: {
				__ntExternFile: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
				__ntmaskfile: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
				__ntCreateMask: 0, 
				imageinfo1: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
				imageinfo2: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
				imageinfo4: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
				imageinfo8: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
				maskinfo: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
				maskoption: 0}, 
			iconprohighlighted: {
				__ntExternFile: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
				__ntmaskfile: {class: 'fileReference, projectPath: "", deltaFromProject:""}, 
				__ntCreateMask: 0, 
				imageinfo1: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
				imageinfo2: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
				imageinfo4: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
				imageinfo8: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
				maskinfo: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
				maskoption: 0
			}, 
			iconFile: {class: 'fileReference, projectPath: "", deltaFromProject: ""}}, 
		packageSettings: {
RWDX		packageName: "Minimal:SIG", 
RWDX		version: "1", 
RWDX		copyright: "®1997 Apple Computer, Inc. All rights reserved.", 
			optimizeSpeed: TRUE, 
			copyProtected: NIL, 
RWDX		deleteOnDownload: TRUE, 
			dispatchOnly: NIL, 
			fourByteAlignment: TRUE, 
			zippyCompression: TRUE, 
			newton20Only: NIL}, 
		profilerSettings: {
			memory: TRUE,
			percent: TRUE, 
			compileforprofiling: NIL, 
			compileForSpeed: NIL, 
			detailedSystemCalls: NIL, 
			detailedUserFunctions: TRUE}, 
RWDU	windowRect: {left: 3, top: 410, bottom:550, right: 730}
	}


*/

/*---------------------------------------------------------------------------*/
/**
 * Load the .ntk file from disk.
 */
int Dtk_Project::load()
{
	// read the project settings from disk
	FILE *f = fopen(filename_, "rb");
	if (!f) {
		return -1;
	}
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	fseek(f, 0, SEEK_SET);
	uint8_t *data = (uint8_t*)malloc(size);
	fread(data, size, 1, f);
	fclose(f);

	newtRef p = NewtReadNSOF(data, size);

	free(data);

	NewtPrintObject(stdout, p);

	// now extract all supported settings and write them into their locations
	if (NewtRefIsFrame(p)) {
		int32_t ix;
		// output
		newtRef output = NewtGetFrameSlot(p, NewtFindSlotIndex(p, NSSYM(outputSettings)));
		if (NewtRefIsFrame(output)) {
			// applicationName
			newtRef name = NewtGetFrameSlot(output, NewtFindSlotIndex(output, NSSYM(applicationName)));
			if (NewtRefIsString(name))
				dtkProjSettings->app->name->set(NewtRefToString(name));
			// applicationSymbol
			newtRef symbol = NewtGetFrameSlot(output, NewtFindSlotIndex(output, NSSYM(applicationSymbol)));
			if (NewtRefIsString(symbol))
				dtkProjSettings->app->symbol->set(NewtRefToString(symbol));
			// autoClose
			newtRef slot = NewtGetFrameSlot(output, NewtFindSlotIndex(output, NSSYM(autoClose)));
			if (slot==kNewtRefTRUE) dtkProjSettings->app->auto_close->set(1);
			if (slot==kNewtRefNIL) dtkProjSettings->app->auto_close->set(0);
			// partType (int30)
			// topFrameExpression (string)
			// customPartType (string, fourCC)
			// fasterSoups (bool)
			// iconProNormal (Frame)
			// iconProHighlighted (Frame)
			// iconFile (fileRef)
		}
		ix = NewtFindSlotIndex(p, NSSYM(packageSettings));
		if (ix>=0) {
			newtRef packageSettings = NewtGetFrameSlot(p, ix);
			// packageName
			ix = NewtFindSlotIndex(packageSettings, NSSYM(packageName));
			if (ix>=0) {
				newtRef packageName = NewtGetFrameSlot(packageSettings, ix);
				dtkProjSettings->package->name->set(NewtRefToString(packageName));
			}
			// copyright
			ix = NewtFindSlotIndex(packageSettings, NSSYM(copyright));
			if (ix>=0) {
				newtRef copyright = NewtGetFrameSlot(packageSettings, ix);
				dtkProjSettings->package->copyright->set(NewtRefToString(copyright));
			}
			// version
			ix = NewtFindSlotIndex(packageSettings, NSSYM(version));
			if (ix>=0) {
				newtRef version = NewtGetFrameSlot(packageSettings, ix);
				dtkProjSettings->package->version->set(NewtRefToString(version));
			}
			// optimizeSpeed (bool)
			// copyProtected (bool)
			// deleteOnDownload (bool)
			newtRef deleteOnDownload = NewtGetFrameSlot(packageSettings, NewtFindSlotIndex(packageSettings, NSSYM(deleteOnDownload)));
			if (deleteOnDownload==kNewtRefTRUE) dtkProjSettings->package->deleteOnDownload->set(1);
			if (deleteOnDownload==kNewtRefNIL)  dtkProjSettings->package->deleteOnDownload->set(0);
			// dispatchOnly (bool)
			// fourByteAlignment (bool)
			// zippyCompression (bool)
			// newton20Only (bool)
		}
		ix = NewtFindSlotIndex(p, NSSYM(projectItems));
		if (ix>=0) {
			newtRef projectItems = NewtGetFrameSlot(p, ix);
			ix = NewtFindSlotIndex(projectItems, NSSYM(items));
			if (ix>0) {
				newtRef items = NewtGetFrameSlot(projectItems, ix);
				uint32_t i, n = NewtArrayLength(items);
				for (i=0; i<n; i++) {
					// file, type, isMainLayout
					newtRef item = NewtGetArraySlot(items, i);
					int type = -1;
					ix = NewtFindSlotIndex(item, NSSYM(type)); 
					if (ix>=0) {
						newtRef type_r = NewtGetFrameSlot(item, ix);
						type = NewtRefToInteger(type_r);
					}
					ix = NewtFindSlotIndex(item, NSSYM(file)); 
					if (ix>=0) {
						newtRef file = NewtGetFrameSlot(item, ix);
						ix = NewtFindSlotIndex(file, NSSYM(deltaFromProject)); 
						if (ix>=0) {
							newtRef name = NewtGetFrameSlot(file, ix);						
							char *filename = NewtRefToString(name);
							Dtk_Document *doc;
							switch (type) {
								case 0:  doc = documents->newLayout(filename); break;
								case 5:  doc = documents->newScript(filename); break;
								default: doc = documents->newDocument(filename); break;
							}
							doc->load();
							documents->addToProject(doc);
							doc->edit();
						}
					}
				}
			}
		}
	}
	dtkProjSettings->updateDialog();

	return 0;
}


/*---------------------------------------------------------------------------*/
/**
 * Set the default settings for a newly created .ntk project.
 */
void Dtk_Project::setDefaults() 
{
	char buf[80];

	dtkProjSettings->app->name->set(shortname_);
	sprintf(buf, "%s:SIG", shortname_);
	dtkProjSettings->app->symbol->set(buf);
	dtkProjSettings->app->auto_close->set(1);

	sprintf(buf, "%s:SIG", shortname_);
	dtkProjSettings->package->name->set(buf);
	dtkProjSettings->package->copyright->set("©2007. All rights reserved.");
	dtkProjSettings->package->version->set("1");
	dtkProjSettings->package->deleteOnDownload->set(1);
}


/*---------------------------------------------------------------------------*/
/**
 * Create a Newt Frame referencing a file relative to the path of the project.
 */
newtRef Dtk_Project::makeFileRef(const char *filename)
{
	newtRefVar fileReferenceA[] = {
		NSSYM(class),			NSSYM(fileReference),
		NSSYM(projectPath),		kNewtRefNIL,
		NSSYM(deltaFromProject),kNewtRefNIL
	};

	if (filename) {
		char projPath[FL_PATH_MAX];
		char currPath[FL_PATH_MAX];
		char relPath[FL_PATH_MAX];

		strcpy(projPath, filename_);
		char *name = (char*)fl_filename_name(projPath);
		if (name) *name = 0;

		const char *here = getcwd(currPath, FL_PATH_MAX);
		if (here) {
			chdir(projPath);
			fl_filename_relative(relPath, FL_PATH_MAX, filename);
			chdir(currPath);
		} else {
			strcpy(relPath, filename);
		}

#ifdef WIN32
		char *d;
		for (d = projPath; *d; ++d) { if (*d=='/') *d = '\\'; }
		for (d =  relPath; *d; ++d) { if (*d=='/') *d = '\\'; }
#endif
		fileReferenceA[3] = NewtMakeString(projPath, false);
		fileReferenceA[5] = NewtMakeString(relPath, false);
	} else {
		fileReferenceA[3] = NewtMakeString("", false);
		fileReferenceA[5] = NewtMakeString("", false);
	}

	newtRef fileReference = NewtMakeFrame2(sizeof(fileReferenceA) / (sizeof(newtRefVar) * 2), fileReferenceA);
	return fileReference;
}


/*---------------------------------------------------------------------------*/
/**
 * Save the project as an .ntk file to disk.
 */
int Dtk_Project::save() 
{
	newtRef items = documents->getProjectItemsRef();

	newtRefVar projectItemsA[] = {
		NSSYM(sortOrder),			NewtMakeInt30(0),
		NSSYM(items),				items
	};
	newtRef projectItems = NewtMakeFrame2(sizeof(projectItemsA) / (sizeof(newtRefVar) * 2), projectItemsA);

	newtRefVar projectSettingsA[] = {
		NSSYM(platform),			NewtMakeString("Newton21", false),
		NSSYM(language),			NewtMakeString("English", false),
		NSSYM(debugBuild),			kNewtRefTRUE,
		NSSYM(ignoreNative),		kNewtRefNIL,
		NSSYM(checkGlobalFunctions),kNewtRefTRUE,
		NSSYM(oldBuildRules),		kNewtRefNIL,
		NSSYM(useStepChildren),		kNewtRefTRUE,
		NSSYM(suppressByteCodes),	kNewtRefNIL,
		NSSYM(fasterFunctions),		kNewtRefTRUE
	};
	newtRef projectSettings = NewtMakeFrame2(sizeof(projectSettingsA) / (sizeof(newtRefVar) * 2), projectSettingsA);

	newtRefVar profilerSettingsA[] = {
		NSSYM(memory),				kNewtRefTRUE,
		NSSYM(percent),				kNewtRefTRUE,
		NSSYM(compileForProfiling),	kNewtRefNIL,
		NSSYM(compileForSpeed),		kNewtRefNIL,
		NSSYM(detailedSystemCalls),	kNewtRefNIL,
		NSSYM(detailedUserFunctions),kNewtRefTRUE
	};
	newtRef profilerSettings = NewtMakeFrame2(sizeof(profilerSettingsA) / (sizeof(newtRefVar) * 2), profilerSettingsA);

	newtRefVar packageSettingsA[] = {
		NSSYM(packageName),			NewtMakeString(dtkProjSettings->package->name->get(), false),
		NSSYM(version),				NewtMakeString(dtkProjSettings->package->version->get(), false),
		NSSYM(copyright),			NewtMakeString(dtkProjSettings->package->copyright->get(), false),
		NSSYM(optimizeSpeed),		kNewtRefTRUE,
		NSSYM(copyProtected),		kNewtRefNIL,
		NSSYM(deleteOnDownload),	dtkProjSettings->package->deleteOnDownload->get() ? kNewtRefTRUE : kNewtRefNIL,
		NSSYM(dispatchOnly),		kNewtRefNIL,
		NSSYM(fourByteAlignment),	kNewtRefTRUE,
		NSSYM(zippyCompression),	kNewtRefTRUE,
		NSSYM(newton20Only),		kNewtRefNIL
	};
	newtRef packageSettings = NewtMakeFrame2(sizeof(packageSettingsA) / (sizeof(newtRefVar) * 2), packageSettingsA);
		
	newtRefVar iconProNormalA[] = {
		NSSYM(__ntExternFile),		makeFileRef(0), 
		NSSYM(__ntmaskfile),		makeFileRef(0), 
		NSSYM(__ntCreateMask),		NewtMakeInt30(0),
		NSSYM(imageinfo1),			makeFileRef(0), 
		NSSYM(imageinfo2),			makeFileRef(0), 
		NSSYM(imageinfo4),			makeFileRef(0), 
		NSSYM(imageinfo8),			makeFileRef(0), 
		NSSYM(maskinfo),			makeFileRef(0), 
		NSSYM(maskoption),			NewtMakeInt30(0),
	};
	newtRef iconProNormal = NewtMakeFrame2(sizeof(iconProNormalA) / (sizeof(newtRefVar) * 2), iconProNormalA);

	newtRefVar iconProHighlightedA[] = {
		NSSYM(__ntExternFile),		makeFileRef(0), 
		NSSYM(__ntmaskfile),		makeFileRef(0), 
		NSSYM(__ntCreateMask),		NewtMakeInt30(0),
		NSSYM(imageinfo1),			makeFileRef(0), 
		NSSYM(imageinfo2),			makeFileRef(0), 
		NSSYM(imageinfo4),			makeFileRef(0), 
		NSSYM(imageinfo8),			makeFileRef(0), 
		NSSYM(maskinfo),			makeFileRef(0), 
		NSSYM(maskoption),			NewtMakeInt30(0),
	};
	newtRef iconProHighlighted = NewtMakeFrame2(sizeof(iconProHighlightedA) / (sizeof(newtRefVar) * 2), iconProHighlightedA);

	newtRefVar outputSettingsA[] = {
		NSSYM(applicationName),		NewtMakeString(dtkProjSettings->app->name->get(), false),
		NSSYM(applicationSymbol),	NewtMakeString(dtkProjSettings->app->symbol->get(), false),
		NSSYM(partType),			NewtMakeInt30(0),
		NSSYM(topFrameExpression),	NewtMakeString("", false),
		NSSYM(autoClose),			dtkProjSettings->app->symbol->get() ? kNewtRefTRUE : kNewtRefNIL,
		NSSYM(customPartType),		NewtMakeString("UNKN", false),
		NSSYM(fasterSoups),			kNewtRefNIL,
		NSSYM(iconProNormal),		iconProNormal,
		NSSYM(iconProHighlighted),	iconProHighlighted, 
		NSSYM(iconFile),			makeFileRef(0)
	};
	newtRef outputSettings = NewtMakeFrame2(sizeof(outputSettingsA) / (sizeof(newtRefVar) * 2), outputSettingsA);

	// this is the size of the Project subwindow which do not use in DTK, so we give this fixed values
	newtRefVar windowRectA[] = {
		NSSYM(left),				NewtMakeInt30(3),
		NSSYM(top),					NewtMakeInt30(410),
		NSSYM(bottom),				NewtMakeInt30(550),
		NSSYM(right),				NewtMakeInt30(730)
	};
	newtRef windowRect = NewtMakeFrame2(sizeof(windowRectA) / (sizeof(newtRefVar) * 2), windowRectA);

	newtRefVar ntkFrameA[] = {
		NSSYM(ntkPlatform),			NewtMakeInt30(1),
		NSSYM(fileVersion),			NewtMakeInt30(2),
		NSSYM(projectItems),		projectItems,
		NSSYM(projectSettings),		projectSettings,
		NSSYM(outputSettings),		outputSettings,
		NSSYM(packageSettings),		packageSettings,
		NSSYM(profilerSettings),	profilerSettings,
		NSSYM(windowRect),			windowRect
	};
	newtRef ntkFrame = NewtMakeFrame2(sizeof(ntkFrameA) / (sizeof(newtRefVar) * 2), ntkFrameA);

	newtRef rcvr = kNewtRefNIL;
	newtRef nsof = NsMakeNSOF(rcvr, ntkFrame, NewtMakeInt30(2));

	int size = NewtBinaryLength(nsof);
	uint8_t *data = NewtRefToBinary(nsof);
	FILE *f = fopen(filename_, "wb");
	if (f) {
		fwrite(data, size, 1, f);
		fclose(f);
	}

	return -1;
}


/*---------------------------------------------------------------------------*/
/*
 * Default application icon
 */
uint8_t bits[] = {
	0x00, 0x00, 0x00, 0x00, // 0, 0
	0x00, 0x04, 0x00, 0x00, // bytes per row, 0
	0x00, 0x00, 0x00, 0x00, // 0, 4
	0x00, 0x1b, 0x00, 0x18, // Height: 27, Width: 24 (probably need to align to 32 bit)
	0x00, 0x00, 0x00, 0x00, // ................................
	0x00, 0x00, 0x00, 0x00, // ................................
	0x00, 0x00, 0x1c, 0x00, // ...................xxx..........
	0x00, 0x00, 0x3f, 0x00, // ..................xxxxxx........
	0x00, 0x00, 0x33, 0x00, // ..................xx..xx........
	0x1f, 0xff, 0x7b, 0x00, // ...xxxxxxxxxxxxx.xxxx.xx........
	0x3f, 0xff, 0x6e, 0x00, // ..xxxxxxxxxxxxxx.xx.xxx.........
	0x30, 0x00, 0xce, 0x00, // ..xx............xx..xxx.........
	0x37, 0xfc, 0xcc, 0x00, // ..xx.xxxxxxxxx..xx..xx..........
	0x37, 0xfd, 0x9c, 0x00, 
	0x34, 0x05, 0x98, 0x00, // etc. bitmap
	0x35, 0x53, 0x38, 0x00, 
	0x34, 0x03, 0x30, 0x00, 
	0x35, 0x56, 0x70, 0x00, 
	0x34, 0x06, 0x60, 0x00, 
	0x35, 0x54, 0xe0, 0x00, 
	0x34, 0x0c, 0xc0, 0x00, 
	0x35, 0x4f, 0xc0, 0x00, 
	0x34, 0x0b, 0x80, 0x00, 
	0x36, 0x0f, 0x00, 0x00, 
	0x37, 0xfe, 0x80, 0x00, 
	0x37, 0xfd, 0x80, 0x00, 
	0x30, 0x0b, 0x80, 0x00, 
	0x18, 0x03, 0x00, 0x00, 
	0x1f, 0xff, 0x00, 0x00, 
	0x0f, 0xfe, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

/*---------------------------------------------------------------------------*/
/*
 * Default application icon mask bits
 */
uint8_t mask[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x18, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x3f, 0x00, 
	0x00, 0x00, 0x3f, 0x00, 0x1f, 0xff, 0x7f, 0x00, 0x3f, 0xff, 0x7e, 0x00, 0x3f, 0xff, 0xfe, 0x00, 
	0x3f, 0xff, 0xfc, 0x00, 0x3f, 0xff, 0xfc, 0x00, 0x3f, 0xff, 0xf8, 0x00, 0x3f, 0xff, 0xf8, 0x00, 
	0x3f, 0xff, 0xf0, 0x00, 0x3f, 0xff, 0xf0, 0x00, 0x3f, 0xff, 0xe0, 0x00, 0x3f, 0xff, 0xe0, 0x00,
	0x3f, 0xff, 0xc0, 0x00, 0x3f, 0xff, 0xc0, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x3f, 0xff, 0x00, 0x00, 
	0x3f, 0xff, 0x80, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x1f, 0xff, 0x00, 0x00, 
	0x1f, 0xff, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


/*---------------------------------------------------------------------------*/
/** 
 * Create a Frame with the default boilerplate InstallScript()
 */
newtRef makeInstallScript() {

	static uint8_t installScriptBin[] = {
		0x20, 0xa4, 0x70, 0x19, 0x91, 0xa5, 0x7d, 0x1a, 0x29, 0x6f, 0x00, 0x49, 0x7d, 0x22, 0xc7, 0x00, 
		0x11, 0xa7, 0x00, 0x07, 0x5f, 0x00, 0x3c, 0x7f, 0x00, 0x07, 0x24, 0xc2, 0xa6, 0x7e, 0x1b, 0x91,
		0x7b, 0x1b, 0x91, 0xc4, 0x6f, 0x00, 0x38, 0x27, 0x00, 0x0c, 0x1c, 0x1d, 0x1e, 0x28, 0x1f, 0x00, 
		0x07, 0x3b, 0x00, 0x22, 0x5f, 0x00, 0x44, 0x00, 0x7f, 0x00, 0x07, 0x05, 0x7f, 0x00, 0x07, 0x06,
		0x6f, 0x00, 0x17, 0x22, 0x22, 0xa7, 0x00, 0x07, 0x00, 0x7b, 0x1f, 0x00, 0x08, 0x1f, 0x00, 0x09,
		0x2a, 0x6f, 0x00, 0x61, 0x7b, 0x7b, 0x1f, 0x00, 0x08, 0x39, 0x00, 0x7b, 0x1f, 0x00, 0x08, 0x22, 
		0x98, 0x7b, 0x1f, 0x00, 0x0a, 0x22, 0x99, 0x02,
	};
	newtRef instructions = NewtMakeBinary(NSSYM(instructions), installScriptBin, sizeof(installScriptBin), false);

	newtRefVar literalsA[] = {
		NSSYM(vars), NSSYM(extras), NSSYM(IsArray), NSSYM(app), NewtMakeString("Extras Drawer", false), 
		NewtMakeString("The application you just installed conflicts with another application."
		" Please contact the application vendor for an updated version.", false),
		NSSYM(GetRoot), NSSYM(Notify), NSSYM(devInstallScript), NSSYM(HasSlot), NSSYM(installScript)
	};
	newtRef literals = NewtMakeArray2(NSSYM(literals), sizeof(literalsA)/sizeof(newtRefVar), literalsA);

	newtRefVar nextArgFrameA[] = {
		NSSYM(_nextArgFrame),	kNewtRefNIL,
		NSSYM(_parent),			kNewtRefNIL,
		NSSYM(_implementor),	kNewtRefNIL,
	};
	newtRef nextArgFrame = NewtMakeFrame2(sizeof(nextArgFrameA) / (sizeof(newtRefVar) * 2), nextArgFrameA);

	newtRefVar argFrameA[] = {
		NSSYM(_nextArgFrame),	nextArgFrame,
		NSSYM(_parent),			kNewtRefNIL,
		NSSYM(_implementor),	kNewtRefNIL,
		NSSYM(p),				kNewtRefNIL,
		NSSYM(i),				kNewtRefNIL,
		NSSYM(extras),			kNewtRefNIL,
		NSSYM(one),				kNewtRefNIL,
		NSSYM(one|iter),		kNewtRefNIL,
	};
	newtRef argFrame = NewtMakeFrame2(sizeof(argFrameA) / (sizeof(newtRefVar) * 2), argFrameA);

	newtRefVar scriptA[] = {
		NS_CLASS,				NSSYM(CodeBlock),
		NSSYM(instructions),	instructions,
		NSSYM(literals),		literals,
		NSSYM(argFrame),		argFrame,
		NSSYM(numArgs),			NewtMakeInt30(1),
		NSSYM(DebuggerInfo),	kNewtRefNIL
	};
	newtRef script = NewtMakeFrame2(sizeof(scriptA) / (sizeof(newtRefVar) * 2), scriptA);
	return script;
}


/*---------------------------------------------------------------------------*/
/** 
 * Create a Frame with the default uninstall script for packages.
 */
newtRef makeRemoveScript() {

	static uint8_t removeScriptBin[] = {
		0x7b, 0x18, 0x19, 0x2a, 0x6f, 0x00, 0x0e, 0x7b, 0x7b, 
		0x18, 0x39, 0x5f, 0x00, 0x0f, 0x22, 0x02, 
	};
	newtRef instructions = NewtMakeBinary(NSSYM(instructions), removeScriptBin, sizeof(removeScriptBin), false);

	newtRefVar literalsA[] = {
		NSSYM(devRemoveScript), NSSYM(HasSlot)
	};
	newtRef literals = NewtMakeArray2(NSSYM(literals), sizeof(literalsA)/sizeof(newtRefVar), literalsA);

	newtRefVar nextArgFrameA[] = {
		NSSYM(_nextArgFrame),	kNewtRefNIL,
		NSSYM(_parent),			kNewtRefNIL,
		NSSYM(_implementor),	kNewtRefNIL
	};
	newtRef nextArgFrame = NewtMakeFrame2(sizeof(nextArgFrameA) / (sizeof(newtRefVar) * 2), nextArgFrameA);

	newtRefVar argFrameA[] = {
		NSSYM(_nextArgFrame),	nextArgFrame,
		NSSYM(_parent),			kNewtRefNIL,
		NSSYM(_implementor),	kNewtRefNIL,
		NSSYM(p),				kNewtRefNIL
	};
	newtRef argFrame = NewtMakeFrame2(sizeof(argFrameA) / (sizeof(newtRefVar) * 2), argFrameA);
	// FIXME argFrame->map[0] = 0x0a000002 (probably marking the fact that the slot must not besorted?)
	// FIXME seems to work anyways though

	newtRefVar scriptA[] = {
		NS_CLASS,				NSSYM(CodeBlock),
		NSSYM(instructions),	instructions,
		NSSYM(literals),		literals,
		NSSYM(argFrame),		argFrame,
		NSSYM(numArgs),			NewtMakeInt30(1),
		NSSYM(DebuggerInfo),	kNewtRefNIL
	};
	newtRef script = NewtMakeFrame2(sizeof(scriptA) / (sizeof(newtRefVar) * 2), scriptA);
	return script;
}


/*---------------------------------------------------------------------------*/
/**
 * We stumble to hack together a simple package file more or less from scratch... .
 */
int Dtk_Project::buildPackage()
{
	int32_t ix;

	// FIXME: this only supports a single document per project
	newtRef theForm = kNewtRefNIL;
	Dtk_Document *doc = documents->getProjectDoc(0);
	if (doc) {
		theForm = doc->compile();
	}
	NewtPrintObject(stdout, theForm);

	newtRefVar iconBoundsA[] = {
		NSSYM(left),			NewtMakeInt30(0),
		NSSYM(top),				NewtMakeInt30(0),
		NSSYM(bottom),			NewtMakeInt30(27),
		NSSYM(right),			NewtMakeInt30(24)
	};
	newtRef iconBounds = NewtMakeFrame2(sizeof(iconBoundsA) / (sizeof(newtRefVar) * 2), iconBoundsA);
	
	newtRefVar iconA[] = {
		NSSYM(bits),			NewtMakeBinary(NSSYM(bits), bits, sizeof(bits), false),
		NSSYM(mask),			NewtMakeBinary(NSSYM(mask), mask, sizeof(mask), false),
		NSSYM(bounds),			iconBounds,
	};
	newtRef icon = NewtMakeFrame2(sizeof(iconA) / (sizeof(newtRefVar) * 2), iconA);

	newtRefVar dataA[] = {
		NSSYM(app),				NewtMakeSymbol(dtkProjSettings->app->symbol->get()),
		NSSYM(text),			NewtMakeString(dtkProjSettings->app->name->get(), false),
		NSSYM(icon),			icon,
		NSSYM(theForm),			theForm, //+
		NSSYM(installScript),	makeInstallScript(), //+
		NSSYM(removeScript),	makeRemoveScript()  //+
	};
	newtRef data = NewtMakeFrame2(sizeof(dataA) / (sizeof(newtRefVar) * 2), dataA);

	newtRefVar part0A[] = {
		NSSYM(class),			NSSYM(PackagePart),
		NSSYM(info),			NewtMakeBinary(NSSYM(binary), (uint8_t*)"A Newton Toolkit application", /*28*/24, false),
		NSSYM(flags),			NewtMakeInt30(129),
		NSSYM(type),			NewtMakeInt32('form'),
		NSSYM(data),			data
	};
	newtRef part0 = NewtMakeFrame2(sizeof(part0A) / (sizeof(newtRefVar) * 2), part0A);
	
	newtRef parts = NewtMakeArray(kNewtRefUnbind, 1);
	NewtSetArraySlot(parts, 0, part0);

	newtRefVar packageA[] = {
		NSSYM(class),			NSSYM(PackageHeader),
		NSSYM(type),			NewtMakeInt32('xxxx'),
		NSSYM(pkg_version),		NewtMakeInt32(0),
		NSSYM(version),			NewtMakeInt32(atoi(dtkProjSettings->package->version->get())),
		NSSYM(copyright),		NewtMakeString(dtkProjSettings->package->copyright->get(), false),
		NSSYM(name),			NewtMakeString(dtkProjSettings->package->name->get(), false),
		NSSYM(flags),			NewtMakeInt32(0x10000000),
		NSSYM(parts),			parts
	};
	newtRef package = NewtMakeFrame2(sizeof(packageA) / (sizeof(newtRefVar) * 2), packageA);

	newtRef rcvr = kNewtRefNIL;
	package_ = NsMakePkg(rcvr, package);

	NewtPrintObject(stdout, package);
	NewtPrintObject(stdout, package_);

	return 0;
}


/*---------------------------------------------------------------------------*/
/**
 * Save the package into the project path, naming it after the project,
 */
int Dtk_Project::savePackage()
{
	// FIXME write the package it to disk
	int size = NewtBinaryLength(package_);
	uint8_t *data = NewtRefToBinary(package_);

	FILE *f = fopen(getPackageName(), "wb");
	if (f) {
		fwrite(data, size, 1, f);
		fclose(f);
	}


	return -1;
}

//
// End of "$Id$".
//
